/*
 * CommandDicitionary.c
 *
 *  Created on: 18 αιεμ 2024
 *      Author: 2022
 */
#include <stdio.h>
#include "CommandDictionary.h"
#include "SubSystemModules/PowerManagment/EPS.h"
#include "TLM_management.h"


int trxvu_command_router(sat_packet_t *cmd){

	switch(cmd->cmd_subtype){

	case 0 :
		printf(" zero ");
		break;


	}

	return 0;
}


int eps_command_router(sat_packet_t *cmd){

	if(!cmd) return null_pointer_error;

	int err = 0;
	switch(cmd->cmd_subtype){

	case UPDATE_ALPHA :
		  err = UpdateAlpha(cmd);
		break;

	case GET_HEATER_VALUES:

		break;

	case SET_HEATER_VALUES:

		break;

	case RESET_EPS_WDT:

		break;


	}
	return err;
}


int telemetry_command_router(sat_packet_t *cmd){

	if(!cmd) return null_pointer_error;
	int err = 0;
	switch(cmd->cmd_subtype){

	case DELETE_FILE :

		break;
	case DELETE_ALL_FILES :

		break;
	case GET_LAST_FS_ERROR:

		break;
	case SET_TLM_PERIOD :

		break;
	case GET_TLM_PERIOD :

		break;
	case GET_IMAGE_INFO :
		//err = CMD_getInfoImage(cmd);

		break;
	case GET_IMAGE_DATA :
		//err = CMD_getDataImage(cmd);
			break;


	}
	return err;
}

int filesystem_command_router(sat_packet_t *cmd){
	F_FILE * file;

	switch(cmd->cmd_subtype){
	case 0 :
		c_fileCreate("taybe",&file);
		fm_close(file);
		break;
	case 1:
		//find first
		break;
	case 2:
		//f_write;
		break;


	}
	return 0;
}


int managment_command_router(sat_packet_t *cmd){
	switch(cmd->cmd_subtype){

	case 0 :
		printf(" zero ");
		break;


	}
	return 0;
}






