/*
 * AckHandler.c
 *
 *  Created on: 18 αιεμ 2024
 *      Author: 2022
 */

#include "SatCommandHandler.h"
#include "SPL.h"

int SendAckPacket(ack_subtype_t acksubtype, sat_packet_t *cmd, unsigned char *data, unsigned short length){

	sat_packet_t ackPacket;
	int error = 0;

	error = AssembleCommand(data,length,ack_type,acksubtype,cmd->ID,&ackPacket);
	if(error != 0)
		return error;

	int avilFrames;
	error = TransmitSplPacket(&ackPacket,&avilFrames);

	//...


  return 0;


}
