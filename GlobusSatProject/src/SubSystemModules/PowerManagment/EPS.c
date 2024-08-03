/*
 * EPS.c
 *
 *  Created on: 25 במאי 2024
 *      Author: 2022
 */
#include "EPS.h"
#include <satellite-subsystems/imepsv2_piu_types.h>
#include <satellite-subsystems/GomEPS.h>
#include <satellite-subsystems/imepsv2_piu.h>
#include <hal\Drivers\SPI.h>
#include <FRAM_FlightParameters.h>
#include "utils.h"

#define GetFilterVoltage(curr_voltage) (voltage_t)(alpha * curr_voltage + (1-alpha) * prev_avg)
#define Gom
int GetBatteryVoltage(voltage_t* c);
int EPS_Conditioning();
int UpdateAlpha(sat_packet_t *);

voltage_t prev_avg = 0;
float alpha = 0;
#ifdef Gom
int EPS_Init(void) // GoM
{
    unsigned char i2c_address = 0x02;
    int rv;

	rv = GomEpsInitialize(&i2c_address, 1);
	if(rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED)
		if(logError(rv, "GomEpsInitialize() failed"))
			return -1;

	 if(GetAlpha(&alpha)){
	    	alpha = DEFAULT_ALPHA_VALUE;
	    }

	    prev_avg = 0;
	    GetBatteryVoltage(&prev_avg);
	return 0;
}
#else
int EPS_Init(){ // ISIS

	IMEPSV2_PIU_t i2c_address;
	i2c_address.i2cAddr = EPS_I2C_ADDR;
	if(logError(IMEPSV2_PIU_Init(&i2c_address , 1) , "EPS_Init-ISIS")) return -1;

    if(logError(IsisSolarPanelv2_initialize(slave0_spi) , "EPS_Init- ISIS-solarPanel_init"))
			return 0;

   // if(GetThresholdVoltages(&eps_threshold_voltages)) return -1;

    if(GetAlpha(&alpha)){
    	alpha = DEFAULT_ALPHA_VALUE;
    }

    prev_avg = 0;
    GetBatteryVoltage(&prev_avg);

    return 0;
}
#endif

int EPS_Conditioning(){
	        voltage_t response1;
         #ifdef GoM
	        Gom_GetBatteryVoltage(&response1);
         #else
	        GetBatteryVoltage(&response1);
         #endif
			voltage_t Current_volt = GetFilterVoltage(response1); //mv

			if(prev_avg == -1){
				prev_avg = Current_volt;
				return 0;
			}
			if(Current_volt - prev_avg > 0) {
				if(Current_volt >= 7400)
					printf("\t Full");
				else if(Current_volt < 7400 && Current_volt >= 7200 )
					printf("\t Normal");
				else
					printf("Safe");

			}else{
				if(Current_volt > 7100 && Current_volt <= 7300)
					printf("\t normal");
				else if(Current_volt > 6500 && Current_volt <= 7100)
					printf("\t Safe");
				else
					printf("\t Critical");
			}

			prev_avg = Current_volt;
			return 0;
}

int UpdateAlpha(sat_packet_t *cmd){

	float new_alpha = *(float*)cmd->data;
	if(new_alpha < 0 || new_alpha >1)
		return logError(-2, "UpdateAlpha");
	int err= logError(FRAM_write((unsigned char*)&new_alpha , EPS_ALPHA_FILTER_VALUE_ADDR , EPS_ALPHA_FILTER_VALUE_SIZE) , "Error writeing to FRAM" );
	if(err== E_NO_SS_ERR)
		GetAlpha(&alpha);
	return err;
}

int GetBatteryVoltage(voltage_t * c){
	  imepsv2_piu__gethousekeepingeng__from_t response;

		int error = imepsv2_piu__gethousekeepingeng(0,&response);
		if( error )
		{
			if(logError(error , "imepsv2_piu__gethousekeepingeng(...)"))
							return FALSE;
		}
		*c = response.fields.batt_input.fields.volt;

		return TRUE;
}

int Gom_GetBatteryVoltage(voltage_t * c){
	    gom_eps_hk_t response;

		int error = GomEpsGetHkData_general(0,&response);
		if( error )
		{
			if(logError(error , "imepsv2_piu__gethousekeepingeng(...)"))
							return FALSE;
		}
		*c = response.fields.vbatt;

		return TRUE;
}



int GetAlpha(float *alpha){
	if(NULL == alpha)
			return E_INPUT_POINTER_NULL;
		if(logError(
				FRAM_read((unsigned char*)&alpha , EPS_ALPHA_FILTER_VALUE_ADDR , EPS_ALPHA_FILTER_VALUE_SIZE), "Error, reading from FRAM"))
			return -1;
		return 0;
}





