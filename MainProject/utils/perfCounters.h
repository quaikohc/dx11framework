#pragma once
#include <stack>

// very basic performance counter helper class - it supports nested functions

class CPerfCounter
{
public:
    CPerfCounter(){};
    ~CPerfCounter(){};
};

namespace CPerfTimer
{
    static std::stack<LARGE_INTEGER> m_stack;

    class CPerfTimer
    {

    public:
        static void Start()
        {
            LARGE_INTEGER startTime;
            QueryPerformanceCounter(&startTime);
            m_stack.push(startTime);
        }

        static float Stop()
        {
            LARGE_INTEGER endTime;
            QueryPerformanceCounter(&endTime);
        
            LARGE_INTEGER timerFrequency;
            QueryPerformanceFrequency(&timerFrequency);

            LARGE_INTEGER diffTime;
            LARGE_INTEGER startTime = m_stack.top();
            m_stack.pop();
            diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;
        
            float timeSpend = (float)diffTime.QuadPart * 1000.0f / (float)timerFrequency.QuadPart;

            return timeSpend;
        }

    };

};

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// RDTSC MACROS //////////////////////////
///////////////////////////////////////////////////////////////////////////////

// source: http://www.codeproject.com/Articles/288821/Performance-counters-with-inline-assembly-macros

//// usage:

//  unsigned __int64 start = 0, stop = 0, overhead = 0, perf = 0;
//  overhead = CalcOverhead(start,stop);	//get the overhead
//  
//  StartPerfCounter(start);
//  // code to test here...
//  StopPerfCounter(stop);
//  
//  perf = CalcPerf(start,stop,overhead);
//  printf("code costs %I64u cpu cycles \n", perf);

#define StartPerfCounter(__start)           \
    __asm pusha                             \
    __asm xor eax, eax                      \
    __asm cpuid                             \
    __asm rdtsc                             \
    __asm mov dword ptr [__start + 0], eax	\
    __asm mov dword ptr [__start + 4], edx


#define StopPerfCounter(__stop)             \
    __asm rdtsc                             \
    __asm mov dword ptr [__stop + 0], eax   \
    __asm mov dword ptr [__stop + 4], edx   \
    __asm popa


static __forceinline unsigned __int64 CalcOverhead(unsigned __int64 __start, unsigned __int64 __stop)
{
    StartPerfCounter(__start);
    StopPerfCounter(__stop);
    return __stop - __start;
}

static __forceinline unsigned __int64 CalcPerf(unsigned __int64 __start, unsigned __int64 __stop, __int64 __overhead)
{
    return __stop - __start - __overhead;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////