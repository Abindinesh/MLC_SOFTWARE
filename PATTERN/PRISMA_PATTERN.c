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
#include "queue.h"


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

extern QueueHandle_t config_queue = NULL;
extern QueueHandle_t response_queue = NULL;

extern int queue_peek();
extern int queue_receive();
extern void update_error();
extern int update_queue();
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

//Send error code to Task_1


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
    int start_flag = 0;
            int pause_flag = 0;
            int pattern_count = 0;
    while (1) {
        int RATE = config_buffer[0], SCHEME = config_buffer[1], START_RED =
                config_buffer[2], START_GREEN = config_buffer[3], START_BLUE =
                config_buffer[4], END_RED = config_buffer[5], END_GREEN =
                config_buffer[6], END_BLUE = config_buffer[7], STEP_RED =
                config_buffer[8], STEP_GREEN = config_buffer[9], STEP_BLUE =
                config_buffer[10], CHANGE_RATE = config_buffer[11], MODE =
                config_buffer[_MODE_], CYCLE;
        int period = (CHANGE_RATE * 10000); //LDVAL trigger = (period / clock period) -1
        int PWM_FREQUENCY = (100000 / RATE); //Frequency = (1/user_entered value(10*10^-6))                          // =(10^6/(user_entered_value*10) = (10^5/user_entered_value)
        int DIVISOR_RED, DIVISOR_GREEN, DIVISOR_BLUE;
        int duty_red = 0;
        int duty_green = 0;
        int duty_blue = 0;
        if (SCHEME == 1) {
            DIVISOR_RED = DIVISOR_GREEN = 7;
            DIVISOR_BLUE = 3;
        } else if (SCHEME == 2) {
            DIVISOR_RED = DIVISOR_GREEN = DIVISOR_BLUE = 15;
        } else if (SCHEME == 3) {
            DIVISOR_RED = DIVISOR_GREEN = DIVISOR_BLUE = 255;
        }
        int current_red, current_green, current_blue;
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
        //PRINTF("\r\nStarting channel No.0 ...");
        PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);
        CYCLE = config_buffer[13];
        //while(1){
        do {
            if (STEP_RED == 0)
                STEP_RED++;
            for (int current_red = START_RED; current_red <= END_RED;
                    current_red += STEP_RED) {
                if (STEP_GREEN == 0)
                    STEP_GREEN++;
                for (int current_green = START_GREEN;
                        current_green <= END_GREEN; current_green +=
                                STEP_GREEN) {
                    if (STEP_BLUE == 0)
                        STEP_BLUE++;
                    for (int current_blue = START_BLUE;
                            current_blue <= END_BLUE; current_blue +=
                                    STEP_BLUE) {
                        duty_red = (100 / DIVISOR_RED) * current_red;
                        duty_green = (100 / DIVISOR_GREEN) * current_green;
                        duty_blue = (100 / DIVISOR_BLUE) * current_blue;
                        //PRINTF("\r\n%d, %d, %d\r\n", duty_red , duty_green, duty_blue);
                        configure_ftm(duty_red, duty_green, duty_blue);
                        update_queue(current_red, current_green, current_blue, pattern_count);
                        /////////////////////////////////////////////////////////////////////////////////////////////////////////
                        while (false == pitIsrFlag) {
                            if (xQueuePeek(config_queue, control_buffer,
                                    0) == pdPASS) {
                                if (control_buffer[0] != 0) {
                                    configure_ftm( RED_OFF, GREEN_OFF,
                                    BLUE_OFF);
                                    //current_red, current_green, current_blue, pattern_count =0;
                                    //update_queue(0,0,0,0);
                                    start_flag = 0;
                                    break;
                                } else if (control_buffer[0] == 0) {
                                    xQueueReceive(config_queue, control_buffer,
                                            0);
                                    if (control_buffer[1] == 's') {
                                        configure_ftm( RED_OFF, GREEN_OFF,
                                        BLUE_OFF);
                                        //current_red, current_green, current_blue, pattern_count = 0;
                                        update_queue(0,0,0,0);
                                        start_flag = 0;
                                        break;
                                    } else if (control_buffer[1] == 'd') {
                                        if (pause_flag == 0) {
                                            PRINTF("RoM");
                                            pitIsrFlag = 0;
                                            PIT_StopTimer(DEMO_PIT_BASEADDR,
                                            DEMO_PIT_CHANNEL);
                                            pause_flag = 1;
                                            //break;
                                        } else   {
                                            PRINTF("RAM");
                                            pause_flag = 0;
                                            pitIsrFlag = 0;
                                            PIT_StartTimer(DEMO_PIT_BASEADDR,
                                            DEMO_PIT_CHANNEL);
                                        }
                                    }
                                }
                            }
                        }
                        if(start_flag == 0){
                            //configure_ftm( RED_OFF, GREEN_OFF, BLUE_OFF);
                            break;
                        }
                        pitIsrFlag = 0;
                        ///////////////////////////////////////////////////////////////////////////////////////////
                    }
                }
            }
            if (current_red != END_RED) {
                current_red = END_RED;
                duty_red = (100 / DIVISOR_RED) * current_red;
            }
            if (current_green != END_GREEN) {
                current_green = END_GREEN;
                duty_red = (100 / (DIVISOR_GREEN)) * current_green;
            }
            if (current_blue != END_BLUE) {
                current_blue = END_BLUE;
                duty_red = (100 / DIVISOR_BLUE) * current_blue;
            }
            pattern_count++;
            update_queue(current_red, current_green, current_blue, pattern_count);
if(CYCLE !=0){
    CYCLE--;
}
        }while(CYCLE);
        configure_ftm( RED_OFF, GREEN_OFF, BLUE_OFF);
        update_queue(current_red, current_green, current_blue, pattern_count);
    //}
        break;
    }
    //configure_ftm( RED_OFF, GREEN_OFF, BLUE_OFF);
    return 0;
}
int Auto_down(int *config_buffer, int *control_buffer) {
	while (1) {
		int RATE = config_buffer[0], SCHEME = config_buffer[1], START_RED =
				config_buffer[2], START_GREEN = config_buffer[3], START_BLUE =
						config_buffer[4], END_RED = config_buffer[5], END_GREEN =
								config_buffer[6], END_BLUE = config_buffer[7], STEP_RED =
										config_buffer[8], STEP_GREEN = config_buffer[9], STEP_BLUE =
												config_buffer[10], CHANGE_RATE = config_buffer[11], CYCLE;
		int period = (CHANGE_RATE * 10000); //LDVAL trigger = (period / clock period) -1
		int PWM_FREQUENCY = (100000 / RATE); //Frequency = (1/user_entered value(10*10^-6))                          // =(10^6/(user_entered_value*10) = (10^5/user_entered_value)
		int DIVISOR_RED, DIVISOR_GREEN, DIVISOR_BLUE;
		int duty_red = 0;
		int duty_green = 0;
		int duty_blue = 0;
		int start_flag;
		int pause_flag = 0;
		if (SCHEME == 1) {
			DIVISOR_RED = DIVISOR_GREEN = 7;
			DIVISOR_BLUE = 3;
		} else if (SCHEME == 2) {
			DIVISOR_RED = DIVISOR_GREEN = DIVISOR_BLUE = 15;
		} else if (SCHEME == 3) {
			DIVISOR_RED = DIVISOR_GREEN = DIVISOR_BLUE = 255;
		}
		int current_red, current_green, current_blue;
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
		while (queue_peek(control_buffer)) {
			//PRINTF("\r\n TEST_1\r\n");
			__asm("NOP");
		}
		if (control_buffer[0] != 0) {
			break;
		} else if (control_buffer[0] == 0) {
			if (queue_receive(control_buffer)) {
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
		//PRINTF("\r\nStarting channel No.0 ...");
		PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);
		CYCLE = config_buffer[13];
		do {
			if (STEP_RED == 0)
				STEP_RED--;
			for (int current_red = END_RED; current_red >= START_RED;
					current_red -= STEP_RED) {
				if (STEP_GREEN == 0)
					STEP_GREEN--;
				for (int current_green = END_GREEN;
						current_green >= START_GREEN; current_green -=
								STEP_GREEN) {
					if (STEP_BLUE == 0)
						STEP_BLUE++;
					for (int current_blue = END_BLUE;
							current_blue >= START_BLUE; current_blue -=
									STEP_BLUE) {
						duty_red = (100 / DIVISOR_RED) * current_red;
						duty_green = (100 / DIVISOR_GREEN) * current_green;
						duty_blue = (100 / DIVISOR_BLUE) * current_blue;
						//PRINTF("\r\n%d, %d, %d\r\n", duty_red , duty_green, duty_blue);
						configure_ftm(duty_red, duty_green, duty_blue);
						update_queue(current_red, current_green, current_blue);
						/////////////////////////////////////////////////////////////////////////////////////////////////////////
						while (false == pitIsrFlag) {
							if (queue_peek(control_buffer)) {
								if (control_buffer[0] != 0) {
									configure_ftm( RED_OFF, GREEN_OFF,
											BLUE_OFF);
									start_flag = 1;
									break;
								} else if (control_buffer[0] == 0) {
									queue_receive(control_buffer);
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
			if (current_red != START_RED) {
				current_red = START_RED;
				duty_red = (100 / DIVISOR_RED) * current_red;
			}
			if (current_green != START_GREEN) {
				current_green = START_GREEN;
				duty_red = (100 / (DIVISOR_GREEN)) * current_green;
			}
			if (current_blue != START_BLUE) {
				current_blue = START_BLUE;
				duty_red = (100 / DIVISOR_BLUE) * current_blue;
				configure_ftm(duty_red, duty_green, duty_blue);
				update_queue(current_red, current_green, current_blue);
			}
			if(CYCLE != 0) {
				CYCLE--;
			}
		}while(CYCLE);
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
												config_buffer[10], CHANGE_RATE = config_buffer[11], CYCLE;
		int period = (CHANGE_RATE * 10000);
		int PWM_FREQUENCY = (100000 / RATE);
		int DIVISOR_RED, DIVISOR_GREEN, DIVISOR_BLUE;
		int duty_red = 0;
		int duty_green = 0;
		int duty_blue = 0;
		int start_flag;
		int pause_flag = 0;
		if (SCHEME == 1) {
			DIVISOR_RED = DIVISOR_GREEN = 7;
			DIVISOR_BLUE = 3;
		} else if (SCHEME == 2) {
			DIVISOR_RED = DIVISOR_GREEN = DIVISOR_BLUE = 4;
		} else if (SCHEME == 3) {
			DIVISOR_RED = DIVISOR_GREEN = DIVISOR_BLUE = 8;
		}
		int current_red, current_green, current_blue;
		PIT_SetTimerPeriod(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL,
				USEC_TO_COUNT(period, PIT_SOURCE_CLOCK));
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
		//CYCLE = config_buffer[13];
		while (queue_peek(control_buffer)) {
			PRINTF("\r\n TEST_3\r\n");
			__asm("NOP");
		}
		if (control_buffer[0] != 0) {
			break;
		} else if (control_buffer[0] == 0) {
			if (queue_receive(control_buffer)) {
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
		//PRINTF("\r\nStarting channel No.0 ...");
		PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);
		CYCLE = config_buffer[13];
		do {
			if (STEP_RED == 0)
				STEP_RED++;
			for (int current_red = START_RED; current_red <= END_RED;
					current_red += STEP_RED) {
				if (STEP_GREEN == 0)
					STEP_GREEN++;
				for (int current_green = START_GREEN;
						current_green <= END_GREEN; current_green +=
								STEP_GREEN) {
					if (STEP_BLUE == 0)
						STEP_BLUE++;
					for (int current_blue = START_BLUE;
							current_blue <= END_BLUE; current_blue +=
									STEP_BLUE) {
						duty_red = (100 / DIVISOR_RED) * current_red;
						duty_green = (100 / DIVISOR_GREEN) * current_green;
						duty_blue = (100 / DIVISOR_BLUE) * current_blue;
						/* Start PWM mode with updated duty cycle */
						configure_ftm(duty_red, duty_green, duty_blue);
						update_queue(current_red, current_green, current_blue);
						while (false == pitIsrFlag) {
							if (queue_peek(control_buffer)) {
								if (control_buffer[0] != 0) {
									configure_ftm( RED_OFF, GREEN_OFF,
											BLUE_OFF);
									start_flag = 1;
									break;
								} else if (control_buffer[0] == 0) {
									queue_receive(control_buffer);
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
			if (current_red != END_RED) {
				current_red = END_RED;
				duty_red = (100 / DIVISOR_RED) * current_red;
			}
			if (current_green != END_GREEN) {
				current_green = END_GREEN;
				duty_red = (100 / (DIVISOR_GREEN)) * current_green;
			}
			if (current_blue != END_BLUE) {
				current_blue = END_BLUE;
				duty_red = (100 / DIVISOR_BLUE) * current_blue;
			}
			configure_ftm(duty_red, duty_green, duty_blue);
			update_queue(current_red, current_green, current_blue);
			delay_pattern(8000000);
			if (STEP_RED == 0)
				STEP_RED--;
			for (int current_red = END_RED; current_red >= START_RED;
					current_red -= STEP_RED) {
				if (STEP_GREEN == 0)
					STEP_GREEN--;
				for (int current_green = END_GREEN;
						current_green >= START_GREEN; current_green -=
								STEP_GREEN) {
					if (STEP_BLUE == 0)
						STEP_BLUE--;
					for (int current_blue = END_BLUE;
							current_blue >= START_BLUE; current_blue -=
									STEP_BLUE) {
						duty_red = (100 / DIVISOR_RED) * current_red;
						duty_green = (100 / DIVISOR_GREEN) * current_green;
						duty_blue = (100 / DIVISOR_BLUE) * current_blue;
						/* Start PWM mode with updated duty cycle */
						configure_ftm(duty_red, duty_green, duty_blue);
						update_queue(current_red, current_green, current_blue);
						while (false == pitIsrFlag) {
							if (queue_peek(control_buffer)) {
								if (control_buffer[0] != 0) {
									configure_ftm( RED_OFF, GREEN_OFF,
											BLUE_OFF);
									start_flag = 1;
									break;
								} else if (control_buffer[0] == 0) {
									queue_receive(control_buffer);
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
			if (current_red != START_RED) {
				current_red = START_RED;
				duty_red = (100 / DIVISOR_RED) * current_red;
			}
			if (current_green != START_GREEN) {
				current_green = START_GREEN;
				duty_red = (100 / (DIVISOR_GREEN)) * current_green;
			}
			if (current_blue != START_BLUE) {
				current_blue = START_BLUE;
				duty_red = (100 / DIVISOR_BLUE) * current_blue;
			}
			configure_ftm(duty_red, duty_green, duty_blue);
			update_queue(current_red, current_green, current_blue);
			if(CYCLE != 0) {
				CYCLE--;
			}
		}while(CYCLE);
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
        int PWM_FREQUENCY = (100000 / RATE);
        int DIVISOR_RED, DIVISOR_GREEN, DIVISOR_BLUE;
        int duty_red = 0;
        int duty_green = 0;
        int duty_blue = 0;
        int start_flag;
        if (SCHEME == 1) {
            DIVISOR_RED = DIVISOR_GREEN = 7;
            DIVISOR_BLUE = 3;
        } else if (SCHEME == 2) {
            DIVISOR_RED = DIVISOR_GREEN = DIVISOR_BLUE = 4;
        } else if (SCHEME == 3) {
            DIVISOR_RED = DIVISOR_GREEN = DIVISOR_BLUE = 8;
        }
        if (control_buffer[1] == 1) {
        }
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
            while (queue_peek(control_buffer)) {
                PRINTF("\r\nSREE\r\n");
                __asm("NOP");
            }
            if (control_buffer[0] != 0) {
                start_flag = 0;
                break;
            } else if (control_buffer[1] == 's' || 'S') {
            	queue_receive(control_buffer);
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
            while (queue_peek(control_buffer)) {
                __asm("NOP");
            }
            if (control_buffer[0] != 0) {
                start_flag = 0;
                break;
            } else if (control_buffer[1] == '>') {
                while (1) {
                    if (STEP_RED == 0)
                        STEP_RED++;
                    for (int current_red = Current_red; current_red <= END_RED;
                            current_red += STEP_RED) {
                        if (STEP_GREEN == 0)
                            STEP_GREEN++;
                        for (int current_green = Current_green;
                                current_green <= END_GREEN; current_green +=
                                        STEP_GREEN) {
                            if (STEP_BLUE == 0)
                                STEP_BLUE++;
                            for (int current_blue = current_blue;
                                    current_blue <= END_BLUE; current_blue +=
                                            STEP_BLUE) {
                                duty_red = (100 / DIVISOR_RED) * current_red;
                                duty_green = (100 / DIVISOR_GREEN)
                                        * Current_green;
                                duty_blue = (100 / DIVISOR_BLUE) * current_blue;
                                //PRINTF("\r\n%d, %d, %d\r\n", duty_red , duty_green, duty_blue);
                                configure_ftm(duty_red, duty_green, duty_blue);
                                update_queue(current_red, current_green,
                                        current_blue);
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
                    for (int current_red = END_RED; current_red >= START_RED;
                            current_red -= STEP_RED) {
                        if (STEP_GREEN == 0)
                            STEP_GREEN--;
                        for (int current_green = END_GREEN;
                                current_green >= START_GREEN; current_green -=
                                        STEP_GREEN) {
                            if (STEP_BLUE == 0)
                                STEP_BLUE--;
                            for (int current_blue = END_BLUE;
                                    current_blue >= START_BLUE; current_blue -=
                                            STEP_BLUE) {
                                duty_red = (100 / DIVISOR_RED) * current_red;
                                duty_green = (100 / DIVISOR_GREEN)
                                        * current_green;
                                duty_blue = (100 / DIVISOR_BLUE) * current_blue;
                                //PRINTF("\r\n%d, %d, %d\r\n", duty_red , duty_green, duty_blue);
                                configure_ftm(duty_red, duty_green, duty_blue);
                                update_queue(current_red, current_green,
                                        current_blue);
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



