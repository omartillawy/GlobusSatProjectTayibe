/*
 * main.c
 *      Author: Akhil
 */

#include "Demos/IsisAntSdemo.h"
#include "Demos/isis_ants2_demo.h"
#include "Demos/GomEPSdemo.h"
#include "Demos/IsisSPdemo.h"
#include "Demos/IsisSPv2demo.h"
#include "Demos/IsisTRXUVdemo.h"
#include "Demos/IsisTRXVUdemo.h"
#include "Demos/IsisMTQv1demo.h"
#include "Demos/IsisMTQv2demo.h"
#include "Demos/cspaceADCSdemo.h"
#include "Demos/ScsGeckoDemo.h"
#include "Demos/IsisHSTxSdemo.h"
#include "Demos/isis_eps_demo.h"
#include "Demos/tausat2_pdhudemo.h"
#include <satellite-subsystems/version/version.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>
#include <at91/peripherals/pio/pio_it.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/SPI.h>
#include <hal/boolean.h>
#include <hal/version/version.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ENABLE_MAIN_TRACES 1
#if ENABLE_MAIN_TRACES
	#define MAIN_TRACE_INFO			TRACE_INFO
	#define MAIN_TRACE_DEBUG		TRACE_DEBUG
	#define MAIN_TRACE_WARNING		TRACE_WARNING
	#define MAIN_TRACE_ERROR		TRACE_ERROR
	#define MAIN_TRACE_FATAL		TRACE_FATAL
#else
	#define MAIN_TRACE_INFO(...)	{ }
	#define MAIN_TRACE_DEBUG(...)	{ }
	#define MAIN_TRACE_WARNING(...)	{ }
	#define MAIN_TRACE_ERROR		TRACE_ERROR
	#define MAIN_TRACE_FATAL		TRACE_FATAL
#endif

