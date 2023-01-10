#include <stdio.h>
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_uart.h"

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

//PRINTF("\033[0;0H %d", );

#define true 1
#define false 0

int interrupt_flag = 0;

void PRISMA_UART_IRQHandler(void) {
	interrupt_flag = true;
	//PRISMA_UART_READ_DATA = UART_ReadByte(PRISMA_UART);
	SDK_ISR_EXIT_BARRIER;
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

int Master_homescreen(char *Master_Home_Usroption) {
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
		PRINTF(" RGB CODING SCHEME                  : 7,7,3\n\r");
		PRINTF(" REFRESH RATE                       : 5\n\r");
		PRINTF(" COLOUR CHANGE RATE                 : 10\n\n\r");
		PRINTF("...PATTERN CONFIGURATION...\n\n\r");
		PRINTF(" START COLOUR                       : 0,0,0\n\r");
		PRINTF(" END COLOUR                         : 7,7,3\n\r");
		PRINTF(" RESOLUTION                         : 1,1,1\n\r");
		PRINTF(" MODE, CYCLE                        : AUTO UP, 5\n\n\r");
		PRINTF("                **STATUS**\n\n\r");
		PRINTF(
				" SLAVE STATUS                       : < CONNECTING/CONNECTED/DISCONNECTED >\n\r");
		PRINTF(
				" MODE OF WORKING                    : < MASTER-SLAVE/STAND ALONE >\n\r");
		PRINTF(
				" CURRENTLY WORKING IN               : < DEFAULT CONFIGURATION/USER DEFINED CONFIGURATION >\n\r");
		PRINTF(" CURRENT LED COLOUR                 : 0,0,0\n\r");
		PRINTF(" NO OF CYCLES COMPLETED             : 0\n\n\r");
		PRINTF("      S     : START/STOP\n\r");
		PRINTF("      D     : PAUSE/RESUME\n\r");
		PRINTF("      F     : EDIT CONFIGURATION\n\r");
		PRINTF("      X     : MODE\n\r");
		PRINTF("      V     : FIND COLOUR\n\r");
		PRINTF("      H     : HELP?\n\n\n\r");
		PRINTF("ENTER YOUR OPTION :");
		*Master_Home_Usroption = GETCHAR();
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
		*edit_scrn_usroptn = GETCHAR();
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
		*system_config_user_option = GETCHAR();
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
			status = 1;
		}
	} while (status);
	return 1;
}

