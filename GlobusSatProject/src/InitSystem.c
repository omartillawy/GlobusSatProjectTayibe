/*
 * InitSystem.c
 *
 *  Created on: 25 במאי 2024
 *      Author: 2022
 */


#include "utils.h"
#include <SubSystemModules\PowerManagment\EPS.h>
#include <hal\Drivers\I2C.h>
#include <hal\Drivers\SPI.h>
#include <SubSystemModules/Communication/TRXVU.h>

#define BusSpeed_Hz 100000
#define Timeout 10


int StartFRAM(){
	return logError(FRAM_start() , "Start_FRAM");
}
int StartI2C(){
	return logError(I2C_start(BusSpeed_Hz,Timeout),"Start_I2C");
}

int StartSPI(){
	return logError(SPI_start(bus0_spi, slave0_spi),"Start_SPI");
}

int StartTime(){
	int error = 0;
	Time expexted_deploy_time = UNIX_DATE_JAN_D1_Y2000;
	error = Time_start(&expexted_deploy_time,0);
	if(0 != error)
		return logError(error,"Start-Time-Time-Start");
	return 0;
}
int Init_Drivers(){
	return !(StartI2C() *StartSPI() * StartTime());
}
int Init_Subsystems(){
	return !(StartFRAM() * EPS_Init() * Init_TRXVU() * InitializeFS);
}

int InitSystems(){
	Init_Drivers();
	Init_Subsystems();
}
