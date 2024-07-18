/*
 * CommandDicitionary.c
 *
 *  Created on: 18 αιεμ 2024
 *      Author: 2022
 */



#include "CommandDictionary.h"


int trxvu_command_router(sat_packet_t *cmd){

	switch(cmd->cmd_subtype){

	case 0 :
		printf(" zero ");
		break;


	}

	return 0;
}


int eps_command_router(sat_packet_t *cmd){
	switch(cmd->cmd_subtype){

	case 0 :
		printf(" zero ");
		break;


	}
	return 0;
}


int telemetry_command_router(sat_packet_t *cmd){
	switch(cmd->cmd_subtype){

	case 0 :
		printf(" zero ");
		break;


	}
	return 0;
}

int filesystem_command_router(sat_packet_t *cmd){
	switch(cmd->cmd_subtype){

	case 0 :
		printf(" zero ");
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






