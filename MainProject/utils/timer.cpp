#include "stdafx.h"
#include "timer.h"



CTimer::CTimer()
{
     countsPerSecond = 0.0;
     CounterStart = 0;

     frameCount = 0;
     fps = 0;

     frameTimeOld = 0;
}


CTimer::~CTimer()
{


}


void CTimer::StartTimer()
{
    LARGE_INTEGER frequencyCount;
    QueryPerformanceFrequency(&frequencyCount);

    countsPerSecond = double(frequencyCount.QuadPart);

    QueryPerformanceCounter(&frequencyCount);
    CounterStart = frequencyCount.QuadPart;
}

double CTimer::GetTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}

double CTimer::GetFrameTime()
{
    LARGE_INTEGER currentTime;
    __int64 tickCount;
    QueryPerformanceCounter(&currentTime);

    tickCount = currentTime.QuadPart-frameTimeOld;
    frameTimeOld = currentTime.QuadPart;

    if(tickCount < 0.0f)
        tickCount = (__int64)0.0f;

    return float(tickCount)/countsPerSecond;
}
