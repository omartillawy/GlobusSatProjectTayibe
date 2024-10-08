/*
 * TRXVU.c
 *
 *  Created on: 18 ���� 2024
 *      Author: 2022
 */


#include "TRXVU.h"
#include "SubSystemModules/Communication/SatCommandHandler.h"
#include "satellite-subsystems/IsisAntS.h"
#include "utils.h"

int Init_TRXVU(){
ISIStrxvuI2CAddress myTRXVUAddress;
	ISIStrxvuFrameLengths myTRXVUFramesLength ;

	//Buffer def
	myTRXVUFramesLength.maxAX25frameLengthTX=SIZE_TXFRAME;
	myTRXVUFramesLength.maxAX25frameLengthRX=SIZE_RXFRAME;

	//I2C add def
	myTRXVUAddress.addressVu_rc=I2C_TRXVU_RC_ADDR;
	myTRXVUAddress.addressVu_tc=I2C_TRXVU_TC_ADDR;

	//Bitrate def
	ISIStrxvuBitrate myTRXVUBitrates;
	myTRXVUBitrates=trxvu_bitrate_9600;
	if(logError(IsisTrxvu_initialize(&myTRXVUAddress,&myTRXVUFramesLength,&myTRXVUBitrates,1),"Initialize-TRXVU"))
		return -1;
	vTaskDelay(1000); //wait a one second

    ISISantsI2Caddress myAntennaAddress;
    myAntennaAddress.addressSideA=ANTS_I2C_SIDE_A_ADDR;
    myAntennaAddress.addressSideB=ANTS_I2C_SIDE_B_ADDR;

    //init the ants sysytem
    if(logError(IsisTrxvu_initialize(&myAntennaAddress,&myTRXVUFramesLength,&myTRXVUBitrates,1),"InitTrxvu-IsisAntS_initialize"))
    	return -1;

    return 0;
}


int GetNumberOfFramesInBuffer(){
	unsigned short RxCounter = 0;
    logError(IsisTrxvu_rcGetFrameCount(0,&RxCounter) , "Isis FrameCount");
    return RxCounter;
}

int GetOnlineCommand(sat_packet_t * cmd){
	if(cmd == NULL)
		return -1;

	unsigned char rcframebuffer[SIZE_RXFRAME] = {0};
    ISIStrxvuRxFrame rxFrameCmd = {0,0,0,rcframebuffer};
	logError(IsisTrxvu_rcGetCommandFrame(0,&rxFrameCmd),"rxFrame");
	int err = ParseDataToCommand(rxFrameCmd.rx_framedata,cmd);
	return err;

}

int TransmitSplPacket(sat_packet_t *packet, int *avalFrames){
	if(packet == NULL)
		return -1;
	int length = sizeof(packet->ID) + sizeof(packet->cmd_subtype) + sizeof(packet->cmd_type) + sizeof(packet->data) + packet->length;
	return log_Error(IsisTrxvu_tcSendAX25DefClSign(0,(unsigned char *)packet, length,avalFrames) , "SendPacket");
}
int TRX_Logic()	{




	int frame = GetNumberOfFramesInBuffer();
	sat_packet_t cmd;
	if(frame > 0){
		GetOnlineCommand(&cmd);
		if(ActUponCommand(&cmd) == -1)
			printf("Our error");

		// .. Beacon logic
	}

 return 0;
}

int turnOnTransponder(){

	unsigned char command[] = {56,2};
	return I2C_write(0x61,command,2);
}

 int turnOffTransponder(){

	unsigned char command[] = {56,1};
	return I2C_write(0x61,command,2);
}
int CMD_SetTransponder(sat_packet_t *cmd){
	 if(!cmd)
		 return null_pointer_error;
	 char c = *(char*)cmd->data;
	 if(c == 1)
		 return turnOffTransponder();
	 return turnOnTransponder();
}

int muteTRXVU(time_unix duration){
	if(duration < 0)
		return -1;
	int err = FRAM_write((unsigned char * )duration ,  MUTE_END_TIME_ADDR, MUTE_END_TIME_SIZE);
	if(err != 0)
		return -1;
	return 0;
}
void UnMuteTRXVU(){
	time_unix t = 0;
	muteTRXVU(t);
}
time_unix getTransponderEndTime(){

	time_unix t;
	int err = FRAM_write((unsigned char * )&t ,  MUTE_END_TIME_ADDR, MUTE_END_TIME_SIZE);
	return t;

}
int SetTransponderEndTime(time_unix *t){
	int err = FRAM_write((unsigned char * )t ,  MUTE_END_TIME_ADDR, MUTE_END_TIME_SIZE);
		if(err != 0)
			return -1;
		return 0;
}

