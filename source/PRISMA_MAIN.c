/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    PRISMA_SCREEN_VALIDATION.c
 * @brief   Application entry point.
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/*Free_scale includes*/
#include <stdio.h>
#include <math.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_uart.h"

#include "../PRISMA_VALIDATION/PRISMA_VALIDATION.c"

/* Task priorities. */
#define get_configuration_PRIORITY (configMAX_PRIORITIES - 1)
#define pattern_generator_PRIORITY (configMAX_PRIORITIES - 1)

#define QUEUE_SEND_BUFFER_SIZE 14
#define QUEUE_RECEIVE_BUFFER_SIZE 4
#define REFRESH_RATE 0
#define COLOUR_CODING_SCHEME 1
#define START_RED 2
#define START_GREEN 3
#define START_BLUE 4
#define END_RED 5
#define END_GREEN 6
#define END_BLUE 7
#define STEP_RED 8
#define STEP_GREEN 9
#define STEP_BLUE 10
#define CHANGE_RATE 11
#define MODE 12
#define CYCLE 13


//QueueHandle_t config_queue = NULL;
//QueueHandle_t response_queue = NULL;

/*******************************************************************************
 * Prototypes
 ************************************* *****************************************/
static void get_configuration(void *pvParameters);
static void pattern_generator(void *pvParameters);

/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */


int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();


#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif

	uart_config_t config;

	BOARD_InitBootPins();
	BOARD_InitBootClocks();

	/*
	 * config.baudRate_Bps = 115200U;
	 * config.parityMode = kUART_ParityDisabled;
	 * config.stopBitCount = kUART_OneStopBit;
	 * config.txFifoWatermark = 0;
	 * config.rxFifoWatermark = 1;
	 * config.enableTx = false;
	 * config.enableRx = false;
	 */
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
	config.enableTx     = true;
	config.enableRx     = true;

	UART_Init(PRISMA_UART, &config, PRISMA_UART_CLK_FREQ);
	UART_EnableInterrupts(PRISMA_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
	EnableIRQ(PRISMA_UART_IRQn);

	PIT_GetDefaultConfig(&pitConfig);
	/* Init pit module */
	PIT_Init(DEMO_PIT_BASEADDR, &pitConfig);
	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL,
			kPIT_TimerInterruptEnable);
	/* Enable at the NVIC */
	EnableIRQ(PIT_IRQ_ID);

	config_queue = xQueueCreate(10, sizeof(int)*14);
	if (config_queue == NULL) {
		PRINTF("\r\nconfig_queue creation failure!\r\n");
		vQueueAddToRegistry(config_queue, "int *config_bufferLogQ_1");
	}
	response_queue = xQueueCreate(10, sizeof(int)*14);
	if (response_queue == NULL) {
		PRINTF("\r\nconfig_queue creation failure!\r\n");
		vQueueAddToRegistry(response_queue, "int *config_bufferLogQ_2");
	}
	if (xTaskCreate(get_configuration,
			"Task 1_Master_Get_configuration",
			configMINIMAL_STACK_SIZE + 100, NULL,
			get_configuration_PRIORITY, NULL) != pdPASS) {
		PRINTF("Task1 creation failed!.\r\n");
		update_error(task_fail);
		while (1)
			;
	}
	if (xTaskCreate(pattern_generator,
			"Task 1_Master_Get_configuration",
			configMINIMAL_STACK_SIZE + 100, NULL,
			get_configuration_PRIORITY, NULL) != pdPASS) {
		PRINTF("Task1 creation failed!.\r\n");
		update_error(task_fail);
		while (1)
			;
	}
	vTaskStartScheduler();
	for (;;)
		;
	return 0;
}

