
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <hal/Timing/WatchDogTimer.h>
#include <hal/boolean.h>
#include <hal/Utility/util.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/SPI.h>
#include <hal/Timing/Time.h>

#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>
#include <at91/utility/exithandler.h>
#include <at91/commons.h>

#include <hcc/api_fat.h>

#include "SubSystemModules/Communication/TRXVU.h"
#include "GlobalStandards.h"
#include "SubSystemModules/PowerManagment/EPS.h"
#include "SubSystemModules/Communication/TRXVU.h"
#include "SubSystemModules/Housekepping/TelemetryCollector.h"
#include "SubSystemModules/Maintenance/Maintenance.h"
#include "InitSystem.h"
#include "main.h"
#include "stdio.h"
#include "stdlib.h"

#ifdef TESTING
	#include "TestingDemos/MainTest.h"
#else

void taskmain(){
	WDT_startWatchdogKickTask(10 /portTICK_RATE_MS,FALSE);
   InitSystems();

	while(TRUE){
		EPS_Conditioning();
		TRX_Logic();
		vTaskDelay(10);
	}
}
#endif

int main(){
	xTaskHandle taskMainHandle;
		printf("***** starting... ****\n");
			TRACE_CONFIGURE_ISP(DBGU_STANDARD, 2000000, BOARD_MCK);
			// Enable the Instruction cache of the ARM9 core. Keep the MMU and Data Cache disabled.
			CP15_Enable_I_Cache();

			// The actual watch dog is already started, this only initializes the watchdog-kick interface.
			WDT_start();

			// create the main operation task of the satellite
			xTaskGenericCreate(taskmain, (const signed char*) "taskMain", 4096, NULL,
					configMAX_PRIORITIES - 2, &taskMainHandle, NULL, NULL);
			vTaskStartScheduler();
			exit(0);
}
