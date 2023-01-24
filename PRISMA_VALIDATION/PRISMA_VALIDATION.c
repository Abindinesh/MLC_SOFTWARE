#include <stdio.h>
#include "fsl_debug_console.h"
#include "queue.h"
#include <string.h>
/*  SDK Included Files */
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_i2c_edma.h"
#include "fsl_dmamux.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#include "../PRISMA_SCREEN_INCLUDES/PRISMA_SCREEN.c"




/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* I2C source clock */
#define I2C_MASTER_CLK_SRC          I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ         CLOCK_GetFreq(I2C0_CLK_SRC)
#define EXAMPLE_I2C_MASTER_BASEADDR I2C0
#define DMA_REQUEST_SRC             kDmaRequestMux0I2C0
#define EXAMPLE_I2C_DMAMUX_BASEADDR DMAMUX0
#define EXAMPLE_I2C_DMA_BASEADDR    DMA0
#define I2C_DMA_CHANNEL             0U
#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define EXAMPLE_I2C_SLAVE_BASEADDR I2C0
#define I2C_BAUDRATE               100000U
#define I2C_DATA_LENGTH            8U
#define I2C_SLAVE_HOLD_TIME_NS 4000U
#define I2C_SLAVE_CLK_SRC          I2C0_CLK_SRC
#define I2C_SLAVE_CLK_FREQ         CLOCK_GetFreq(I2C0_CLK_SRC)



 int Auto_up_validation(int *start_colour, int *end_colour,int *resolution,int *send_status_flag) {
	 if (start_colour[0] < end_colour[0]) {
		 if(resolution[0] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else if (start_colour[0] == end_colour[0]) {
		 if(resolution[0] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)\r\n",start_colour[0],end_colour[0]);
		 delay(20);
	 }
	 if (start_colour[1] < end_colour[1]) {
		 if(resolution[1] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else if (start_colour[1] == end_colour[1]) {
		 if(resolution[1] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
		 delay(20);
	 }

	 if (start_colour[2] < end_colour[2]) {
		 if(resolution[2] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }

	 } else if (start_colour[2] == end_colour[2]) {
		 if(resolution[2] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
		 delay(20);
	 }
	 return 1;
 }
 int Auto_down_validation(int *start_colour, int *end_colour,int *resolution,int *send_status_flag) {
	 if (start_colour[0] > end_colour[0]) {
		 if(resolution[0] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }

	 } else if (start_colour[0] == end_colour[0]) {
		 if(resolution[0] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-DOWN, TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)",start_colour[0],end_colour[0]);
		 delay(20);
	 }
	 if (start_colour[1] > end_colour[1]) {
		 if(resolution[1] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }

	 } else if (start_colour[1] == end_colour[1]) {
		 if(resolution[1] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-DOWN, TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
		 delay(20);
	 }

	 if (start_colour[2] > end_colour[2]) {
		 if(resolution[2] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }

	 } else if (start_colour[2] == end_colour[2]) {
		 if(resolution[2] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-DOWN, TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
		 delay(20);
	 }
	 return 1;
 }
 int Auto_up_down_validation(int *start_colour, int *end_colour,int *resolution,int *send_status_flag) {
	 if (start_colour[0] < end_colour[0]) {
		 if(resolution[0] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else if (start_colour[0] == end_colour[0]) {
		 if(resolution[0] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP/DOWN, TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)",start_colour[0],end_colour[0]);
		 delay(20);
	 }
	 if (start_colour[1] < end_colour[1]) {
		 if(resolution[1] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }

	 } else if (start_colour[1] == end_colour[1]) {
		 if(resolution[1] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP/DOWN, TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
		 delay(20);
	 }

	 if (start_colour[2] < end_colour[2]) {
		 if(resolution[2] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }

	 } else if (start_colour[2] == end_colour[2]) {
		 if(resolution[2] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP/DOWN,TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
		 delay(20);
	 }

	 return 1;
 }
 int manual_validation(int *start_colour, int *end_colour,int *resolution,int *send_status_flag) {
	 if (start_colour[0] < end_colour[0]) {
		 if(resolution[0] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else if (start_colour[0] == end_colour[0]) {
		 if(resolution[0] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)\r\n",start_colour[0],end_colour[0]);
		 delay(20);
	 }
	 if (start_colour[1] < end_colour[1]) {
		 if(resolution[1] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else if (start_colour[1] == end_colour[1]) {
		 if(resolution[1] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
		 delay(20);
	 }

	 if (start_colour[2] < end_colour[2]) {
		 if(resolution[2] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }

	 } else if (start_colour[2] == end_colour[2]) {
		 if(resolution[2] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
		 delay(20);
	 }
	 return 1;
 }


