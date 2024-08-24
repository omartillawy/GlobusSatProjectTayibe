/*
 * SatCommandHandler.c

 *
 *  Created on: 18 αιεμ 2024
 *      Author: 2022
 */
#include <stdio.h>
#include <stdlib.h>


#include <string.h>
#include "SatCommandHandler.h"

#include "SPL.h"
#include "CommandDictionary.h"

int ParseDataToCommand(unsigned char * data, sat_packet_t *cmd){

	if( data == NULL || cmd == NULL)
		return -1;

	memcpy(&(cmd->ID), data, 4);
	if(cmd->ID != ALL_SAT_ID)
		return -1;
	memcpy(&(cmd->cmd_type), data+4,1);
	memcpy(&(cmd->cmd_subtype), data+5,1);
	memcpy(&(cmd->length), data+6,2);
	memcpy(&(cmd->data), data+8,cmd->length);


	return 0;
}

int ActUponCommand(sat_packet_t *cmd){

	if(!cmd)
		return null_pointer_error;
  int err = 0;
  switch(cmd->cmd_type){
	case trxvu_cmd_type:
		    err = trxvu_command_router(cmd);
		    break;
	case eps_cmd_type:
			err = eps_command_router(cmd);
		 break;
	case telemetry_cmd_type:
		    err = telemetry_command_router(cmd);
		break;
	case filesystem_cmd_type:
		    err = filesystem_command_router(cmd);
		break;
	case managment_cmd_type:
		    err = managment_command_router(cmd);
		break;
	default : err =  -1;

	}
  return err;


}

int AssembleCommand(unsigned char *data, unsigned short data_length, char type, char subtype,unsigned int id, sat_packet_t *cmd){

	if(data_length > MAX_COMMAND_DATA_LENGTH)
		return execution_error;

	cmd->ID = id;
	cmd->cmd_subtype = subtype;
	cmd->cmd_type = type;
	if(memcpy(cmd->data,data,data_length) == NULL)
		return execution_error;
	cmd->length = data_length;

	return command_succsess;
}