Boolean selectAndExecuteTest()
{
	int selection = 0;
	Boolean offerMoreTests = TRUE;

	//Initialize the I2C
	int retValInt = I2C_start(100000, 10);
	if(retValInt != 0)
	{
		TRACE_FATAL("\n\r I2C_start_Master for demo: %d! \n\r", retValInt);
	}

	printf( "\n\r Select the device to be tested to perform: \n\r");
	printf("\t 1) TRXVU test \n\r");
	printf("\t 2) HSTxS Test \n\r");
	printf("\t 3) AntS test \n\r");
	printf("\t 4) Solar Panels V2 test \n\r");
	printf("\t 5) MTQv2 test \n\r");
	printf("\t 6) ISIS EPS Test \n\r");
	printf("\t 7) TAUSAT2 PDHU test\n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 11) == 0);

	switch(selection)
	{
		case 1:
			offerMoreTests = TRXVUtest();
			break;
		case 2:
			offerMoreTests = HSTxStest();
			break;
		case 3:
			offerMoreTests = AntStest();
			break;
		case 4:
			offerMoreTests = SolarPanelv2test();
			break;
		case 5:
			offerMoreTests = IsisMTQv2test();
			break;
		case 6:
			offerMoreTests = isis_eps__test();
			break;
		case 7:
			offerMoreTests = TAUSAT2PdhuDemoMain();
			break;
		case 8:
			turnOnTransponder(); // test 2
			break;
		case 9:
			turnOffTransponder(); // test 2
			break;
		case 10:
			Show_Telemetry(); // test 3
			break;
		case 11:
			Show_Operation_Modes(); // test 4
			break;

		default:
			break;
	}

	return offerMoreTests;
}

void taskMain()
{
	Boolean offerMoreTests = FALSE;

	WDT_startWatchdogKickTask(10 / portTICK_RATE_MS, FALSE);

	while(1)
	{
		LED_toggle(led_1);

		offerMoreTests = selectAndExecuteTest();

		if(offerMoreTests == FALSE)
		{
			break;
		}
	}

	while(1) {
		LED_toggle(led_1);
		vTaskDelay(500);
	}

}

int main()
{
	unsigned int i;
	xTaskHandle taskMainHandle;

	TRACE_CONFIGURE_ISP(DBGU_STANDARD, 115200, BOARD_MCK);
	// Enable the Instruction cache of the ARM9 core. Keep the MMU and Data Cache disabled.
	CP15_Enable_I_Cache();

	LED_start();

	// The actual watchdog is already started, this only initializes the watchdog-kick interface.
	WDT_start();

	PIO_InitializeInterrupts(AT91C_AIC_PRIOR_LOWEST+4);

	printf("\n\nDemo applications for ISIS OBC Satellite Subsystems Library built on %s at %s\n", __DATE__, __TIME__);
	printf("\nDemo applications use:\n");
	printf("* Sat Subsys lib version %s.%s.%s built on %s at %s\n",
			SatelliteSubsystemsVersionMajor, SatelliteSubsystemsVersionMinor, SatelliteSubsystemsVersionRevision,
			SatelliteSubsystemsCompileDate, SatelliteSubsystemsCompileTime);
	printf("* HAL lib version %s.%s.%s built on %s at %s\n", HalVersionMajor, HalVersionMinor, HalVersionRevision,
			HalCompileDate, HalCompileTime);

	LED_wave(1);
	LED_waveReverse(1);
	LED_wave(1);
	LED_waveReverse(1);

	MAIN_TRACE_DEBUG("\t main: Starting main task.. \n\r");
	xTaskGenericCreate(taskMain, (const signed char*)"taskMain", 4096, NULL, configMAX_PRIORITIES-2, &taskMainHandle, NULL, NULL);

	MAIN_TRACE_DEBUG("\t main: Starting scheduler.. \n\r");
	vTaskStartScheduler();

	// This part should never be reached.
	MAIN_TRACE_DEBUG("\t main: Unexpected end of scheduling \n\r");

	//Flash some LEDs for about 100 seconds
	for (i=0; i < 2500; i++)
	{
		LED_wave(1);
		MAIN_TRACE_DEBUG("MAIN: STILL ALIVE %d\n\r", i);
	}
	exit(0);
}
void show_SP_Telemetry(){
	unsigned short paneltemp[5];
		float conv_temp[5];

		print_error(IsisSolarPanel_getTemperature(slave3_spi, &paneltemp[0]));
		print_error(IsisSolarPanel_getTemperature(slave4_spi, &paneltemp[1]));
		print_error(IsisSolarPanel_getTemperature(slave5_spi, &paneltemp[2]));
		print_error(IsisSolarPanel_getTemperature(slave6_spi, &paneltemp[3]));
		print_error(IsisSolarPanel_getTemperature(slave7_spi, &paneltemp[4]));

		conv_temp[0] = (float)((short)paneltemp[0]) * 0.0078125;
		conv_temp[1] = (float)((short)paneltemp[1]) * 0.0078125;
		conv_temp[2] = (float)((short)paneltemp[2]) * 0.0078125;
		conv_temp[3] = (float)((short)paneltemp[3]) * 0.0078125;
		conv_temp[4] = (float)((short)paneltemp[4]) * 0.0078125;

		printf("\r\n Temperature values \r\n");
		printf("\r\n %f \n", conv_temp[0]);
		printf("\r\n %f \n", conv_temp[1]);
		printf("\r\n %f \n", conv_temp[2]);
		printf("\r\n %f \n", conv_temp[3]);
		printf("\r\n %f \n", conv_temp[4]);

		vTaskDelay(1 / portTICK_RATE_MS);

}
void show_Ant_Telemetry(){
	unsigned char antennaSystemsIndex = 0;
		ISISantsTelemetry allTelem;
		float eng_value = 0;
		int sides[] = {isisants_sideA, isisants_sideB};
        int k;
		printf("\r\nAntS ");
		for (k = 0; k < 2; k++){
		print_error(IsisAntS_getAlltelemetry(antennaSystemsIndex, sides[k], &allTelem));

		printf("Current deployment status 0x%x 0x%x (raw value) \r\n", allTelem.fields.ants_deployment.raw[0], allTelem.fields.ants_deployment.raw[1]);
		printDeploymentStatus(1, allTelem.fields.ants_deployment.fields.ant1Undeployed);
		printDeploymentStatus(2, allTelem.fields.ants_deployment.fields.ant2Undeployed);
		printDeploymentStatus(3, allTelem.fields.ants_deployment.fields.ant3Undeployed);
		printDeploymentStatus(4, allTelem.fields.ants_deployment.fields.ant4Undeployed);

		eng_value = ((float)allTelem.fields.ants_temperature * -0.2922) + 190.65;
		printf("\r\n AntS temperature %f deg. C\r\n", eng_value);
		printf("\r\n AntS uptime %d sec. \r\n", allTelem.fields.ants_uptime);
		}
}
void show_GoM_EPS_Telemetry(){
	gom_eps_hk_t myEpsTelemetry_hk;

		printf("\r\nEPS Telemetry HK General \r\n\n");
		print_error(GomEpsGetHkData_general(0, &myEpsTelemetry_hk));

		printf("Voltage of boost converters PV1 = %d mV\r\n", myEpsTelemetry_hk.fields.vboost[0]);
		printf("Voltage of boost converters PV2 = %d mV\r\n", myEpsTelemetry_hk.fields.vboost[1]);
		printf("Voltage of boost converters PV3 = %d mV\r\n", myEpsTelemetry_hk.fields.vboost[2]);

		printf("Voltage of the battery = %d mV\r\n", myEpsTelemetry_hk.fields.vbatt);

		printf("Current inputs current 1 = %d mA\r\n", myEpsTelemetry_hk.fields.curin[0]);
		printf("Current inputs current 2 = %d mA\r\n", myEpsTelemetry_hk.fields.curin[1]);
		printf("Current inputs current 3 = %d mA\r\n", myEpsTelemetry_hk.fields.curin[2]);

		printf("Current from boost converters = %d mA\r\n", myEpsTelemetry_hk.fields.cursun);
		printf("Current out of the battery = %d mA\r\n", myEpsTelemetry_hk.fields.cursys);

		printf("Current outputs current 1 = %d mA\r\n", myEpsTelemetry_hk.fields.curout[0]);
		printf("Current outputs current 2 = %d mA\r\n", myEpsTelemetry_hk.fields.curout[1]);
		printf("Current outputs current 3 = %d mA\r\n", myEpsTelemetry_hk.fields.curout[2]);
		printf("Current outputs current 4 = %d mA\r\n", myEpsTelemetry_hk.fields.curout[3]);
		printf("Current outputs current 5 = %d mA\r\n", myEpsTelemetry_hk.fields.curout[4]);
		printf("Current outputs current 6 = %d mA\r\n", myEpsTelemetry_hk.fields.curout[5]);

		printf("Output Status 1 = %d \r\n", myEpsTelemetry_hk.fields.output[0]);
		printf("Output Status 2 = %d \r\n", myEpsTelemetry_hk.fields.output[1]);
		printf("Output Status 3 = %d \r\n", myEpsTelemetry_hk.fields.output[2]);
		printf("Output Status 4 = %d \r\n", myEpsTelemetry_hk.fields.output[3]);
		printf("Output Status 5 = %d \r\n", myEpsTelemetry_hk.fields.output[4]);
		printf("Output Status 6 = %d \r\n", myEpsTelemetry_hk.fields.output[5]);
		printf("Output Status 7 = %d \r\n", myEpsTelemetry_hk.fields.output[6]);
		printf("Output Status 8 = %d \r\n", myEpsTelemetry_hk.fields.output[7]);

		printf("Output Time until Power is on 1 = %d \r\n", myEpsTelemetry_hk.fields.output_on_delta[0]);
		printf("Output Time until Power is on 2 = %d \r\n", myEpsTelemetry_hk.fields.output_on_delta[1]);
		printf("Output Time until Power is on 3 = %d \r\n", myEpsTelemetry_hk.fields.output_on_delta[2]);
		printf("Output Time until Power is on 4 = %d \r\n", myEpsTelemetry_hk.fields.output_on_delta[3]);
		printf("Output Time until Power is on 5 = %d \r\n", myEpsTelemetry_hk.fields.output_on_delta[4]);
		printf("Output Time until Power is on 6 = %d \r\n", myEpsTelemetry_hk.fields.output_on_delta[5]);
		printf("Output Time until Power is on 7 = %d \r\n", myEpsTelemetry_hk.fields.output_on_delta[6]);
		printf("Output Time until Power is on 8 = %d \r\n", myEpsTelemetry_hk.fields.output_on_delta[7]);

		printf("Output Time until Power is off 1 = %d \r\n", myEpsTelemetry_hk.fields.output_off_delta[0]);
		printf("Output Time until Power is off 2 = %d \r\n", myEpsTelemetry_hk.fields.output_off_delta[1]);
		printf("Output Time until Power is off 3 = %d \r\n", myEpsTelemetry_hk.fields.output_off_delta[2]);
		printf("Output Time until Power is off 4 = %d \r\n", myEpsTelemetry_hk.fields.output_off_delta[3]);
		printf("Output Time until Power is off 5 = %d \r\n", myEpsTelemetry_hk.fields.output_off_delta[4]);
		printf("Output Time until Power is off 6 = %d \r\n", myEpsTelemetry_hk.fields.output_off_delta[5]);
		printf("Output Time until Power is off 7 = %d \r\n", myEpsTelemetry_hk.fields.output_off_delta[6]);
		printf("Output Time until Power is off 8 = %d \r\n", myEpsTelemetry_hk.fields.output_off_delta[7]);

		printf("Number of latch-ups 1 = %d \r\n", myEpsTelemetry_hk.fields.latchup[0]);
		printf("Number of latch-ups 2 = %d \r\n", myEpsTelemetry_hk.fields.latchup[1]);
		printf("Number of latch-ups 3 = %d \r\n", myEpsTelemetry_hk.fields.latchup[2]);
		printf("Number of latch-ups 4 = %d \r\n", myEpsTelemetry_hk.fields.latchup[3]);
		printf("Number of latch-ups 5 = %d \r\n", myEpsTelemetry_hk.fields.latchup[4]);
		printf("Number of latch-ups 6 = %d \r\n", myEpsTelemetry_hk.fields.latchup[5]);

		printf("Time left on I2C WDT = %d \r\n", myEpsTelemetry_hk.fields.wdt_i2c_time_left);
		printf("Time left on WDT GND = %d \r\n", myEpsTelemetry_hk.fields. wdt_gnd_time_left);
		printf("Time left on I2C WDT CSP ping 1 = %d \r\n", myEpsTelemetry_hk.fields.wdt_csp_pings_left[0]);
		printf("Time left on I2C WDT CSP ping 2 = %d \r\n", myEpsTelemetry_hk.fields.wdt_csp_pings_left[1]);

		printf("Number of I2C WD reboots = %d\r\n", myEpsTelemetry_hk.fields.counter_wdt_i2c);
		printf("Number of WDT GND reboots = %d\r\n", myEpsTelemetry_hk.fields.counter_wdt_gnd);
		printf("Number of WDT CSP ping 1 reboots = %d\r\n", myEpsTelemetry_hk.fields.counter_wdt_csp[0]);
		printf("Number of WDT CSP ping 2 reboots = %d\r\n", myEpsTelemetry_hk.fields.counter_wdt_csp[0]);
		printf("Number of EPS reboots = %d\r\n", myEpsTelemetry_hk.fields.counter_boot);

		printf("Temperature sensors. TEMP1 = %d \r\n", myEpsTelemetry_hk.fields.temp[0]);
		printf("Temperature sensors. TEMP2 = %d \r\n", myEpsTelemetry_hk.fields.temp[1]);
		printf("Temperature sensors. TEMP3 = %d \r\n", myEpsTelemetry_hk.fields.temp[2]);
		printf("Temperature sensors. TEMP4 = %d \r\n", myEpsTelemetry_hk.fields.temp[3]);
		printf("Temperature sensors. BATT0 = %d \r\n", myEpsTelemetry_hk.fields.temp[4]);
		printf("Temperature sensors. BATT1 = %d \r\n", myEpsTelemetry_hk.fields.temp[5]);

		printf("Cause of last EPS reset = %d\r\n", myEpsTelemetry_hk.fields.bootcause);
		printf("Battery Mode = %d\r\n", myEpsTelemetry_hk.fields.battmode);
		printf("PPT tracker Mode = %d\r\n", myEpsTelemetry_hk.fields.pptmode);
		printf(" \r\n");
}
void show_TRXVU_Telemetry(){
	vurc_getRxTelemTest_revD();
	vutc_getTxTelemTest_revD();
}

//test 3
void Show_Telemetry(){
	show_SP_Telemetry();
	show_Ant_Telemetry();
	show_GoM_EPS_Telemetry();
	show_TRXVU_Telemetry();
}
