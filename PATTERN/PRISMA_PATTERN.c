/* Freescale includes. */
#include <stdio.h>
#include <math.h>
#include "board.h"
#include "fsl_pit.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "fsl_ftm.h"
//#include "peripherals.h"
#include "fsl_device_registers.h"
#include "fsl_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_PIT_BASEADDR PIT
#define DEMO_PIT_CHANNEL  kPIT_Chnl_0
#define PIT_LED_HANDLER   PIT0_IRQHandler
#define PIT_IRQ_ID        PIT0_IRQn
/* Get source clock for PIT driver */
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
#define BOARD_FTM_BASEADDR FTM3
#define BOARD_FTM_CHANNEL_blue  kFTM_Chnl_1
#define BOARD_FTM_CHANNEL_green  kFTM_Chnl_3
#define BOARD_FTM_CHANNEL_red  kFTM_Chnl_2
#define FTM_INTERRUPT_NUMBER FTM3_IRQn
#define FTM_LED_HANDLER      FTM3_IRQHandler
#define FTM_CHANNEL_INTERRUPT_ENABLE kFTM_Chnl1InterruptEnable
#define FTM_CHANNEL_FLAG             kFTM_Chnl1Flag
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
//#ifndef FTM_PWM_ON_LEVEL
#define FTM_PWM_ON_LEVEL kFTM_LowTrue

#define RED_OFF 0
#define GREEN_OFF 0
#define BLUE_OFF 0
#define _MODE_          12

QueueHandle_t config_queue = NULL;
QueueHandle_t response_queue = NULL;


ftm_config_t ftmInfo;
ftm_chnl_pwm_signal_param_t ftmParam[3];
ftm_pwm_level_select_t pwmLevel = FTM_PWM_ON_LEVEL;
pit_config_t pitConfig;
uart_config_t config;

/* pit flag*/
volatile bool pitIsrFlag = false;

enum error_code {
	Queue_receive = 4001, pwm_fail = 4002, task_fail = 4003
};

void PIT_LED_HANDLER(void) {
	/* Clear interrupt flag.*/
	PIT_ClearStatusFlags(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL, kPIT_TimerFlag);
	pitIsrFlag = true;
	/* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
	 * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
	 * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
	 */
	__DSB();
}

//Send current colour to Task_1
void update_color(int i, int j, int k) {

	int response[4];
	response[2] = i;
	response[1] = j;
	response[0] = k;
	xQueueSend(response_queue, &response, 0);

	return;
}

//Send error code to Task_1
void update_error(int error_code) {
	//PRINTF("\r\n%d ERROR\r\n", error_code);
	xQueueSend(response_queue, &error_code, 0);
}

//Configure ftm with updated pwm duty
void configure_ftm(int duty_red, int duty_green, int duty_blue) {
	if ((kStatus_Success
			!= FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR,
					(ftm_chnl_t) BOARD_FTM_CHANNEL_red, kFTM_EdgeAlignedPwm,
					duty_red))
			|| (kStatus_Success
					!= FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR,
							(ftm_chnl_t) BOARD_FTM_CHANNEL_green,
							kFTM_EdgeAlignedPwm, duty_green))
							|| (kStatus_Success
									!= FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR,
											(ftm_chnl_t) BOARD_FTM_CHANNEL_blue,
											kFTM_EdgeAlignedPwm, duty_blue))) {
		//PRINTF("Update duty cycle fail, the target duty cycle may out of range!\r\n");
		update_error(pwm_fail);
	}
	/* Software trigger to update registers */
	FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
	return;
}

void delay_pattern(int time) {
	volatile uint32_t i = 0U;
	for (i = 0U; i < time; ++i) {
		__asm("NOP");
	/* delay */}
}

