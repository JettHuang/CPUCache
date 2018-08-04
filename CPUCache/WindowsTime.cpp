// // \brief
//		Time Query On Windows Platform.
//

#include <Windows.h>
#include "WindowsTime.h"


static double GSecondsPerCycle = 0.0;
static LARGE_INTEGER GFrequency = { 0, 0 };

double appInitTiming()
{
	QueryPerformanceFrequency(&GFrequency);
	GSecondsPerCycle = 1.0 / GFrequency.QuadPart;

	return appSeconds();
}

double appSeconds()
{
	LARGE_INTEGER Cycles;
	QueryPerformanceCounter(&Cycles);
	return Cycles.QuadPart * GSecondsPerCycle;
}

double appMicroSeconds()
{
	LARGE_INTEGER Cycles;
	QueryPerformanceCounter(&Cycles);

	return (Cycles.QuadPart * 1000000.0) / GFrequency.QuadPart;
}

int64_t appCycles()
{
	LARGE_INTEGER Cycles;
	QueryPerformanceCounter(&Cycles);
	return Cycles.QuadPart;
}
