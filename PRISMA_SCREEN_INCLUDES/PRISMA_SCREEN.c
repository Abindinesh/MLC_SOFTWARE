#include <stdio.h>
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_uart.h"
#include "../PATTERN/PRISMA_PATTERN.c"
#include "queue.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* UART instance and clock */
#define PRISMA_UART            UART0
#define PRISMA_UART_CLKSRC     UART0_CLK_SRC
#define PRISMA_UART_CLK_FREQ   CLOCK_GetFreq(UART0_CLK_SRC)
#define PRISMA_UART_IRQn       UART0_RX_TX_IRQn
#define PRISMA_UART_IRQHandler UART0_RX_TX_IRQHandler
#define PRISMA_RING_BUFFER_SIZE 16
#define gotoxy(x,y) PRINTF("\033[%d;%dH", (y), (x))

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

int interrupt_flag = 0;
char uart_read;
void PRISMA_UART_IRQHandler(void) {
	if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(PRISMA_UART)) {
		interrupt_flag = 1;
		uart_read = UART_ReadByte(PRISMA_UART);
	}
}

void delay(int count) {
	while (count != 0) {
		int count1 = 900000;
		while (count1 != 0) {
			count1--;
		}
		count--;
	}
}

void homescreen(void) {
	PRINTF("\e[1;1H\e[2J");
	PRINTF(
			"                                                                 00                                             \n\r");
	delay(1);
	PRINTF(
			"                                                                 " "                                             \n\r");
	delay(1);
	PRINTF(
			"                                        8b,dPPYba,   8b,dPPYba,  88  ,adPPYba,  88,dPYba,,adPYba,   ,adPPYYba,  \n\r");
	delay(1);
	PRINTF(
			"                                        88P'    \"8a  88P'   \"Y8  88  I8[    \"\"  88P'   \"88\"    \"8a  \"\"     `Y8  \n\r");
	delay(1);
	PRINTF(
			"                                        88       d8  88          88   `\"Y8ba,   88      88      88  ,adPPPPP88  \n\r");
	delay(1);
	PRINTF(
			"                                        88b,   ,a8\"  88          88  aa    ]8I  88      88      88  88,    ,88  \n\r");
	delay(1);
	PRINTF(
			"                                        88`YbbdP\"'   88          88  `\"YbbdP\"'  88      88      88  `\"8bbdP\"Y8  \n\r");
	delay(1);
	PRINTF(
			"                                        88                                                                      \n\r");
	delay(1);
	PRINTF(
			"                                        88                                                                      \n\r");
	delay(9);
	PRINTF(
			"                                                                      _    _     _____    _____ \n\r");
	PRINTF(
			"                                                                       \\  /        |     |     | \n\r");
	PRINTF(
			"                                                                        \\/       __|__ . |_____| \n\r");
	PRINTF(
			"                                                                                                           ");
	delay(20);
}