int Auto_up(int *config_buffer, int *control_buffer) {
	while (1) {
		int RATE = config_buffer[0], SCHEME = config_buffer[1], START_RED =
				config_buffer[2], START_GREEN = config_buffer[3], START_BLUE =
						config_buffer[4], END_RED = config_buffer[5], END_GREEN =
								config_buffer[6], END_BLUE = config_buffer[7], STEP_RED =
										config_buffer[8], STEP_GREEN = config_buffer[9], STEP_BLUE =
												config_buffer[10], CHANGE_RATE = config_buffer[11], MODE =
														config_buffer[_MODE_], CYCLE;
		int period = (CHANGE_RATE * 10000); //LDVAL trigger = (period / clock period) -1
		int PWM_FREQUENCY = (100000 / RATE); //Frequency = (1/user_entered value(10*10^-6))    // =(10^6/(user_entered_value*10) = (10^5/user_entered_value)
		int X, Y, Z;
		int duty_red = 0;
		int duty_green = 0;
		int duty_blue = 0;
		int start_flag = 0;
		int pause_flag = 0;
		if (config_buffer[1] == 1) {
			X = Y = 7;
			Z = 3;
		} else if (config_buffer[1] == 2) {
			X = Y = Z = 15;
		} else if (config_buffer[1] == 3) {
			X = Y = Z = 255;
		}
		int i, j, k;
		PIT_GetDefaultConfig(&pitConfig);
		/* Init pit module */
		PIT_Init(DEMO_PIT_BASEADDR, &pitConfig);
		/* Enable timer interrupts for channel 0 */
		PIT_EnableInterrupts(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL,
				kPIT_TimerInterruptEnable);
		/* Enable at the NVIC */
		EnableIRQ(PIT_IRQ_ID);

		PIT_SetTimerPeriod(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL,
				USEC_TO_COUNT(period, PIT_SOURCE_CLOCK));
		// PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);
		FTM_GetDefaultConfig(&ftmInfo);
		/* Calculate the clock division based on the PWM frequency to be obtained */
		ftmInfo.prescale = FTM_CalculateCounterClkDiv(BOARD_FTM_BASEADDR,
				PWM_FREQUENCY, FTM_SOURCE_CLOCK);
		/* Initialize FTM module */
		FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);
		/* Configure ftm params with frequency 24kHZ */
		ftmParam[0].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_red;
		ftmParam[0].level = 1;
		ftmParam[0].dutyCyclePercent = 0;
		ftmParam[0].firstEdgeDelayPercent = 0U;
		ftmParam[0].enableComplementary = false;
		ftmParam[0].enableDeadtime = false;
		ftmParam[1].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_green;
		ftmParam[1].level = 1;
		ftmParam[1].dutyCyclePercent = 0;
		ftmParam[1].firstEdgeDelayPercent = 0U;
		ftmParam[1].enableComplementary = false;
		ftmParam[1].enableDeadtime = false;
		ftmParam[2].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_blue;
		ftmParam[2].level = 1;
		ftmParam[2].dutyCyclePercent = 0;
		ftmParam[2].firstEdgeDelayPercent = 0U;
		ftmParam[2].enableComplementary = false;
		ftmParam[2].enableDeadtime = false;
		if (kStatus_Success
				!= FTM_SetupPwm(BOARD_FTM_BASEADDR, ftmParam, 3U,
						kFTM_EdgeAlignedPwm, PWM_FREQUENCY, FTM_SOURCE_CLOCK)) {
			//PRINTF("\r\nSetup PWM fail, please check the configuration parameters!\r\n");
			update_error(pwm_fail);
			return -1;
		}
		FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);
		CYCLE = config_buffer[13];
		while (xQueuePeek(config_queue, control_buffer, 0) != pdPASS) {
			//PRINTF("\r\n TEST_1\r\n");
			__asm("NOP");
		}
		if (control_buffer[0] != 0) {
			break;
		} else if (control_buffer[0] == 0) {
			if (xQueueReceive(config_queue, control_buffer, 0) != pdPASS) {
				//PRINTF("\r\nQueue Receive failed \r\n");
				update_error(Queue_receive);
			}
		}
		if (control_buffer[1] == 's') {
			start_flag = 1;
		} else {
			start_flag = 0;
			break;
		}
		/* Start channel 0 */
		PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);
		while (CYCLE) {
			if (STEP_RED == 0)
				STEP_RED++;
			for (int i = START_RED; i <= END_RED; i += STEP_RED) {
				if (STEP_GREEN == 0)
					STEP_GREEN++;
				for (int j = START_GREEN; j <= END_GREEN; j += STEP_GREEN) {
					if (STEP_BLUE == 0)
						STEP_BLUE++;
					for (int k = START_BLUE; k <= END_BLUE; k += STEP_BLUE) {
						duty_red = (100 / X) * i;
						duty_green = (100 / Y) * j;
						duty_blue = (100 / Z) * k;
						configure_ftm(duty_red, duty_green, duty_blue);
						update_color(i, j, k);
						while (pitIsrFlag == false) {
							if (xQueuePeek(config_queue, control_buffer,
									0) == pdPASS) {
								if (control_buffer[0] != 0) {
									configure_ftm( RED_OFF, GREEN_OFF, BLUE_OFF);
									start_flag = 1;
									break;
								} else if (control_buffer[0] == 0) {
									xQueueReceive(config_queue, control_buffer,
											0);
									if (control_buffer[1] == 's') {
										configure_ftm( RED_OFF, GREEN_OFF, BLUE_OFF);
										start_flag = 1;
										break;
									} else if (control_buffer[1] == 'p') {
										if (pause_flag == 1) {
											PIT_StartTimer(DEMO_PIT_BASEADDR,
													DEMO_PIT_CHANNEL);
											pause_flag = 0;
										} else {
											pause_flag = 1;
											PIT_StopTimer(DEMO_PIT_BASEADDR,
													DEMO_PIT_CHANNEL);
										}
									}
								}
							}
						}
						pitIsrFlag = 0;
						//taskYIELD();
					}
				}
			}
			duty_red = (100 / X) * END_RED;
			duty_red = (100 / (Y)) * END_GREEN;
			duty_red = (100 / Z) * END_BLUE;
			configure_ftm(duty_red, duty_green, duty_blue);
			update_color(i, j, k);
			if (config_buffer[13] == 0) {
				CYCLE = 1;
			} else {
				CYCLE--;
			}
			taskYIELD();
		}
		break;
	}
	return 0;
}