static void get_configuration(void *pvParameters) {
	int configuration_flag;
	if (GPIO_PinRead(GPIOD, 0U) == 1U)
		configuration_flag = 1;//master
	else
		configuration_flag = 0;//slave
	while(1) {
		char colour_coding_scheme_value[3] = {7,7,3};
		char pattern_config_usroption;
		char mode_option_string[10] = "AUTO UP";
		int colour_coding_scheme_queue_value = 1;
		int mode_option_string_value = 1;
		int start_colour[3] = {0,0,0};
		int end_colour[3] = {7,7,3};
		int resolution[3] = {1,1,1};
		int find_colour[3] = {0,0,0};
		int refresh_rate = 5;
		int colour_change_rate = 40;
		int repeat_cycle = 2;
		int mode_usroption = 1;
		int start_status_flag = 0;
		char Hand_Shake[20];
		int send_status_flag = 0;

		if (configuration_flag == 1) {
			strcpy(Hand_Shake,"STAND ALONE");
			homescreen();
			while(1) {
				int QUEUE_SEND_BUFFER[QUEUE_SEND_BUFFER_SIZE];
				int QUEUE_RECEIVE_BUFFER[QUEUE_RECEIVE_BUFFER_SIZE];
				int QUEUE_SEARCH_BUFFER[QUEUE_SEND_BUFFER_SIZE];
				char master_home_usroption = 0;
				char system_config_user_option;
				char edit_scrn_usroptn;
				char colour_coding_scheme_useroptn;
				char rate_config_useroptn;
				char find_usroption;
				char help_scrn_usroption;
				Master_homescreen(&master_home_usroption,colour_coding_scheme_value,&refresh_rate,&colour_change_rate,start_colour,end_colour,resolution,mode_option_string,&repeat_cycle,Hand_Shake);
				if(master_home_usroption == 'S') {
					start_status_flag = !start_status_flag;
					if(start_status_flag == 1) {
						while(1) {
							if (mode_option_string_value == 1) {    //validation for AUTO UP
								Auto_up_validation(start_colour, end_colour, resolution,&send_status_flag);
							} else if(mode_option_string_value == 2) {              //validation for AUTO DOWN
								Auto_down_validation(start_colour, end_colour, resolution,&send_status_flag);
							} else if(mode_option_string_value == 3) {                     //validation for AUTO UP/DOWN
								Auto_up_down_validation(start_colour, end_colour, resolution,&send_status_flag);
							} else if(mode_option_string_value == 4) {                      // MANUAL validation
								manual_validation(start_colour, end_colour, resolution,&send_status_flag);
							} else;
							if(send_status_flag == 3) {
								QUEUE_SEND_BUFFER[REFRESH_RATE] = refresh_rate;
								QUEUE_SEND_BUFFER[COLOUR_CODING_SCHEME] = colour_coding_scheme_queue_value;
								QUEUE_SEND_BUFFER[START_RED] = start_colour[0];
								QUEUE_SEND_BUFFER[START_GREEN] = start_colour[1];
								QUEUE_SEND_BUFFER[START_BLUE] = start_colour[2];
								QUEUE_SEND_BUFFER[END_RED] = end_colour[0];
								QUEUE_SEND_BUFFER[END_GREEN] = end_colour[1];
								QUEUE_SEND_BUFFER[END_BLUE] = end_colour[2];
								QUEUE_SEND_BUFFER[STEP_RED] = resolution[0];
								QUEUE_SEND_BUFFER[STEP_GREEN] = resolution[1];
								QUEUE_SEND_BUFFER[STEP_BLUE] = resolution[2];
								QUEUE_SEND_BUFFER[CHANGE_RATE] = colour_change_rate;
								QUEUE_SEND_BUFFER[MODE] = mode_option_string_value;
								QUEUE_SEND_BUFFER[CYCLE] = repeat_cycle;
								/*************************** QUEUE SEND	*************************************/
								if(xQueueSend(config_queue, &QUEUE_SEND_BUFFER,0) != pdTRUE) {
									PRINTF("QUEUE SEND FAILED1\r\n");
								}
								QUEUE_SEND_BUFFER[0] = 0;
								QUEUE_SEND_BUFFER[1] = 's';
								if(xQueueSend(config_queue, &QUEUE_SEND_BUFFER,0) != pdTRUE){
									PRINTF("QUEUE SEND FAILED2\r\n");
								}
								/*************************** QUEUE RECEIVE	*************************************/
								if(start_status_flag == 1) {
									interrupt_flag = 0;
									while(!interrupt_flag) {
										if(xQueueReceive(response_queue, QUEUE_RECEIVE_BUFFER, 0) == pdTRUE) {
											Master_homescreen_update(QUEUE_RECEIVE_BUFFER);
										}
									}
									break;
								}
							}
							break;
						}
					} else if (start_status_flag == 0){
						/*************************** QUEUE SEND	*************************************/
						QUEUE_SEND_BUFFER[0] = 0;
						QUEUE_SEND_BUFFER[1] = 's';
						if(xQueueSend(config_queue, &QUEUE_SEND_BUFFER,0) != pdTRUE){
							PRINTF("QUEUE SEND FAILED2\r\n");
						}

					} else;
				} else if(master_home_usroption == 'D'){
					if(start_status_flag == 1) {
						/*************************** QUEUE SEND	*************************************/
						QUEUE_SEND_BUFFER[0] = 0;
						QUEUE_SEND_BUFFER[1] = 'd';
						if(xQueueSend(config_queue, &QUEUE_SEND_BUFFER,0) != pdTRUE){
							PRINTF("QUEUE SEND FAILED2\r\n");
						}
					} else {
						PRINTF("YOU SHOULD START PATTERN GENERATION FIRST AND TRY <<< PAUSING >>>\r\n");
						delay(10);
					}
				} else if(master_home_usroption == 'F'){
					while (Edit_config_screen(&edit_scrn_usroptn)) {
						if (edit_scrn_usroptn == '1') {
							while (System_config_screen(&system_config_user_option)) {
								if(system_config_user_option == '1') {
									while (Colour_coding_scheme_screen(&colour_coding_scheme_useroptn,&colour_coding_scheme_queue_value,colour_coding_scheme_value)) {
										if(colour_coding_scheme_useroptn == 'B') {
											break;
										} else if(colour_coding_scheme_useroptn == 'N') {
											break;
										}
									}
									if(colour_coding_scheme_useroptn == 'N') {
										break;
									}
								} else if (system_config_user_option == '2') {
									while (Rate_configuration_screen(&rate_config_useroptn,&refresh_rate,&colour_change_rate)) {
										if(rate_config_useroptn == 'B') {
											break;
										} else if(rate_config_useroptn == 'N') {
											break;
											//return home operation
										}
									}
									if(rate_config_useroptn == 'N') {
										break;
									}
								} else if (system_config_user_option == 'B') {
									break;
								} else {
								}
								if(colour_coding_scheme_useroptn == 'N') {
									break;
									//return home operation
								}
								if(rate_config_useroptn == 'N') {
									break;
									//return home operation
								}
							}
						} else if (edit_scrn_usroptn == '2') {
							while(Pattern_configuration_screen(start_colour, end_colour, resolution, &pattern_config_usroption,&repeat_cycle)) {
								if(pattern_config_usroption == 'B') {
									break;
								} else if(pattern_config_usroption == 'N') {
									// return home operation
									break;
								}
							}
							if(pattern_config_usroption == 'N') {
								//return home operation
								break;
							}
						} else if (edit_scrn_usroptn == 'B') {
							break;
						} else ;
						if(rate_config_useroptn == 'N') {
							break;
							//return home operation
						}
						if(colour_coding_scheme_useroptn == 'N') {
							break;
							//return home operation
						}
					}
				} else if (master_home_usroption == 'X'){
					while(ModeSelect_screen(&mode_usroption,&mode_option_string_value,mode_option_string)){
						if(mode_usroption == 11) {
							break;
						}
					}
				} else if (master_home_usroption == 'V'){
					while(Find_colour_screen(find_colour,QUEUE_SEARCH_BUFFER,&find_usroption)) {
						if(find_usroption == 'B') {
							break;
						} else if (find_usroption == 'S') {
							//PRINTF("\033[13;9Hqueue send is done here");
							/*************************** QUEUE SEND	*************************************/
							if(xQueueSend(config_queue, QUEUE_SEARCH_BUFFER, 0) != pdTRUE) {
								PRINTF("QUEUE SEND FAILED1\r\n");
							}
							QUEUE_SEARCH_BUFFER[0] = 0;
							QUEUE_SEARCH_BUFFER[1] = 's';
							if(xQueueSend(config_queue, QUEUE_SEARCH_BUFFER, 0) != pdTRUE){
								PRINTF("QUEUE SEND FAILED2\r\n");
							}
							//implement send function for search
						} else ;
					}
				} else if (master_home_usroption == 'H'){
					while(Help_screen(&help_scrn_usroption)) {
						if(help_scrn_usroption == 'Q') {
							break;
						}
					}
				} else {
				}
			}
		} else {
			Slave_homescreen();
			delay(2);
		}
	}
}
static void pattern_generator(void *pvParameters) {
	int control_buffer[14];
	int config_buffer[14];
	while (1) {
		while(xQueuePeek(config_queue, control_buffer, 0) != pdTRUE){
		}
		if (xQueuePeek(config_queue, control_buffer, 0) == pdTRUE) {
			if (control_buffer[0] != 0) {
				if (xQueueReceive(config_queue, control_buffer, 0) == pdTRUE) {
					//PRINTF("Received from task 1");
					for (int i = 0; i <= 13; i++) {
						config_buffer[i] = control_buffer[i];
					}
				}
			}
		}
		if (config_buffer[_MODE_] == 1) {
			Auto_up(config_buffer, control_buffer);
		} else if (config_buffer[_MODE_] == 2) {
			Auto_down(config_buffer, control_buffer);
		} else if (config_buffer[_MODE_] == 3) {
			Auto_up_down(config_buffer, control_buffer);
		} else if (config_buffer[ _MODE_] == 4) {
			Manual_mode(config_buffer, control_buffer);
		}
	}
}



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
void update_queue(int current_red, int current_green, int current_blue, int pattern_count) {
	int response[4];
	response[0] = current_red;
	response[1] = current_green;
	response[2] = current_blue;
	response[3] =  pattern_count;
	xQueueSend(response_queue, response,0);

}
//Send error code to Task_1
void update_error(int error_code) {
	//PRINTF("\r\n%d ERROR\r\n", error_code);
	xQueueSend(response_queue, &error_code, 0);
}

