#include <stdio.h>
#include "fsl_debug_console.h"
#include "../PRISMA_SCREEN_INCLUDES/PRISMA_SCREEN.c"
int Master_UI() {
	char master_home_usroption;
	char edit_scrn_usroptn;
	char system_config_user_option;
	char colour_coding_scheme_useroptn;
	char colour_coding_scheme_value[3] = {7,7,3};
	char rate_config_useroptn;
	char pattern_config_usroption;
	char find_usroption;
	char help_scrn_usroption;
	char mode_option_string[10] = "AUTO UP";
	int mode_option_string_value = 1;
	int start_colour[3] = {0,0,0};
	int end_colour[3] = {7,7,3};
	int resolution[3] = {1,1,1};
	int find_colour[3] = {0,0,0};
	int refresh_rate = 5;
	int colour_change_rate = 10;
	int repeat_cycle = 5;
	int mode_usroption = 1;

	homescreen();
	while(1) {
		Master_homescreen(&master_home_usroption,colour_coding_scheme_value,&refresh_rate,&colour_change_rate,start_colour,end_colour,resolution,mode_option_string,&repeat_cycle);
		if(master_home_usroption == 'S') {
			while(1) {
				int send_status_flag;
				if (mode_option_string_value == 1) {    //validation for AUTO UP
					if (start_colour[0] < end_colour[0]) {
						if(resolution[0] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else if (start_colour[0] == end_colour[0]) {
						if(resolution[0] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)\r\n",start_colour[0],end_colour[0]);
						delay(20);
					}
					if (start_colour[1] < end_colour[1]) {
						if(resolution[1] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else if (start_colour[1] == end_colour[1]) {
						if(resolution[1] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
						delay(20);
					}

					if (start_colour[2] < end_colour[2]) {
						if(resolution[2] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}

					} else if (start_colour[2] == end_colour[2]) {
						if(resolution[2] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
						delay(20);
						break;
					}
				} else if(mode_option_string_value == 2) {              //validation for AUTO DOWN
					if (start_colour[0] > end_colour[0]) {
						if(resolution[0] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}

					} else if (start_colour[0] == end_colour[0]) {
						if(resolution[0] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)",start_colour[0],end_colour[0]);
						delay(20);
					}
					if (start_colour[1] > end_colour[1]) {
						if(resolution[1] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}

					} else if (start_colour[1] == end_colour[1]) {
						if(resolution[1] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
						delay(20);
					}

					if (start_colour[2] > end_colour[2]) {
						if(resolution[2] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}

					} else if (start_colour[2] == end_colour[2]) {
						if(resolution[2] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
						delay(20);
						break;
					}

				} else if(mode_option_string_value == 3) {                     //validation for AUTO UP/DOWN
					if (start_colour[0] < end_colour[0]) {
						if(resolution[0] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else if (start_colour[0] == end_colour[0]) {
						if(resolution[0] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)",start_colour[0],end_colour[0]);
						delay(20);
					}
					if (start_colour[1] < end_colour[1]) {
						if(resolution[1] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}

					} else if (start_colour[1] == end_colour[1]) {
						if(resolution[1] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
						delay(20);
					}

					if (start_colour[2] < end_colour[2]) {
						if(resolution[2] > 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}

					} else if (start_colour[2] == end_colour[2]) {
						if(resolution[2] == 0) {
							send_status_flag = send_status_flag + 1;
						} else {
							PRINTF("THE START COLOUR OF RED IS %d>>>>>> END COLOUR IS %d>>>>>>RESOLUTION IS %d>>>>>>>IS INVALID<<<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
							delay(20);
							break;
						}
					} else {
						PRINTF("TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
						delay(20);
						break;
					}


				} else if(mode_option_string_value == 4) {                      // MANUAL validation
				}
				//implement queue send
				PRINTF("success");
				delay(20);
				break;
			}
		} else if(master_home_usroption == 'D'){
		} else if(master_home_usroption == 'F'){
			while (Edit_config_screen(&edit_scrn_usroptn)) {
				if (edit_scrn_usroptn == '1') {
					while (System_config_screen(&system_config_user_option)) {
						if(system_config_user_option == '1') {
							while (Colour_coding_scheme_screen(&colour_coding_scheme_useroptn,colour_coding_scheme_value)) {
								if(colour_coding_scheme_useroptn == 'B') {
									break;
								} else if(colour_coding_scheme_useroptn == 'N') {
									//implement return home operation
								}
							}
						} else if (system_config_user_option == '2') {
							while (Rate_configuration_screen(&rate_config_useroptn,&refresh_rate,&colour_change_rate)) {
								if(rate_config_useroptn == 'B') {
									break;
								} else if(rate_config_useroptn == 'N') {
									//implement return home operation
								}
							}
						} else if (system_config_user_option == 'B') {
							break;
						} else {
						}
					}
				} else if (edit_scrn_usroptn == '2') {
					while(Pattern_configuration_screen(start_colour, end_colour, resolution, &pattern_config_usroption,&repeat_cycle)) {
						if(pattern_config_usroption == 'B') {
							break;
						} else if(pattern_config_usroption == 'N') {
							//implement return home operation
						}
					}
				} else if (edit_scrn_usroptn == 'B') {
					break;
				} else ;
			}
		} else if (master_home_usroption == 'X'){
			while(ModeSelect_screen(&mode_usroption,&mode_option_string_value,mode_option_string)){
				if(mode_usroption == 11) {
					break;
				}
			}
		} else if (master_home_usroption == 'V'){
			while(Find_colour_screen(find_colour,&find_usroption)) {
				if(find_usroption == 'B') {
					break;
				} else if (find_usroption == 'S') {
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
	return 0;
}

int Slave_UI() {
	return 0;
}