int Auto_down(int *config_buffer, int *control_buffer) {
	while (1) {
		int RATE = config_buffer[0], SCHEME = config_buffer[1], START_RED =
				config_buffer[2], START_GREEN = config_buffer[3], START_BLUE =
						config_buffer[4], END_RED = config_buffer[5], END_GREEN =
								config_buffer[6], END_BLUE = config_buffer[7], STEP_RED =
										config_buffer[8], STEP_GREEN = config_buffer[9], STEP_BLUE =
												config_buffer[10], CHANGE_RATE = config_buffer[11], MODE =
														config_buffer[_MODE_], CYCLE;
		int period = ((CHANGE_RATE * 10 * PIT_SOURCE_CLOCK) - 1);
		int PWM_FREQUENCY = (100000 / RATE); //Frequency = (1/user_entered value(10*10^-6))
		// =(10^6/(user_entered_value*10) = (10^5/user_entered_value)
				int X, Y, Z;
		int duty_red = 0;
		int duty_green = 0;
		int duty_blue = 0;
		int start_flag = 0;
		int pause_flag = 0;
		if (config_buffer[1] == 1) {
			X = Y = 7;
			Z = 3;
		} else if (config_buffer[1] == 2) {
			X = Y = Z = 15;
		} else if (config_buffer[1] == 3) {
			X = Y = Z = 255;
		}
		int i, j, k;
		PIT_SetTimerPeriod(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL,
				USEC_TO_COUNT(period, PIT_SOURCE_CLOCK));
		// PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);
		FTM_GetDefaultConfig(&ftmInfo);
		/* Calculate the clock division based on the PWM frequency to be obtained */
		ftmInfo.prescale = FTM_CalculateCounterClkDiv(BOARD_FTM_BASEADDR,
				PWM_FREQUENCY, FTM_SOURCE_CLOCK);
		/* Initialize FTM module */
		FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);
		/* Configure ftm params with frequency 24kHZ */
		ftmParam[0].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_red;
		ftmParam[0].level = 1;
		ftmParam[0].dutyCyclePercent = 0;
		ftmParam[0].firstEdgeDelayPercent = 0U;
		ftmParam[0].enableComplementary = false;
		ftmParam[0].enableDeadtime = false;
		ftmParam[1].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_green;
		ftmParam[1].level = 1;
		ftmParam[1].dutyCyclePercent = 0;
		ftmParam[1].firstEdgeDelayPercent = 0U;
		ftmParam[1].enableComplementary = false;
		ftmParam[1].enableDeadtime = false;
		ftmParam[2].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_blue;
		ftmParam[2].level = 1;
		ftmParam[2].dutyCyclePercent = 0;
		ftmParam[2].firstEdgeDelayPercent = 0U;
		ftmParam[2].enableComplementary = false;
		ftmParam[2].enableDeadtime = false;
		if (kStatus_Success
				!= FTM_SetupPwm(BOARD_FTM_BASEADDR, ftmParam, 3U,
						kFTM_EdgeAlignedPwm, PWM_FREQUENCY, FTM_SOURCE_CLOCK)) {
			//PRINTF("\r\nSetup PWM fail, please check the configuration parameters!\r\n");
			update_error(pwm_fail);
			return -1;
		}
		FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);
		CYCLE = config_buffer[13];
		while (xQueuePeek(config_queue, control_buffer, 0) != pdPASS) {
			PRINTF("\r\n TEST_2\r\n");
			__asm("NOP");
		}
		PRINTF("\r\n SECOND \r\n");
		if (control_buffer[0] != 0) {
			break;
		} else if (control_buffer[0] == 0) {
			if (xQueueReceive(config_queue, control_buffer, 0) != pdPASS) {
				//PRINTF("\r\nQueue Receive failed \r\n");
				update_error(Queue_receive);
			}
		}
		if (control_buffer[1] == 's') {
			start_flag = 1;
		} else {
			start_flag = 0;
			break;
		}
		/* Start channel 0 */
		PRINTF("\r\nStarting channel No.0 ...");
		PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);
		while (CYCLE) {
			if (STEP_RED == 0)
				STEP_RED--;
			for (int i = END_RED; i >= END_RED; i -= START_RED) {
				if (STEP_GREEN == 0)
					STEP_GREEN--;
				for (int j = END_GREEN; j >= START_GREEN; j -= STEP_GREEN) {
					if (STEP_BLUE == 0)
						STEP_BLUE--;
					for (int k = END_BLUE; k >= START_BLUE; k -= STEP_BLUE) {
						duty_red = (100 / X) * i;
						duty_green = (100 / Y) * j;
						duty_blue = (100 / Z) * k;
						//PRINTF("\r\n%d, %d, %d\r\n", duty_red , duty_green, duty_blue);
						configure_ftm(duty_red, duty_green, duty_blue);
						update_color(i, j, k);
						/////////////////////////////////////////////////////////////////////////////////////////////////////////
						while (false == pitIsrFlag) {
							if (xQueuePeek(config_queue, control_buffer,
									0) == pdPASS) {
								if (control_buffer[0] != 0) {
									configure_ftm( RED_OFF, GREEN_OFF,
											BLUE_OFF);
									start_flag = 1;
									break;
								} else if (control_buffer[0] == 0) {
									xQueueReceive(config_queue, control_buffer,
											0);
									if (control_buffer[1] == 's') {
										configure_ftm( RED_OFF, GREEN_OFF,
												BLUE_OFF);
										start_flag = 1;
										break;
									} else if (control_buffer[1] == 'p') {
										if (pause_flag == 1) {
											PIT_StartTimer(DEMO_PIT_BASEADDR,
													DEMO_PIT_CHANNEL);
											pause_flag = 0;
										} else {
											pause_flag = 1;
											PIT_StopTimer(DEMO_PIT_BASEADDR,
													DEMO_PIT_CHANNEL);
										}
									}
								}
							}
						}
						pitIsrFlag = 0;
						///////////////////////////////////////////////////////////////////////////////////////////
					}
				}
			}
			duty_red = (100 / X) * START_RED;
			duty_red = (100 / (Y)) * START_GREEN;
			duty_red = (100 / Z) * START_BLUE;
			configure_ftm(duty_red, duty_green, duty_blue);
			update_color(i, j, k);
			if (config_buffer[13] == 0) {
				CYCLE = 1;
			} else {
				CYCLE--;
			}
			taskYIELD();
		}
		break;
	}
	return 0;
}

