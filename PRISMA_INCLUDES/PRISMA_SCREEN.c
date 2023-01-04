#include <stdio.h>
#include "fsl_debug_console.h"
void delay(int count)
{
	while(count != 0) {
		int count1 = 900000;
		while(count1 != 0) {
			count1 --;
		}
		count--;
	}
}

void homescreen(void) {
	PRINTF("                                                                 00                                             \n\r");
	delay(1);
	PRINTF("                                                                 ""                                             \n\r");
	delay(1);
	PRINTF("                                        8b,dPPYba,   8b,dPPYba,  88  ,adPPYba,  88,dPYba,,adPYba,   ,adPPYYba,  \n\r");
	delay(1);
	PRINTF("                                        88P'    \"8a  88P'   \"Y8  88  I8[    \"\"  88P'   \"88\"    \"8a  \"\"     `Y8  \n\r");
	delay(1);
	PRINTF("                                        88       d8  88          88   `\"Y8ba,   88      88      88  ,adPPPPP88  \n\r");
	delay(1);
	PRINTF("                                        88b,   ,a8\"  88          88  aa    ]8I  88      88      88  88,    ,88  \n\r");
	delay(1);
	PRINTF("                                        88`YbbdP\"'   88          88  `\"YbbdP\"'  88      88      88  `\"8bbdP\"Y8  \n\r");
	delay(1);
	PRINTF("                                        88                                                                      \n\r");
	delay(1);
	PRINTF("                                        88                                                                      \n\r");
	delay(9);
	PRINTF("                                                                      _    _     _____    _____ \n\r");
	PRINTF("                                                                       \\  /        |     |     | \n\r");
	PRINTF("                                                                        \\/       __|__ . |_____| \n\r");
	PRINTF("                                                                                                           ");
	delay(40);
	PRINTF("\e[1;1H\e[2J");
}

int Master_homescreen(char *homereturn) {
	char useroption;
	int retvalue;
	PRINTF("************************************************************** WELCOME TO PRISMA **************************************************************\n\r");
	PRINTF("                                                     GENERATE YOUR OWN LED COLOUR PATTERN\n\n\r");
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
	PRINTF(" SLAVE STATUS                       : < CONNECTING/CONNECTED/DISCONNECTED >\n\r");
	PRINTF(" MODE OF WORKING                    : < MASTER-SLAVE/STAND ALONE >\n\r");
	PRINTF(" CURRENTLY WORKING IN               : < DEFAULT CONFIGURATION/USER DEFINED CONFIGURATION >\n\r");
	PRINTF(" CURRENT LED COLOUR                 : 0,0,0\n\r");
	PRINTF(" NO OF CYCLES COMPLETED             : 0\n\n\r");
	PRINTF("      S     : START/STOP\n\r");
	PRINTF("      D     : PAUSE/RESUME\n\r");
	PRINTF("      F     : EDIT CONFIGURATION\n\r");
	PRINTF("      X     : MODE\n\r");
	PRINTF("      V     : FIND COLOUR\n\r");
	PRINTF("      H     : HELP?\n\r");
	do {
		PRINTF("ENTER YOUR OPTION :");
		useroption = GETCHAR();
		PRINTF("%c\n\r",useroption);
		if ((useroption == 'S') || (useroption == 's')) {
			*homereturn = 's';
			retvalue = 0;
		} else if ((useroption == 'D') || (useroption == 'd')) {
			*homereturn = 'd';
			retvalue = 0;
		} else if ((useroption == 'F') || (useroption == 'f')) {
			*homereturn = 'f';
			retvalue = 0;
		} else if ((useroption == 'X') || (useroption == 'x')) {
			*homereturn = 'x';
			retvalue = 0;
		} else if ((useroption == 'V') || (useroption == 'v')) {
			*homereturn = 'v';
			retvalue = 0;
		} else if ((useroption == 'H') || (useroption == 'h')) {
			*homereturn = 'h';
			retvalue = 0;
		} else {
			PRINTF("invalid option !!!\r\n");
			retvalue =1;
		}
	} while (retvalue);
	return 1;
}

void Edit_config_screen(void) {
	PRINTF("************************ EDIT CONFIGURATION ************************\n\n\r");
	PRINTF("       1    : SYSTEM CONFIGURATION\n\r");
	PRINTF("       2    : PATTERN CONFIGURATION\n\n\n\r");
	PRINTF("       B : BACK TO HOME\n\r");
}

