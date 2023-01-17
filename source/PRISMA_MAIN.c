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

//QueueHandle_t config_queue = NULL;
//QueueHandle_t response_queue = NULL;

/* Task priorities. */
#define get_configuration_PRIORITY (configMAX_PRIORITIES - 1)
#define pattern_generator_PRIORITY (configMAX_PRIORITIES - 1)


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

	config_queue = xQueueCreate(2, sizeof(int) * 14);
	if (config_queue != NULL) {
		vQueueAddToRegistry(config_queue, "int *config_bufferLogQ_1");
	}
	response_queue = xQueueCreate(1, sizeof(int) * 4);
	if (response_queue != NULL) {
		vQueueAddToRegistry(response_queue, "int *config_bufferLogQ_2");
	}
	if (xTaskCreate(get_configuration,
			"Task 1_Master_Get_configuration",
			configMINIMAL_STACK_SIZE + 100, NULL,
			get_configuration_PRIORITY, NULL) != pdPASS) {
		//PRINTF("Task1 creation failed!.\r\n");
		update_error(task_fail);
		while (1)
			;
	}
	if (xTaskCreate(pattern_generator,
			"Task 1_Master_Get_configuration",
			configMINIMAL_STACK_SIZE + 100, NULL,
			get_configuration_PRIORITY, NULL) != pdPASS) {
		//PRINTF("Task1 creation failed!.\r\n");
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
		configuration_flag = 1;
	else
		configuration_flag = 0;
	while(1) {
		if (configuration_flag == 1) {
			Master_UI();
		} else
			Slave_UI() ;
	}
}

static void pattern_generator(void *pvParameters) {
	//Get pit clock

	int control_buffer[14];
	int config_buffer[14];//={0, 's', 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	while (xQueuePeek(config_queue, control_buffer, 0) != pdTRUE) {
		//PRINTF(" \r\nREADY\r\n");
		__asm("NOP");
	}
	while (1) {
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