int Auto_up_down(int *config_buffer, int *control_buffer) {
	while (1) {
		int RATE = config_buffer[0], SCHEME = config_buffer[1], START_RED =
				config_buffer[2], START_GREEN = config_buffer[3], START_BLUE =
						config_buffer[4], END_RED = config_buffer[5], END_GREEN =
								config_buffer[6], END_BLUE = config_buffer[7], STEP_RED =
										config_buffer[8], STEP_GREEN = config_buffer[9], STEP_BLUE =
												config_buffer[10], CHANGE_RATE = config_buffer[11], MODE =
														config_buffer[_MODE_], CYCLE;
		int PWM_FREQUENCY = (100000 / RATE);
		int X, Y, Z;
		int duty_red = 0;
		int duty_green = 0;
		int duty_blue = 0;
		int start_flag = 0;
		int pause_flag = 0;
		if (config_buffer[1] == 1) {
			X = Y = 7;
			Z = 3;
		} else if (config_buffer[1] == 2) {
			X = Y = Z = 4;
		} else if (config_buffer[1] == 3) {
			X = Y = Z = 8;
		}
		int i, j, k;
		FTM_GetDefaultConfig(&ftmInfo);
		/* Calculate the clock division based on the PWM frequency to be obtained */
		ftmInfo.prescale = FTM_CalculateCounterClkDiv(BOARD_FTM_BASEADDR,
				PWM_FREQUENCY, FTM_SOURCE_CLOCK);
		/* Initialize FTM module */
		FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);
		/* Configure ftm params with frequency 24kHZ */
		ftmParam[0].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_red;
		ftmParam[0].level = 1;
		ftmParam[0].dutyCyclePercent = 0;
		ftmParam[0].firstEdgeDelayPercent = 0U;
		ftmParam[0].enableComplementary = false;
		ftmParam[0].enableDeadtime = false;
		ftmParam[1].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_green;
		ftmParam[1].level = 1;
		ftmParam[1].dutyCyclePercent = 0;
		ftmParam[1].firstEdgeDelayPercent = 0U;
		ftmParam[1].enableComplementary = false;
		ftmParam[1].enableDeadtime = false;
		ftmParam[2].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_blue;
		ftmParam[2].level = 1;
		ftmParam[2].dutyCyclePercent = 0;
		ftmParam[2].firstEdgeDelayPercent = 0U;
		ftmParam[2].enableComplementary = false;
		ftmParam[2].enableDeadtime = false;
		if (kStatus_Success
				!= FTM_SetupPwm(BOARD_FTM_BASEADDR, ftmParam, 3U,
						kFTM_EdgeAlignedPwm, PWM_FREQUENCY,
						FTM_SOURCE_CLOCK)) {
			update_error(pwm_fail);
			return -1;
		}
		FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);
		CYCLE = config_buffer[13];
		while (xQueuePeek(config_queue, control_buffer, 0) != pdPASS) {
			PRINTF("\r\n TEST_\r\n");
			__asm("NOP");
		}
		if (control_buffer[0] != 0) {
			break;
		} else if (control_buffer[0] == 0) {
			if (xQueueReceive(config_queue, control_buffer, 0) != pdPASS) {
				//PRINTF("\r\nQueue Receive failed \r\n");
				update_error(Queue_receive);
			}
		}
		if (control_buffer[1] == 's') {
			start_flag = 1;
		} else {
			start_flag = 0;
			break;
		}
		/* Start channel 0 */
		PRINTF("\r\nStarting channel No.0 ...");
		PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);
		while (CYCLE) {
			if (STEP_RED == 0)
				STEP_RED++;
			for (int i = START_RED; i <= END_RED; i += STEP_RED) {
				if (STEP_GREEN == 0)
					STEP_GREEN++;
				for (int j = START_GREEN; j <= END_GREEN; j += STEP_GREEN) {
					if (STEP_BLUE == 0)
						STEP_BLUE++;
					for (int k = START_BLUE; k <= END_BLUE; k += STEP_BLUE) {
						duty_red = (100 / X) * i;
						duty_green = (100 / Y) * j;
						duty_blue = (100 / Z) * k;
						/* Start PWM mode with updated duty cycle */
						configure_ftm(duty_red, duty_green, duty_blue);
						update_color(i, j, k);
						while (false == pitIsrFlag) {
							if (xQueuePeek(config_queue, control_buffer,
									0) == pdPASS) {
								if (control_buffer[0] != 0) {
									configure_ftm( RED_OFF, GREEN_OFF,
											BLUE_OFF);
									start_flag = 1;
									break;
								} else if (control_buffer[0] == 0) {
									xQueueReceive(config_queue, control_buffer,
											0);
									if (control_buffer[1] == 's') {
										configure_ftm( RED_OFF, GREEN_OFF,
												BLUE_OFF);
										start_flag = 1;
										break;
									} else if (control_buffer[1] == 'p') {
										if (pause_flag == 1) {
											PIT_StartTimer(
													DEMO_PIT_BASEADDR,
													DEMO_PIT_CHANNEL);
											pause_flag = 0;
										} else {
											pause_flag = 1;
											PIT_StopTimer(DEMO_PIT_BASEADDR,
													DEMO_PIT_CHANNEL);
										}
									}
								}
							}
						}
						pitIsrFlag = 0;
					}
				}
			}
			duty_red = (100 / X) * END_RED;
			duty_green = (100 / (Y)) * END_GREEN;
			duty_blue = (100 / Z) * END_BLUE;
			configure_ftm(duty_red, duty_green, duty_blue);
			update_color(i, j, k);
			delay(8000000);
			if (STEP_RED == 0)
				STEP_RED--;
			for (int i = END_RED; i >= START_RED; i -= STEP_RED) {
				if (STEP_GREEN == 0)
					STEP_GREEN--;
				for (int j = END_GREEN; j >= START_GREEN; j -= STEP_GREEN) {
					if (STEP_BLUE == 0)
						STEP_BLUE--;
					for (int k = END_BLUE; k >= START_BLUE; k -= STEP_BLUE) {
						duty_red = (100 / X) * i;
						duty_green = (100 / Y) * j;
						duty_blue = (100 / Z) * k;
						/* Start PWM mode with updated duty cycle */
						configure_ftm(duty_red, duty_green, duty_blue);
						update_color(i, j, k);
						while (false == pitIsrFlag) {
							if (xQueuePeek(config_queue, control_buffer,
									0) == pdPASS) {
								if (control_buffer[0] != 0) {
									configure_ftm( RED_OFF, GREEN_OFF,
											BLUE_OFF);
									start_flag = 1;
									break;
								} else if (control_buffer[0] == 0) {
									xQueueReceive(config_queue, control_buffer,
											0);
									if (control_buffer[1] == 's') {
										configure_ftm( RED_OFF, GREEN_OFF,
												BLUE_OFF);
										start_flag = 1;
										break;
									} else if (control_buffer[1] == 'p') {
										if (pause_flag == 1) {
											PIT_StartTimer(
													DEMO_PIT_BASEADDR,
													DEMO_PIT_CHANNEL);
											pause_flag = 0;
										} else {
											pause_flag = 1;
											PIT_StopTimer(DEMO_PIT_BASEADDR,
													DEMO_PIT_CHANNEL);
										}
									}
								}
							}
						}
						pitIsrFlag = 0;
					}
				}
			}
			duty_red = (100 / X) * START_RED;
			duty_green = (100 / (Y)) * START_GREEN;
			duty_blue = (100 / Z) * START_BLUE;
			configure_ftm(duty_red, duty_green, duty_blue);
			update_color(i, j, k);
			if (config_buffer[13] == 0) {
				CYCLE = 1;
			} else {
				CYCLE--;
			}
			if (start_flag == 0) {
				break;
			}
		}
		break;
	}
	return 0;
}

