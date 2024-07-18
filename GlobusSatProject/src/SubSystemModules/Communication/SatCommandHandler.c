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
	    // Validate input parameters
	    if (data == NULL || cmd == NULL) {
	        return -1;
	    }

	    if(cmd->ID >> 24 != ALL_SAT_ID)
	    	return -1;


	    // Calculate the minimum size of sat_packet_t without the data array
	    unsigned int min_packet_size = sizeof(unsigned int) + sizeof(char) * 2 + sizeof(unsigned short);
	    unsigned int data_length = cmd->length;

	    // Validate data length
	    if (data_length < min_packet_size) {
	        return -1;
	    }

	    // Extracting fields from the raw data using memcpy
	    unsigned int offset = 0;

	    // Copy ID
	    memcpy(&cmd->ID, data + offset, sizeof(cmd->ID));
	    offset += sizeof(cmd->ID);

	    // Copy cmd_type
	    memcpy(&cmd->cmd_type, data + offset, sizeof(cmd->cmd_type));
	    offset += sizeof(cmd->cmd_type);

	    // Copy cmd_subtype
	    memcpy(&cmd->cmd_subtype, data + offset, sizeof(cmd->cmd_subtype));
	    offset += sizeof(cmd->cmd_subtype);

	    // Copy length
	    memcpy(&cmd->length, data + offset, sizeof(cmd->length));
	    offset += sizeof(cmd->length);

	    // Validate command data length
	    if (cmd->length > MAX_COMMAND_DATA_LENGTH || cmd->length != data_length - offset) {
	        return -1;
	    }

	    // Copy data
	    memcpy(cmd->data, data + offset, cmd->length);

	    return 0;

}

//
//typedef enum __attribute__ ((__packed__)) spl_command_type_t {
//	trxvu_cmd_type,
//	eps_cmd_type,
//	telemetry_cmd_type, //2
//	filesystem_cmd_type,
//	managment_cmd_type,
//	ack_type,// 5
//	dump_type
//}spl_command_type;



int ActUponCommand(sat_packet_t *cmd){

	if(!cmd)
		return null_pointer_error;

  switch(cmd->cmd_type){
	case trxvu_cmd_type:
		    trxvu_command_router(cmd);
		    break;
	case eps_cmd_type:
			eps_command_router(cmd);
		 break;
	case telemetry_cmd_type:
		    telemetry_command_router(cmd);
		break;
	case filesystem_cmd_type:
		    filesystem_command_router(cmd);
		break;
	case managment_cmd_type:
		    managment_command_router(cmd);
		break;
	default : return -1;

	}
  return 0;


}

int AssembleCommand(unsigned char *data, unsigned short data_length, char type, char subtype,unsigned int id, sat_packet_t *cmd){

	if(MAX_COMMAND_DATA_LENGTH < data_length)
		return execution_error;

	cmd->ID = id;
	cmd->cmd_subtype = subtype;
	cmd->cmd_type = type;
	if(memcpy(cmd->data,data,data_length) == NULL)
		return execution_error;
	cmd->length = data_length;

	return command_succsess;
}

