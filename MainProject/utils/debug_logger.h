#pragma once

#include <windows.h>
#include <intrin.h>
 
// thread safe lightweight in-memory logger by Jeff Preshing (http://preshing.com) 

namespace DebugLogger
{
    struct Event
    {
        DWORD           tid;    // thread ID
        const char*     msg;    // message string
        DWORD           param;  // user parameter
    };
 
    static const int BUFFER_SIZE = 65536;   // must be a power of 2
    extern Event g_events[BUFFER_SIZE];
    extern LONG  g_pos;
 
    inline void Log(const char* msg, DWORD param)
    {
        LONG index = _InterlockedIncrement(&g_pos);
        Event* e = g_events + (index & (BUFFER_SIZE - 1));  // wrap to buffer size
        e->tid = ((DWORD*) __readfsdword(24))[9];           // get thread ID
        e->msg = msg;
        e->param = param;
    }
}

//  simple file logger ( +vs console output in debug)
// TODO: extend it

namespace BasicLogger
{
    class Log 
    {
        protected:
            Log();

            std::wofstream  AppLog;

        public:
            static Log&  Get();

            bool         Open();
            bool         Close();

            bool         Write(const wchar_t *TextString);
            bool         Write(std::wstring& TextString);
            bool         WriteSeparater();
    };
}

#define LOG_IN_MEM(m, p) DebugLogger::Log(m, p);
#define LOG(m)           BasicLogger::Log::Get().Write(m);