void System_config_screen(void) {
	PRINTF("************************ SYSTEM CONFIGURATION ************************\n\n\r");
	PRINTF("       1    : RGB COLOUR CODING SCHEME\n\r");
	PRINTF("       2    : RATES\n\n\n\r");
	PRINTF("       B : BACK\n\r");
}

void Colour_coding_scheme_screen(void) {
	PRINTF("********************* RGB COLOUR CODING SCHEME *********************\n\n\r");
	PRINTF("-----  CURRENT COLOUR CODING SCHEME   : 7,7,3  -----\n\n\r");
	PRINTF("CHOOSE COLOUR CODING SCHEME\n\n\r");
	PRINTF("    1.     : 7,7,3\n\r");
	PRINTF("    2.     : 4,4,4\n\r");
	PRINTF("    3.     : 8,8,8\n\n\r");
	PRINTF("       B : BACK\n\r");
	PRINTF("       N : HOME\n\n\r");
}

void Rate_configuration_screen(void) {
	PRINTF("********************* FILL THE DETAILS *********************\n\n\r");
	PRINTF("-----  CURRENT REFRESH RATE             : 5  -----\n\r");
	PRINTF("-----  CURRENT COLOUR CHANGE RATE       : 10 -----\n\n\r");
	PRINTF("ENTER NEW REFRESH RATE (1 TO 9999)     :\n\r");
	PRINTF("ENTER NEW COULUR CHANGE RATE(1 TO 500) :\n\n\n\n\r");
	PRINTF("         B : BACK\n\r");
	PRINTF("         N : HOME\n\r");
}

void Pattern_configuration_screen(void) {
	PRINTF("************************ PATTERN CONFIGURATION ************************\n\n\r");
	PRINTF("-----  CURRENT START COLOUR         : 0,0,0  -----\n\r");
	PRINTF("-----  CURRENT END COLOUR           : 7,7,3  -----\n\r");
	PRINTF("-----  CURRENT RESOLUTION           : 1,1,1  -----\n\r");
	PRINTF("-----  CURRENT REPEAT CYCLE         : 5      -----\n\n\r");
	PRINTF("***** FILL THE DETAILS *****\n\n\r");
	PRINTF("ENTER START COLOUR (0,0,0 TO 7,7,3) :\n\r");
	PRINTF("ENTER END COLOUR (0,0,0 TO 7,7,3)   :\n\r");
	PRINTF("ENTER RESOLUTION (0,0,0 TO 7,7,3)   :\n\r");
	PRINTF("ENTER REPEAT CYCLE                  :\n\n\n\r");
	PRINTF("        B : BACK\n\r");
	PRINTF("        N : HOME\n\r");
}

void ModeSelect_screen(void) {
	PRINTF("************************ MODE ************************\n\n\r");
	PRINTF("-----  CURRENT MODE                  : AUTO UP  -----\n\n\r");
	PRINTF("CHOOSE MODE\n\r");
	PRINTF("    Q      : AUTO UP\n\r");
	PRINTF("    W      : AUTO DOWN\n\r");
	PRINTF("    E      : AUTO UP/DOWN\n\r");
	PRINTF("    R      : MANUAL\n\n\n\r");
	PRINTF("        B : BACK\n\r");
}

void Find_colour_screen(void) {
	PRINTF("************************ FIND COLOUR ************************\n\n\r");
	PRINTF(" ENTER THE COLOUR    :\n\n\n\r");
	PRINTF(" Note : look at the LED\n\n\n\r");
	PRINTF("       T : TRY AGAIN\n\r");
	PRINTF("       B : BACK\n\r");
}

void Slave_homescreen(void) {
	PRINTF("************************************************************** WELCOME TO PRISMA **************************************************************\n\r");
	PRINTF("                                                     GENERATE YOUR OWN LED COLOUR PATTERN\n\n\r");
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
	PRINTF("************************************************************** WELCOME TO PRISMA **************************************************************\n\r");
	PRINTF("                                                     GENERATE YOUR OWN LED COLOUR PATTERN\n\n\r");
	PRINTF("WAITING FOR MASTER.......");
}
