#include "stdafx.h"
#include "debug_logger.h"



namespace DebugLogger
{
    Event g_events[BUFFER_SIZE];
    LONG g_pos = -1;
}


namespace BasicLogger
{

Log::Log()
{
}

Log& Log::Get()
{
    static Log log;
    return( log );
}

bool Log::Open()
{
    std::wstring filename = /*fs.GetLogFolder() +*/ L"\\Log.txt";
    AppLog.open(filename.c_str() );
    
    Write( L"Log file opened." );
    
    return( true );
}

bool Log::Write(const wchar_t *cTextString )
{
    AppLog << cTextString << "\n";
#if _DEBUG
    ::OutputDebugStringW( cTextString );
    ::OutputDebugStringW( L"\n" );
#endif
    return( true );
}

bool Log::Write(std::wstring& TextString )
{
    Log::Write( TextString.c_str() );
    AppLog.flush();
    return( true );
}

bool Log::Close()
{
    Write( L"Log file closed." );
    AppLog.close();
    return( true );
}

bool Log::WriteSeparater()
{
    Write( L"------------------------------------------------------------" );
    return( true );
}




}