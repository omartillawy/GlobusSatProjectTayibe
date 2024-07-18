/*
 * TLM_management.c
 *
 *  Created on: 12 αιεμ 2024
 *      Author: 2022
 */


#include <satellite-subsystems/IsisTRXUV.h>
#include <satellite-subsystems/IsisAntS.h>
#include <satellite-subsystems/imepsv2_piu.h>
#include <hcc/api_mdriver_atmel_mcipdc.h>
#include <TLM_management.h>
#include <hal/errors.h>
#include <hcc/api_fat.h>

#define SD_CARD_DRIVER_PRI 0
#define SD_CARD_DRIVER_SEC 1
#define FIRST_TIME -1

FileSystemResult InitializeFS(Boolean first_time){

int err=hcc_mem_init();
if(err!=E_NO_SS_ERR)
	printf("hcc_mem_init error:",err);

err=fs_init();
if(err != E_NO_SS_ERR){
printf("fs_init error:",err);
}

fs_start();

err=f_enterFS();

if(err != E_NO_SS_ERR)
	printf("f_entersFS error:",err);

//init the volume of SD card 0(A)
 // todo make indentation
err=f_initvolume(0,atmel_mcipdc_initfunc,SD_CARD_DRIVER_PRI);
if(err!=E_NO_SS_ERR)
	printf("f_initvoulume primary error :%d\n",err);

DeInitializeFS(SD_CARD_DRIVER_PRI);

hcc_mem_init();
fs_init();
f_enterFS();
err=f_initvolume(0,atmel_mcipdc_initfunc,SD_CARD_DRIVER_SEC);
if(err !=E_NO_SS_ERR)
	printf("f_initvolume secondary error:%d\n",err);
return FS_SUCCSESS;
}