int Master_homescreen(char *Master_Home_Usroption,char *colour_coding_scheme_value,int *refresh_rate,int *colour_change_rate,int *start_colour, int *end_colour,int *resolution,char *mode_option_string,int *repeat_cycle, char *Hand_Shake) {
	int status = -1;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF(
				"************************************************************** WELCOME TO PRISMA **************************************************************\n\r");
		PRINTF(
				"                                                     GENERATE YOUR OWN LED COLOUR PATTERN\n\n\r");
		PRINTF("         **CURRENT CONFIGURATION**\n\n\r");
		PRINTF("...SYSTEM CONFIGURATION...\n\n\r");
		PRINTF(" CONFIGURED AS                      : MASTER\n\r");
		PRINTF(" RGB CODING SCHEME                  : ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("\r\n");
			else {
				PRINTF("%d", colour_coding_scheme_value[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF(" REFRESH RATE                       : %d\n\r",*refresh_rate);
		PRINTF(" COLOUR CHANGE RATE                 : %d\n\n\r",*colour_change_rate);
		PRINTF("...PATTERN CONFIGURATION...\n\n\r");
		PRINTF(" START COLOUR                       : ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("\r\n");
			else {
				PRINTF("%d", start_colour[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF(" END COLOUR                         : ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("\r\n");
			else {
				PRINTF("%d", end_colour[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF(" RESOLUTION                         : ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("\r\n");
			else {
				PRINTF("%d", resolution[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF(" MODE, CYCLE                        : %s, %d\n\n\r",mode_option_string,*repeat_cycle);
		PRINTF("                **STATUS**\n\n\r");
		PRINTF(
				" SLAVE STATUS                       : < CONNECTING/CONNECTED/DISCONNECTED >\n\r");
		PRINTF(
				" MODE OF WORKING                    : < %s >\n\r",Hand_Shake);

		PRINTF(" CURRENT LED COLOUR                 : \n\r");
		PRINTF(" NO OF CYCLES COMPLETED             : 0\n\n\r");
		PRINTF("      S     : START/STOP\n\r");
		PRINTF("      D     : PAUSE/RESUME\n\r");
		PRINTF("      F     : EDIT CONFIGURATION\n\r");
		PRINTF("      X     : MODE\n\r");
		PRINTF("      V     : FIND COLOUR\n\r");
		PRINTF("      H     : HELP?\n\n\n\r");
		PRINTF("ENTER YOUR OPTION :");
		while(!interrupt_flag) {
		}
		*Master_Home_Usroption = uart_read;
		interrupt_flag = 0;
		PRINTF("%c\r\n", *Master_Home_Usroption);
		delay(2);
		if ((*Master_Home_Usroption == 's')
				|| (*Master_Home_Usroption == 'S')) {
			*Master_Home_Usroption = 'S';
			status = 0;
		} else if ((*Master_Home_Usroption == 'd')
				|| (*Master_Home_Usroption == 'D')) {
			*Master_Home_Usroption = 'D';
			status = 0;

		} else if ((*Master_Home_Usroption == 'f')
				|| (*Master_Home_Usroption == 'F')) {
			*Master_Home_Usroption = 'F';
			status = 0;

		} else if ((*Master_Home_Usroption == 'x')
				|| (*Master_Home_Usroption == 'X')) {
			*Master_Home_Usroption = 'X';
			status = 0;

		} else if ((*Master_Home_Usroption == 'v')
				|| (*Master_Home_Usroption == 'V')) {
			*Master_Home_Usroption = 'V';
			status = 0;

		} else if ((*Master_Home_Usroption == 'h')
				|| (*Master_Home_Usroption == 'H')) {
			*Master_Home_Usroption = 'H';
			status = 0;

		} else {
			PRINTF("INVALID OPTION TRIED!!!\n\n\rTRY AGAIN...\r\n");
			delay(9);
			status = 1;
		}
	} while (status);
	return 1;
}

int Master_homescreen_update(int *QUEUE_RECEIVE_BUFFER) {
		PRINTF("\033[24;36H"            );
		PRINTF("\033[24;39H");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("\r\n");
			else {
				PRINTF("%d", QUEUE_RECEIVE_BUFFER[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF("\033[35;21H");
	return 1;
}

int Edit_config_screen(char *edit_scrn_usroptn) {
	int status;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF(
				"************************ EDIT CONFIGURATION ************************\n\n\r");
		PRINTF("       1    : SYSTEM CONFIGURATION\n\r");
		PRINTF("       2    : PATTERN CONFIGURATION\n\n\n\r");
		PRINTF("       B : BACK TO HOME\n\r");
		PRINTF("ENTER YOUR OPTION :");
		while(!interrupt_flag) {
		}
		*edit_scrn_usroptn = uart_read;
		interrupt_flag = 0;
		PRINTF("%c\r\n", *edit_scrn_usroptn);
		delay(2);
		if (*edit_scrn_usroptn == '1') {
			status = 0;
		} else if (*edit_scrn_usroptn == '2') {
			status = 0;
		} else if ((*edit_scrn_usroptn == 'B') || (*edit_scrn_usroptn == 'b')) {
			*edit_scrn_usroptn = 'B';
			status = 0;
		} else {
			PRINTF("INVALID OPTION TRIED!!!\n\n\rTRY AGAIN...\r\n");
			delay(9);
			status = 1;
		}
	} while (status);
	return 1;
}

int System_config_screen(char *system_config_user_option) {
	int status;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF(
				"************************ SYSTEM CONFIGURATION ************************\n\n\r");
		PRINTF("       1    : RGB COLOUR CODING SCHEME\n\r");
		PRINTF("       2    : RATES\n\n\n\r");
		PRINTF("       B : BACK\n\r");
		PRINTF("ENTER YOUR OPTION :");
		while(!interrupt_flag) {
		}
		*system_config_user_option = uart_read;
		interrupt_flag = 0;
		PRINTF("%c\r\n", *system_config_user_option);
		delay(2);
		if (*system_config_user_option == '1') {
			status = 0;
		} else if (*system_config_user_option == '2') {
			status = 0;
		} else if ((*system_config_user_option == 'B')
				|| (*system_config_user_option == 'b')) {
			*system_config_user_option = 'B';
			status = 0;
		} else {
			PRINTF("INVALID OPTION TRIED!!!\n\n\rTRY AGAIN...\r\n");
			delay(9);
			status = 1;
		}
	} while (status);
	return 1;
}

int Colour_coding_scheme_screen(char *colour_coding_scheme_useroptn,int *colour_coding_scheme_queue_value,char *colour_coding_scheme_value) {
	int status;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF(
				"********************* RGB COLOUR CODING SCHEME *********************\n\n\r");
		PRINTF("-----  CURRENT COLOUR CODING SCHEME   : ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("  -----\r\n");
			else {
				PRINTF("%d", colour_coding_scheme_value[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF("CHOOSE COLOUR CODING SCHEME\n\n\r");
		PRINTF("    1.     : 7,7,3\n\r");
		PRINTF("    2.     : 4,4,4\n\r");
		PRINTF("    3.     : 8,8,8\n\n\r");
		PRINTF("       B : BACK\n\r");
		PRINTF("       N : HOME\n\n\r");
		PRINTF("ENTER YOUR OPTION :");
		while(!interrupt_flag) {
		}
		*colour_coding_scheme_useroptn = uart_read;
		interrupt_flag = 0;
		PRINTF("%c\r\n", *colour_coding_scheme_useroptn);
		if (*colour_coding_scheme_useroptn == '1') {
			*colour_coding_scheme_queue_value = 1;
			colour_coding_scheme_value[0] = 7;
			colour_coding_scheme_value[1] = 7;
			colour_coding_scheme_value[2] = 3;
			PRINTF("SUCCESSFULLY UPDATED");
			delay(9);
			status = 1;
		} else if (*colour_coding_scheme_useroptn == '2') {
			//			*colour_coding_scheme_queue_value = 2;
			//			colour_coding_scheme_value[0] = 4;
			//			colour_coding_scheme_value[1] = 4;
			//			colour_coding_scheme_value[2] = 4;
			//			PRINTF("SUCCESSFULLY UPDATED");
			PRINTF("THIS SCHEME IS NOT AVAILABLE NOW");
			delay(9);
			status = 1;
		} else if (*colour_coding_scheme_useroptn == '3') {
			//			*colour_coding_scheme_queue_value = 3;
			//			colour_coding_scheme_value[0] = 8;
			//			colour_coding_scheme_value[1] = 8;
			//			colour_coding_scheme_value[2] = 8;
			//			PRINTF("SUCCESSFULLY UPDATED");
			PRINTF("THIS SCHEME IS NOT AVAILABLE NOW");
			delay(9);
			status = 1;
		} else if ((*colour_coding_scheme_useroptn == 'B')
				|| (*colour_coding_scheme_useroptn == 'b')) {
			*colour_coding_scheme_useroptn = 'B';
			status = 0;
		} else if ((*colour_coding_scheme_useroptn == 'N')
				|| (*colour_coding_scheme_useroptn == 'n')) {
			*colour_coding_scheme_useroptn = 'N';
			status = 0;
			return 0;
		} else {
			PRINTF("INVALID OPTION TRIED!!!\n\n\rTRY AGAIN...\r\n");
			delay(9);
			status = 1;
		}
	} while (status);

	return 1;
}

int Rate_configuration_screen(char *rate_config_useroptn, int *refresh_rate,
		int *colour_change_rate) {
	int status, RF_status, CLR_status;
	int readed_rate;
	char option;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF(
				"********************* FILL THE DETAILS *********************\n\n\r");
		PRINTF("----- 1. REFRESH RATE             : %d  -----\n\r",
				*refresh_rate);
		PRINTF("----- 2. COLOUR CHANGE RATE       : %d -----\n\n\n\r",
				*colour_change_rate);
		PRINTF("         B : BACK\n\r");
		PRINTF("         N : HOME\n\r");
		PRINTF("SELECT OPTION  :\r\n");
		PRINTF("\033[9;17H");
		while(!interrupt_flag) {
		}
		option = uart_read;
		interrupt_flag = 0;
		if (option == '1') {
			do {
				int refresh_rate1 = 0;
				PRINTF("\033[9;0HENTER NEW REFRESH RATE (1 TO 9999)     : ****");
				for(int i = 1;i <= 4;i++) {
					PRINTF("\033[9;%dH",41+i);
					while(!interrupt_flag) {
					}
					readed_rate = uart_read;
					interrupt_flag = 0;
					if((readed_rate >= 48) && (readed_rate <= 57)) {
						readed_rate = readed_rate - 48;
						if(i == 1) {
							PRINTF("%d",readed_rate);
							PRINTF("\033[9;43H");
							refresh_rate1 = refresh_rate1 + readed_rate * 1000;
						} else if (i == 2) {
							PRINTF("%d",readed_rate);
							PRINTF("\033[9;44H");
							refresh_rate1 = refresh_rate1 + readed_rate * 100;
						} else if (i == 3) {
							PRINTF("%d",readed_rate);
							PRINTF("\033[9;45H");
							refresh_rate1 = refresh_rate1 + readed_rate * 10;
						} else if (i == 4) {
							PRINTF("%d\r\n",readed_rate);
							refresh_rate1 = refresh_rate1 + readed_rate * 1;
						} else ;
					} else {
						PRINTF("\033[10;0HNOT VALID NUMBER");
						delay(30);
						PRINTF("\033[10;0H                          ");
						i--;
					}
				}
				*refresh_rate = refresh_rate1;
				if ((*refresh_rate >= 1) && (*refresh_rate <= 9999)) {
					PRINTF("\033[11;0 SUCCESSFULLY UPDATED......\r\n");
					delay(10);
					RF_status = 0;
				} else {
					PRINTF("\033[10;0H INVALID!!!\r\nTRY AGAIN...");
					delay(10);
					PRINTF("\033[11;0H                              ");
					PRINTF(
							"\033[9;0H                                                                                  ");
					PRINTF(
							"\033[11;0H                                                                                  ");
					RF_status = 1;
				}
			} while (RF_status);
			status = 1;
		} else if (option == '2') {
			do {
				int colour_change_rate1 = 0;
				PRINTF("\033[9;0H ENTER NEW COLOUR CHANGE RATE(1 TO 500) : ***");
				for(int i=1;i <= 3;i++) {
					PRINTF("\033[9;%dH",42+i);
					while(!interrupt_flag) {
					}
					readed_rate = uart_read;
					interrupt_flag = 0;
					if(i == 1) {
						if((readed_rate >= 48) && (readed_rate <= 53)) {
							readed_rate = readed_rate - 48;
							PRINTF("%d",readed_rate);
							colour_change_rate1 = colour_change_rate1 + readed_rate * 100;
						} else {
							PRINTF("\033[10;0HNOT VALID NUMBER!!!\r\n");
							delay(10);
							PRINTF("\033[10;0H                                            ");
							i--;
						}
					} else if(i == 2) {
						if((readed_rate >= 48) && (readed_rate <= 57)) {
							readed_rate = readed_rate - 48;
							PRINTF("%d",readed_rate);
							colour_change_rate1 = colour_change_rate1 + readed_rate * 10;
						} else {
							PRINTF("\033[10;0HNOT VALID NUMBER!!!\r\n");
							delay(10);
							PRINTF("\033[10;0H                                            ");
							i--;
						}
					} else if(i == 3) {
						if((readed_rate >= 48) && (readed_rate <= 57)) {
							readed_rate = readed_rate - 48;
							PRINTF("%d",readed_rate);
							colour_change_rate1 = colour_change_rate1 + readed_rate * 1;
						} else {
							PRINTF("\033[10;0HNOT VALID NUMBER!!!\r\n");
							delay(10);
							PRINTF("\033[10;0H                                            ");
							i--;
						}
					} else ;
				}
				*colour_change_rate = colour_change_rate1;
				if ((*colour_change_rate >= 1)
						&& (*colour_change_rate <= 500)) {
					PRINTF("\033[10;0H SUCCESSFULLY UPDATED......\r\n");
					delay(10);
					CLR_status = 0;
				} else {
					PRINTF("\033[10;0H INVALID!!!\r\nTRY AGAIN...");
					delay(10);
					PRINTF("\033[10;0H                                ");
					PRINTF(
							"\033[9;0H                                                                                  ");
					PRINTF(
							"\033[11;0H                                                                                  ");
					CLR_status = 1;
				}
			} while (CLR_status);
			status = 1;
		} else if ((option == 'b') || (option == 'B')) {
			*rate_config_useroptn = 'B';
			status = 0;
		} else if ((option == 'n') || (option == 'N')) {
			*rate_config_useroptn = 'N';
			status = 0;
			return 0;
		} else {
			PRINTF("INVALID OPTION TRIED!!!\n\n\rTRY AGAIN...\r\n");
			delay(9);
			status = 1;
		}
	} while (status);
	return 1;
}

int Pattern_configuration_screen(int *start_colour, int *end_colour,
		int *resolution, char *pattern_config_usroption, int *repeat_cycle) {
	char option;
	int status,strt_success;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF(
				"************************ PATTERN CONFIGURATION ************************\n\n\r");
		PRINTF("----- 1. START COLOUR         :   ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("-----\r\n");
			else {
				PRINTF("%d", start_colour[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF("----- 2. END COLOUR           :   ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("-----\r\n");
			else {
				PRINTF("%d", end_colour[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF("----- 3. RESOLUTION           :   ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("-----\r\n");
			else {
				PRINTF("%d", resolution[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		PRINTF("----- 4. REPEAT CYCLE         :   %d    ", *repeat_cycle);
		PRINTF("\033[6;40H-----\r\n\n\n\n");
		PRINTF("        B : BACK\n\r");
		PRINTF("        N : HOME\n\n\n\r");
		PRINTF("SELECT AN OPTION     :");
		while(!interrupt_flag) {
		}
		option = uart_read;
		interrupt_flag = 0;
		PRINTF("%c\r\n",option);
		if (option == '1') {
			do {
				strt_success = 1;
				PRINTF("ENTER START COLOUR\r\n");
				PRINTF("\033[16;5HR(0 to 7) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					start_colour[0] = uart_read;
					interrupt_flag = 0;
					if((start_colour[0] >= 48) && (start_colour[0] <= 55)) {
						start_colour[0] = start_colour[0] - 48;
						PRINTF("\033[16;17H%d\r\n",start_colour[0]);
						break;
					} else {
						PRINTF("\033[17;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[17;9H                            ");
						PRINTF("\033[16;17H                                 ");
						PRINTF("\033[16;17H");
						continue;
					}
				}
				PRINTF("\033[17;5HG(0 to 7) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					start_colour[1] = uart_read;
					interrupt_flag = 0;
					if((start_colour[1] >= 48) && (start_colour[1] <= 55)) {
						start_colour[1] = start_colour[1] - 48;
						PRINTF("\033[17;17H%d\r\n",start_colour[1]);
						break;
					} else {
						PRINTF("\033[18;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[18;9H                            ");
						PRINTF("\033[17;17H                                 ");
						PRINTF("\033[17;17H");
						continue;
					}
				}
				PRINTF("\033[18;5HB(0 to 3) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					start_colour[2] = uart_read;
					interrupt_flag = 0;
					if((start_colour[2] >= 48) && (start_colour[2] <= 51)) {
						start_colour[2] = start_colour[2] - 48;
						PRINTF("\033[18;17H%d\r\n",start_colour[2]);
						delay(10);
						strt_success = 0;
						break;
					} else {
						PRINTF("\033[19;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[19;9H                            ");
						PRINTF("\033[18;17H                                 ");
						PRINTF("\033[18;17H");
						continue;
					}
				}
			} while (strt_success);
			status = 1;
		} else if (option == '2') {
			do {
				strt_success = 1;
				PRINTF("ENTER END COLOUR\r\n");
				PRINTF("\033[16;5HR(0 to 7) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					end_colour[0] = uart_read;
					interrupt_flag = 0;
					if((end_colour[0] >= 48) && (end_colour[0] <= 55)) {
						end_colour[0] = end_colour[0] - 48;
						PRINTF("\033[16;17H%d\r\n",end_colour[0]);
						break;
					} else {
						PRINTF("\033[17;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[17;9H                            ");
						PRINTF("\033[16;17H                                 ");
						PRINTF("\033[16;17H");
						continue;
					}
				}
				PRINTF("\033[17;5HG(0 to 7) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					end_colour[1] = uart_read;
					interrupt_flag = 0;
					if ((end_colour[1] >= 48) && (end_colour[1] <= 55)) {
						end_colour[1] = end_colour[1] - 48;
						PRINTF("\033[17;17H%d\r\n",end_colour[1]);
						break;
					} else {
						PRINTF("\033[18;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[18;9H                            ");
						PRINTF("\033[17;17H                               ");
						PRINTF("\033[17;17H");
						continue;
					}
				}
				PRINTF("\033[18;5HB(0 to 3) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					end_colour[2] = uart_read;
					interrupt_flag = 0;;
					if((end_colour[2] >= 48) && (end_colour[2] <= 51)) {
						end_colour[2] = end_colour[2] - 48;
						PRINTF("\033[18;17H%d\r\n",end_colour[2]);
						delay(10);
						strt_success = 0;
						break;
					} else {
						PRINTF("\033[19;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[19;9H                            ");
						PRINTF("\033[18;17H                                 ");
						PRINTF("\033[18;17H");
						continue;
					}
				}
			} while (strt_success);
			status = 1;
		} else if (option == '3') {
			do {
				strt_success = 1;
				PRINTF("ENTER RESOLUTION COLOUR\r\n");
				PRINTF("\033[16;5HR(0 to 7) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					resolution[0] = uart_read;
					interrupt_flag = 0;
					if ((resolution[0] >= 48) && (resolution[0] <= 55)) {
						resolution[0] = resolution[0] - 48;
						PRINTF("\033[16;17H%d\r\n",resolution[0]);
						break;
					} else {
						PRINTF("\033[17;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[17;9H                            ");
						PRINTF("\033[16;17H                                 ");
						PRINTF("\033[16;17H");
						continue;
					}
				}
				PRINTF("\033[17;5HG(0 to 7) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					resolution[1] = uart_read;
					interrupt_flag = 0;
					if ((resolution[1] >= 48) && (resolution[1] <= 55)) {
						resolution[1] = resolution[1] - 48;
						PRINTF("\033[17;17H%d\r\n",resolution[1]);
						break;
					} else {
						PRINTF("\033[18;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[18;9H                            ");
						PRINTF("\033[17;17H                               ");
						PRINTF("\033[17;17H");
						continue;
					}
				}
				PRINTF("\033[18;5HB(0 to 3) : ");
				while (1) {
					while(!interrupt_flag) {
					}
					resolution[2] = uart_read;
					interrupt_flag = 0;
					if((resolution[2] >= 48) && (resolution[2] <= 51)) {
						resolution[2] = resolution[2] - 48;
						PRINTF("\033[18;17H%d\r\n",resolution[2]);
						delay(10);
						strt_success = 0;
						break;
					} else {
						PRINTF("\033[19;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[19;9H                            ");
						PRINTF("\033[18;17H                                 ");
						PRINTF("\033[18;17H");
						continue;
					}
				}
			} while (strt_success);
			status = 1;
		} else if (option == '4') {
			int RF_status;
			int readed_cycle;
			do {
				int repat_cycle1 = 0;
				PRINTF("\033[15;0HENTER NEW REPEAT CYCLE(0 to 100)     : ***");
				for(int i=1;i <= 3;i++) {
					PRINTF("\033[15;%dH",39+i);
					while(!interrupt_flag) {
					}
					readed_cycle = uart_read;
					interrupt_flag = 0;
					if(i == 1) {
						if((readed_cycle >= 48) && (readed_cycle <= 49)) {
							readed_cycle = readed_cycle - 48;
							PRINTF("%d",readed_cycle);
							repat_cycle1 = repat_cycle1 + readed_cycle * 100;
						} else {
							PRINTF("\033[16;0HNOT VALID NUMBER!!!\r\n");
							delay(10);
							PRINTF("\033[16;0H                                            ");
							i--;
						}
					} else if(i == 2) {
						if((readed_cycle >= 48) && (readed_cycle <= 57)) {
							readed_cycle = readed_cycle - 48;
							PRINTF("%d",readed_cycle);
							repat_cycle1 = repat_cycle1 + readed_cycle * 10;
						} else {
							PRINTF("\033[16;0HNOT VALID NUMBER!!!\r\n");
							delay(10);
							PRINTF("\033[16;0H                                            ");
							i--;
						}
					} else if(i == 3) {
						if((readed_cycle >= 48) && (readed_cycle <= 57)) {
							readed_cycle = readed_cycle - 48;
							PRINTF("%d\r\n",readed_cycle);
							repat_cycle1 = repat_cycle1 + readed_cycle * 1;
						} else {
							PRINTF("\033[16;0HNOT VALID NUMBER!!!\r\n");
							delay(10);
							PRINTF("\033[16;0H                                            ");
							i--;
						}
					} else ;
					*repeat_cycle = repat_cycle1;
				}
				if ((*repeat_cycle >= 0) && (*repeat_cycle <= 100)) {
					PRINTF("\033[16;0 SUCCESSFULLY UPDATED......\r\n");
					delay(10);
					RF_status = 0;
				} else {
					PRINTF("\033[16;0H INVALID!!!\r\nTRY AGAIN...");
					delay(10);
					PRINTF("\033[16;0H                              ");
					PRINTF("\033[17;0H                                                                                  ");
					PRINTF("\033[15;0H                                                                                                               ");
					PRINTF("\033[15;36H");
					RF_status = 1;
				}
			} while (RF_status);
			status = 1;

		} else if ((option == 'b') || (option == 'B')) {
			*pattern_config_usroption = 'B';
			status = 0;
		} else if ((option == 'n') || (option == 'N')) {
			*pattern_config_usroption = 'N';
			status = 0;
			return 0;
		}
	} while (status);
	return 1;
}

int ModeSelect_screen(int *mode_usroption,int *mode_option_string_value,char *mode_option_string) {
	char option;
	int status;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF("************************ MODE ************************\n\n\r");
		PRINTF("-----  CURRENT MODE                  : %s  -----\n\n\r",mode_option_string);
		PRINTF("CHOOSE MODE\n\r");
		PRINTF("    Q      : AUTO UP\n\r");
		PRINTF("    W      : AUTO DOWN\n\r");
		PRINTF("    E      : AUTO UP/DOWN\n\r");
		PRINTF("    R      : MANUAL\n\n\n\r");
		PRINTF("        B : BACK\n\r");
		PRINTF("SELECT AN OPTION     :  ");
		while(!interrupt_flag) {
		}
		option = uart_read;
		interrupt_flag = 0;
		PRINTF("%c\r\n",option);
		if ((option == 'q') || (option == 'Q')) {
			*mode_usroption = 1;
			strcpy(mode_option_string, "AUTO UP");
			PRINTF("SUCCESSFULLY UPDATED");
			*mode_option_string_value = 1;
			delay(10);
			status = 1;
		} else if ((option == 'w') || (option == 'W')) {
			*mode_usroption = 2;
			strcpy(mode_option_string, "AUTO DOWN");
			*mode_option_string_value = 2;
			status = 1;
			PRINTF("SUCCESSFULLY UPDATED");
			delay(10);
		} else if ((option == 'e') || (option == 'E')) {
			*mode_usroption = 3;
			strcpy(mode_option_string, "AUTO UP/DOWN");
			*mode_option_string_value = 3;
			status = 1;
			PRINTF("SUCCESSFULLY UPDATED");
			delay(10);
		} else if ((option == 'r') || (option == 'R')) {
			*mode_usroption = 4;
			strcpy(mode_option_string, "MANUAL");
			*mode_option_string_value = 4;
			status = 1;
			PRINTF("SUCCESSFULLY UPDATED");
			delay(10);
		} else if ((option == 'b') || (option == 'B')) {
			*mode_usroption = 11;
			status = 0;
		} else {
			PRINTF("INVALID OPTION!!!");
			delay(10);
			status = 1;

		}
	} while (status);
	return 1;
}

int Find_colour_screen(int *find_colour,char *find_usroption) {
	int strt_success;
	char option;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF("************************ FIND COLOUR ************************\n\n\r");
		PRINTF("\033[4;0H       T : SEARCH\n\r");
		PRINTF("\033[5;0H       B : BACK\n\r");
		PRINTF("\033[6;0HCURRENTLY DISPLAYING COLOUR CODE     : ");
		for (int i = 0; i <= 3; i++) {
			if (i == 3)
				PRINTF("\r\n");
			else {
				PRINTF("%d", find_colour[i]);
				if (i < 2) {
					PRINTF(",");
				}
			}
		}
		while(!interrupt_flag) {
			PRINTF("\033[8;0HPRESS 'T' TO SEARCH");
			delay(5);
			PRINTF("\033[8;0H                   ");
			delay(5);
		}
		PRINTF("\033[4;0H                                       \n\r");
		PRINTF("\033[5;0H                                       \n\r");
		PRINTF("\033[6;0H                                                                   ");
		option = uart_read;
		interrupt_flag = 0;
		if((option == 't')||(option == 'T')) {
			PRINTF("\033[14;0H                                   ");
			PRINTF("\033[4;0HENTER THE COLOUR(R(0 to 7);G(0 to 7);B(0 to 3):\n\n\n\r");
			PRINTF("\033[5;5HR : ");
			PRINTF("\033[6;5HG : ");
			PRINTF("\033[7;5HB : ");
			PRINTF("\033[5;9H");
			while (1) {
				while(!interrupt_flag) {
				}
				find_colour[0] = uart_read;
				interrupt_flag = 0;
				if((find_colour[0] >= 48) && (find_colour[0] <= 55)) {
					find_colour[0] = find_colour[0] - 48;
					PRINTF("\033[5;9H%d\r\n",find_colour[0]);
					break;
				} else {
					PRINTF("\033[13;9HINVALID ENTRY");
					delay(10);
					PRINTF("\033[13;9H                            ");
					PRINTF("\033[5;9H                                 ");
					PRINTF("\033[5;9H");
					continue;
				}
			}
			PRINTF("\033[6;5HG : ");
			while (1) {
				while(!interrupt_flag) {
				}
				find_colour[1] = uart_read;
				interrupt_flag = 0;
				if((find_colour[1] >= 48) && (find_colour[1] <= 55)) {
					find_colour[1] = find_colour[1] - 48;
					PRINTF("\033[6;9H%d\r\n",find_colour[1]);
					break;
				} else {
					PRINTF("\033[13;9HINVALID ENTRY");
					delay(10);
					PRINTF("\033[13;9H                            ");
					PRINTF("\033[6;9H                                 ");
					PRINTF("\033[6;9H");
					continue;
				}
			}
			PRINTF("\033[7;5HB : ");
			while (1) {
				while(!interrupt_flag) {
				}
				find_colour[2] = uart_read;
				interrupt_flag = 0;
				if((find_colour[2] >= 48) && (find_colour[2] <= 51)) {
					find_colour[2] = find_colour[2] - 48;
					PRINTF("\033[7;9H%d\r\n",find_colour[2]);
					break;
				} else {
					PRINTF("\033[13;9HINVALID ENTRY");
					delay(10);
					PRINTF("\033[13;9H                            ");
					PRINTF("\033[7;9H                                 ");
					PRINTF("\033[7;9H");
					continue;
				}
			}
			PRINTF("\033[14;0HNote : OBSERVE THE LED\n\n\n\r");
			delay(10);
			PRINTF("\033[5;9H                            ");
			PRINTF("\033[6;9H                            ");
			PRINTF("\033[7;9H                            ");
			*find_usroption = 'S';
			strt_success = 0;
		} else if ((option == 'b') || (option == 'B')) {
			*find_usroption = 'B';
			find_colour[0] = 0;
			find_colour[1] = 0;
			find_colour[2] = 0;
			strt_success = 0;
		} else {
			PRINTF("INVALID OPTION");
			strt_success = 1;
		}
	} while (strt_success);
	return 1;
}

void Slave_homescreen(void) {
	PRINTF("\e[1;1H\e[2J");
	PRINTF(
			"************************************************************** WELCOME TO PRISMA **************************************************************\n\r");
	PRINTF(
			"                                                     GENERATE YOUR OWN LED COLOUR PATTERN\n\n\r");
	PRINTF("         **CURRENT CONFIGURATION**\n\n\r");
	PRINTF("...SYSTEM CONFIGURATION...\n\n\r");
	PRINTF(" CONFIGURED AS                      : SLAVE\n\r");
	PRINTF(" RGB CODING SCHEME                  : 7,7,3\n\r");
	PRINTF(" REFRESH RATE                       : 5\n\r");
	PRINTF(" COLOUR CHANGE RATE                 : 10\n\n\r");
	PRINTF("...PATTERN CONFIGURATION...\n\n\r");
	PRINTF(" START COLOUR                       : 0,0,0\n\r");
	PRINTF(" END COLOUR                         : 7,7,3\n\r");
	PRINTF(" RESOLUTION                         : 1,1,1\n\r");
	PRINTF(" MODE, CYCLE                        : AUTO UP, 5\n\n\r");
	PRINTF("                **STATUS**\n\n\r");
	PRINTF(" CURRENT LED COLOUR                 : 0,0,0\n\r");
	PRINTF(" NO OF CYCLES COMPLETED             : 0\n\n\r");
}

void Slave_homescreen_failed(void) {
	PRINTF(
			"************************************************************** WELCOME TO PRISMA **************************************************************\n\r");
	PRINTF(
			"                                                     GENERATE YOUR OWN LED COLOUR PATTERN\n\n\r");
	PRINTF("WAITING FOR MASTER.......");
}

int Help_screen(char *help_scrn_usroption) {
	int status = 1;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF("WAITING FOR MASTER.......");
		while(!interrupt_flag) {
		}
		*help_scrn_usroption = uart_read;
		interrupt_flag = 0;
		if((*help_scrn_usroption == 'q') || (*help_scrn_usroption == 'Q')) {
			*help_scrn_usroption = 'Q';
			status = 0;
		} else {
			PRINTF("invalid option entered");
			status = 1;
		}
	} while(status);
	return 1;
}