int Colour_coding_scheme_screen(char *colour_coding_scheme_useroptn,
		char *colour_coding_scheme_value) {
	int status;
	do {
		PRINTF("\e[1;1H\e[2J");
		PRINTF(
				"********************* RGB COLOUR CODING SCHEME *********************\n\n\r");
		PRINTF("-----  CURRENT COLOUR CODING SCHEME   : ");
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
		PRINTF("CHOOSE COLOUR CODING SCHEME\n\n\r");
		PRINTF("    1.     : 7,7,3\n\r");
		PRINTF("    2.     : 4,4,4\n\r");
		PRINTF("    3.     : 8,8,8\n\n\r");
		PRINTF("       B : BACK\n\r");
		PRINTF("       N : HOME\n\n\r");
		PRINTF("ENTER YOUR OPTION :");
		*colour_coding_scheme_useroptn = GETCHAR();
		PRINTF("%c\r\n", *colour_coding_scheme_useroptn);
		delay(5);
		if (*colour_coding_scheme_useroptn == '1') {
			colour_coding_scheme_value[0] = 7;
			colour_coding_scheme_value[1] = 7;
			colour_coding_scheme_value[2] = 3;
			PRINTF("SUCCESSFULLY UPDATED");
			delay(9);
			status = 1;
		} else if (*colour_coding_scheme_useroptn == '2') {
			colour_coding_scheme_value[0] = 4;
			colour_coding_scheme_value[1] = 4;
			colour_coding_scheme_value[2] = 4;
			PRINTF("SUCCESSFULLY UPDATED");
			delay(9);
			status = 1;
		} else if (*colour_coding_scheme_useroptn == '3') {
			colour_coding_scheme_value[0] = 8;
			colour_coding_scheme_value[1] = 8;
			colour_coding_scheme_value[2] = 8;
			PRINTF("SUCCESSFULLY UPDATED");
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
		PRINTF("SELECT OPTION\r\n");
		option = GETCHAR();
		if (option == '1') {
			do {
				PRINTF("\033[9;0H ENTER NEW REFRESH RATE (1 TO 9999)     :");
				SCANF("%d", &*refresh_rate);
				PRINTF("%d\r\n", *refresh_rate);
				if ((*refresh_rate >= 1) && (*refresh_rate <= 9999)) {
					PRINTF("\033[10;0 SUCCESSFULLY UPDATED......\r\n");
					delay(10);
					RF_status = 0;
				} else {
					PRINTF("\033[10;0H INVALID!!!\r\nTRY AGAIN...");
					delay(10);
					PRINTF("\033[10;0H                              ");
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
				PRINTF("\033[9;0H ENTER NEW COLOUR CHANGE RATE(1 TO 500) :");
				SCANF("%d", &*colour_change_rate);
				PRINTF("%d\r\n", *colour_change_rate);
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
	int status,strt_success;;
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
		option = GETCHAR();
		PRINTF("%c\r\n",option);
		if (option == '1') {
			do {
				strt_success = 1;
				PRINTF("ENTER START COLOUR\r\n");
				PRINTF("\033[16;5HR : ");
				while (1) {
					SCANF("%d", &start_colour[0]);
					PRINTF("\033[16;9H%d\r\n",start_colour[0]);
					if ((start_colour[0] >= 0) && (start_colour[0] <= 7)) {
						delay(10);
						break;
					} else {
						PRINTF("\033[17;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[17;9H                            ");
						PRINTF("\033[16;9H                                 ");
						PRINTF("\033[16;9H");
						continue;
					}
				}
				PRINTF("\033[17;5HG : ");
				while (1) {
					SCANF("%d", &start_colour[1]);
					PRINTF("\033[17;9H%d\r\n",start_colour[1]);
					if ((start_colour[1] >= 0) && (start_colour[1] <= 7)) {
						delay(10);
						break;
					} else {
						PRINTF("\033[18;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[18;9H                            ");
						PRINTF("\033[17;9H                               ");
						PRINTF("\033[17;9H");
						continue;
					}
				}
				PRINTF("\033[18;5HB : ");
				while (1) {
					SCANF("%d", &start_colour[2]);
					PRINTF("\033[18;9H%d\r\n",start_colour[2]);
					if ((start_colour[2] >= 0) && (start_colour[2] <= 3)) {
						delay(10);
						strt_success = 0;
						break;
					} else {
						PRINTF("\033[19;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[19;9H                            ");
						PRINTF("\033[18;9H                                      ");
						PRINTF("\033[18;9H");
						continue;
					}
				}
			} while (strt_success);
			status = 1;
		} else if (option == '2') {
			do {
				strt_success = 1;
				PRINTF("ENTER END COLOUR\r\n");
				PRINTF("\033[16;5HR : ");
				while (1) {
					SCANF("%d", &end_colour[0]);
					PRINTF("\033[16;9H%d\r\n",end_colour[0]);
					if ((end_colour[0] >= 0) && (end_colour[0] <= 7)) {
						delay(10);
						break;
					} else {
						PRINTF("\033[17;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[17;9H                            ");
						PRINTF("\033[16;9H                                 ");
						PRINTF("\033[16;9H");
						continue;
					}
				}
				PRINTF("\033[17;5HG : ");
				while (1) {
					SCANF("%d", &end_colour[1]);
					PRINTF("\033[17;9H%d\r\n",end_colour[1]);
					if ((end_colour[1] >= 0) && (end_colour[1] <= 7)) {
						delay(10);
						break;
					} else {
						PRINTF("\033[18;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[18;9H                            ");
						PRINTF("\033[17;9H                               ");
						PRINTF("\033[17;9H");
						continue;
					}
				}
				PRINTF("\033[18;5HB : ");
				while (1) {
					SCANF("%d", &end_colour[2]);
					PRINTF("\033[18;9H%d\r\n",end_colour[2]);
					if ((end_colour[2] >= 0) && (end_colour[2] <= 3)) {
						delay(10);
						strt_success = 0;
						break;
					} else {
						PRINTF("\033[19;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[19;9H                            ");
						PRINTF("\033[18;9H                                      ");
						PRINTF("\033[18;9H");
						continue;
					}
				}
			} while (strt_success);
			status = 1;
		} else if (option == '3') {
			do {
				strt_success = 1;
				PRINTF("ENTER RESOLUTION COLOUR\r\n");
				PRINTF("\033[16;5HR : ");
				while (1) {
					SCANF("%d", &resolution[0]);
					PRINTF("\033[16;9H%d\r\n",resolution[0]);
					if ((resolution[0] >= 0) && (resolution[0] <= 7)) {
						delay(10);
						break;
					} else {
						PRINTF("\033[17;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[17;9H                            ");
						PRINTF("\033[16;9H                                 ");
						PRINTF("\033[16;9H");
						continue;
					}
				}
				PRINTF("\033[17;5HG : ");
				while (1) {
					SCANF("%d", &resolution[1]);
					PRINTF("\033[17;9H%d\r\n",resolution[1]);
					if ((resolution[1] >= 0) && (resolution[1] <= 7)) {
						delay(10);
						break;
					} else {
						PRINTF("\033[18;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[18;9H                            ");
						PRINTF("\033[17;9H                               ");
						PRINTF("\033[17;9H");
						continue;
					}
				}
				PRINTF("\033[18;5HB : ");
				while (1) {
					SCANF("%d", &resolution[2]);
					PRINTF("\033[18;9H%d\r\n",resolution[2]);
					if ((resolution[2] >= 0) && (resolution[2] <= 3)) {
						delay(10);
						strt_success = 0;
						break;
					} else {
						PRINTF("\033[19;9HINVALID ENTRY");
						delay(10);
						PRINTF("\033[19;9H                            ");
						PRINTF("\033[18;9H                                      ");
						PRINTF("\033[18;9H");
						continue;
					}
				}
			} while (strt_success);
			status = 1;
		} else if (option == '4') {
			int RF_status;
			int scan_success;
			do {
				PRINTF("\033[15;0H ENTER NEW REPEAT CYCLE     :    ");
				scan_success = SCANF("%d", &*repeat_cycle);
				if(scan_success) {
					PRINTF("\033[15;34H%d\r\n", *repeat_cycle);
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
		}
	} while (status);
	return 1;
}

void ModeSelect_screen(void) {
	char option;
	int status = 1;
	PRINTF("************************ MODE ************************\n\n\r");
	PRINTF("-----  CURRENT MODE                  : AUTO UP  -----\n\n\r");
	PRINTF("\033[3;40H-----\r\n\n\n\n");
	PRINTF("CHOOSE MODE\n\r");
	PRINTF("    Q      : AUTO UP\n\r");
	PRINTF("    W      : AUTO DOWN\n\r");
	PRINTF("    E      : AUTO UP/DOWN\n\r");
	PRINTF("    R      : MANUAL\n\n\n\r");
	PRINTF("        B : BACK\n\r");
	option = GETCHAR();
	do {
		PRINTF("SELECT AN OPTION     :  ");
		PRINTF("%c\r\n",option);
	} while(status);
}

void Find_colour_screen(void) {
	PRINTF(
			"************************ FIND COLOUR ************************\n\n\r");
	PRINTF(" ENTER THE COLOUR    :\n\n\n\r");
	PRINTF(" Note : look at the LED\n\n\n\r");
	PRINTF("       T : TRY AGAIN\n\r");
	PRINTF("       B : BACK\n\r");
}

void Slave_homescreen(void) {
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
