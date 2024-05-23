#include "initSystem.h"

int InitSubsystems()
{

	StartI2C();

	StartSPI();

	StartFRAM();

	StartTIME();

     InitializeFS();

     InitSavePeriodTimes();

     EPS_Init();



}
