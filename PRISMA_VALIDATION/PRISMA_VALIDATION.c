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
	char mode_option_string[10] = "AUTO UP";
	int start_colour[3] = {0,0,0};
	int end_colour[3] = {7,7,3};
	int resolution[3] = {1,1,1};
	int find_colour[3];
	int refresh_rate = 5;
	int colour_change_rate = 10;
	int repeat_cycle = 5;
	int mode_usroption = 1;
	homescreen();
	while(1) {
		Master_homescreen(&master_home_usroption);
		if(master_home_usroption == 'S') {
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
			while(ModeSelect_screen(&mode_usroption,mode_option_string)){
				if(mode_usroption == 11) {
					break;
				}
			}
		} else if (master_home_usroption == 'V'){
			while(Find_colour_screen(find_colour)) {
				if(pattern_config_usroption == 'B') {
					break;
				} else if(pattern_config_usroption == 'N') {

				}
			}
		} else if (master_home_usroption == 'H'){
		} else {
		}

	}
	return 0;
}
int Slave_UI() {
	return 0;
}