int Manual_mode(int *config_buffer, int *control_buffer) {
	while (1) {
		int RATE = config_buffer[0], SCHEME = config_buffer[1], START_RED =
				config_buffer[2], START_GREEN = config_buffer[3], START_BLUE =
						config_buffer[4], END_RED = config_buffer[5], END_GREEN =
								config_buffer[6], END_BLUE = config_buffer[7], STEP_RED =
										config_buffer[8], STEP_GREEN = config_buffer[9], STEP_BLUE =
												config_buffer[10];
		int current_colour[3];
		int PWM_FREQUENCY = (100000 / RATE);
		int X, Y, Z;
		int duty_red = 0;
		int duty_green = 0;
		int duty_blue = 0;
		int start_flag = 0;
		if (config_buffer[1] == 1) {
			X = Y = 7;
			Z = 3;
		} else if (config_buffer[1] == 2) {
			X = Y = Z = 4;
		} else if (config_buffer[1] == 3) {
			X = Y = Z = 8;
		}
		if (control_buffer[1] == 1) {
		}
		int i, j, k, sign, loop = 1;
		FTM_GetDefaultConfig(&ftmInfo);
		/* Calculate the clock division based on the PWM frequency to be obtained */
		ftmInfo.prescale = FTM_CalculateCounterClkDiv(BOARD_FTM_BASEADDR,
				PWM_FREQUENCY, FTM_SOURCE_CLOCK);
		/* Initialize FTM module */
		FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);
		/* Configure ftm params with frequency 24kHZ */
		ftmParam[0].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_red;
		ftmParam[0].level = 1;
		ftmParam[0].dutyCyclePercent = 0;
		ftmParam[0].firstEdgeDelayPercent = 0U;
		ftmParam[0].enableComplementary = false;
		ftmParam[0].enableDeadtime = false;
		ftmParam[1].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_green;
		ftmParam[1].level = 1;
		ftmParam[1].dutyCyclePercent = 0;
		ftmParam[1].firstEdgeDelayPercent = 0U;
		ftmParam[1].enableComplementary = false;
		ftmParam[1].enableDeadtime = false;
		ftmParam[2].chnlNumber = (ftm_chnl_t) BOARD_FTM_CHANNEL_blue;
		ftmParam[2].level = 1;
		ftmParam[2].dutyCyclePercent = 0;
		ftmParam[2].firstEdgeDelayPercent = 0U;
		ftmParam[2].enableComplementary = false;
		ftmParam[2].enableDeadtime = false;
		if (kStatus_Success
				!= FTM_SetupPwm(BOARD_FTM_BASEADDR, ftmParam, 3U,
						kFTM_EdgeAlignedPwm, PWM_FREQUENCY,
						FTM_SOURCE_CLOCK)) {
			//PRINTF("\r\nSetup PWM fail, please check the configuration parameters!\r\n");
			update_error(4004);
			return -1;
		}
		FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);
		while (1) {
			//  Current_colour[0] = config_buffer[2];
			//  Current_colour[1] = config_buffer[3];
			//  Current_colour[2] = config_buffer[4];
			while (xQueuePeek(config_queue, control_buffer, 0) != pdPASS) {
				PRINTF("\r\nSREE\r\n");
				__asm("NOP");
			}
			if (control_buffer[0] != 0) {
				start_flag = 0;
				break;
			} else if (control_buffer[1] == 's' || 'S') {
				xQueueReceive(config_queue, control_buffer, 0);
				if (start_flag == 0) {
					start_flag = 1;
					break;
				} else {
					start_flag = 0;
					//continue;
				}
			}
		}
		if (start_flag == 0)
			break;
		int Current_red = config_buffer[2];
		int Current_green = config_buffer[3];
		int Current_blue = config_buffer[4];
		while (1) {
			while (xQueuePeek(config_queue, control_buffer, 0) != pdPASS) {
				__asm("NOP");
			}
			if (control_buffer[0] != 0) {
				start_flag = 0;
				break;
			} else if (control_buffer[1] == '>') {
				while (1) {
					if (STEP_RED == 0)
						STEP_RED++;
					for (int i = Current_red; i <= END_RED; i += STEP_RED) {
						if (STEP_GREEN == 0)
							STEP_GREEN++;
						for (int j = Current_green; j <= END_GREEN; j +=
								STEP_GREEN) {
							if (STEP_BLUE == 0)
								STEP_BLUE++;
							for (int k = Current_blue; k <= END_BLUE; k +=
									STEP_BLUE) {
								duty_red = (100 / X) * i;
								duty_green = (100 / Y) * j;
								duty_blue = (100 / Z) * k;
								//PRINTF("\r\n%d, %d, %d\r\n", duty_red , duty_green, duty_blue);
								configure_ftm(duty_red, duty_green, duty_blue);
								update_color(i, j, k);
							}
						}
					}
					if (Current_red == END_RED && Current_green == END_GREEN
							&& Current_blue == END_BLUE) {
						Current_red = config_buffer[2];
						Current_green = config_buffer[3];
						Current_blue = config_buffer[4];
						continue;
					} else {
						break;
					}
				}
			} else if (control_buffer[1] == '<') {
				while (1) {
					if (STEP_RED == 0)
						STEP_RED--;
					for (int i = Current_red; i >= START_RED; i -= STEP_RED) {
						if (STEP_GREEN == 0)
							STEP_GREEN--;
						for (int j = Current_green; j >= START_GREEN; j -=
								STEP_GREEN) {
							if (STEP_BLUE == 0)
								STEP_BLUE--;
							for (int k = Current_blue; k >= START_BLUE; k -=
									STEP_BLUE) {
								duty_red = (100 / X) * i;
								duty_green = (100 / Y) * j;
								duty_blue = (100 / Z) * k;
								//PRINTF("\r\n%d, %d, %d\r\n", duty_red , duty_green, duty_blue);
								configure_ftm(duty_red, duty_green, duty_blue);
								update_color(i, j, k);
							}
						}
					}
					if (Current_red == START_RED && Current_green == START_GREEN
							&& Current_blue == START_BLUE) {
						Current_red = config_buffer[5];
						Current_green = config_buffer[6];
						Current_blue = config_buffer[7];
						continue;
					} else {
						break;
					}
				}
			} else {
				continue;
			}
		}
	}
	return 0;
}









