#include "stdafx.h"
#include "app.h"
#include "game_loop_manager.h"

#include <AntTweakBar.h>
#include "../utils/basicReflection.h"

int CApp::mouseX = 0;
int CApp::mouseY = 0;

SPSCQueue<InputPacket, 128>       CApp::g_rawInputMsgsQUeue;

struct foo 
{
  int       m_varInt;
  short     m_varFloat;
  bool      m_varBool;
  long      m_varLong;

  RTTI(foo, MEMBER(m_varInt) MEMBER(m_varFloat) MEMBER(m_varBool) MEMBER(m_varLong))
};


CApp::CApp() //: m_userGameLoop(NULL)
{
    ////////////    reflection test ////////////


//    const char* var3Name = foo::members()[3].name;
//    size_t var0Offset = foo::members()[0].offset;
//    size_t var0Size  = foo::members()[0].type->size();







    ////////////   reflection test end  ////////////
}

CApp::~CApp()
{
}

bool CApp::Initialize(HINSTANCE hInstance,  uint width, uint height, uint posX, uint posY)
{ 
    m_size.x = width;
    m_size.y = height;
    m_pos.x  = posX;
    m_pos.y  = posY;

    m_screenResolutionX = GetSystemMetrics(SM_CXSCREEN);
    m_screenResolutionY = GetSystemMetrics(SM_CYSCREEN);

 //   LimitThreadAffinityToCurrentProc();

    if(!InitializeWindow(hInstance, true, true))
        return false;

    m_physics.Initialize();

    return true; 
}

bool CApp::InitializeWindow(HINSTANCE hInstance, int ShowWnd, bool windowed)
{
    UNUSED(windowed);

    WNDCLASSEX wc;

    wc.cbSize         = sizeof(WNDCLASSEX);
    wc.style          = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc    = WndProc;
    wc.cbClsExtra     = NULL;
    wc.cbWndExtra     = NULL;
    wc.hInstance      = hInstance;
    wc.hIcon          = NULL;
    wc.hCursor        = NULL;
    wc.hbrBackground  = NULL;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = L"Test app";
    wc.hIconSm        = NULL;

    if (!RegisterClassEx(&wc)) assert(false);

    m_hWnd = CreateWindowEx(
        NULL,
        L"Test app",
        L"HashFramework test application",
        WS_OVERLAPPEDWINDOW,
        m_pos.x, m_pos.y,
        m_size.x, m_size.y,
        NULL,
        NULL,
        hInstance,
        NULL
        );

    if (!m_hWnd) assert(false);

    ShowWindow(m_hWnd, ShowWnd);
    UpdateWindow(m_hWnd);

    Globals::Get().inputManager.Initialize();

    return true;
}


void CApp::Run()
{
    MSG msg = {0};

    while(true)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
            ProcessFrame();
    }

    return;
}

void CApp::SetUserGameLoop(void (*gameLoop)(float))
{ 
    g_userGameLoop = gameLoop; 
}

void CApp::ProcessFrame()
{
    m_mainTimer.Update();

    BasicGameLoop(m_mainTimer.GetDelta());
}

DWORD WINAPI RawThread(HANDLE hCloseEvent)
{
    HWND hDummy;
    UINT cbSize;
    MSG msg;

    hDummy = CreateWindowEx(0, L"button", 0, 0, 0, 0, 1, 1, 0, 0, g_hInstance, 0);

    RAWINPUTDEVICE Rid[2] ;

    Rid[0].usUsagePage = 0x01; 
    Rid[0].usUsage     = 0x02; // mouse
    Rid[0].dwFlags     = 0; 
    Rid[0].hwndTarget  = hDummy;

    Rid[1].usUsagePage = 0x01 ;
    Rid[1].usUsage     = 0x06 ; // keyboard
    Rid[1].dwFlags     = 0 ;     
    Rid[1].hwndTarget  = hDummy ;

    if( !RegisterRawInputDevices(Rid, 2, sizeof(Rid[0]) ) )
        assert(false);


    while(1)
    {
        DWORD result = MsgWaitForMultipleObjects(1, &hCloseEvent, 0, INFINITE, QS_RAWINPUT);

        if(result == WAIT_OBJECT_0 + 1)
        {
            RAWINPUT *pri;

            if(GetRawInputBuffer(0, &cbSize, sizeof(RAWINPUTHEADER)))
                break;

            cbSize *= 16;

            if(!(pri = (RAWINPUT*) malloc(cbSize)))
                break;

            while(1)
            {
                DWORD cri = GetRawInputBuffer(pri, &cbSize, sizeof(RAWINPUTHEADER));

                if(cri && cri != -1)
                {
                    RAWINPUT *p = pri;

                    for(; cri; cri--, p = NEXTRAWINPUTBLOCK(p))
                    { 
                        InputPacket inputPacket;

                        if(p->header.dwType == RIM_TYPEKEYBOARD)
                        {
                            inputPacket.type              = 0;
                            inputPacket.keyboard.key      = ( (RAWINPUT*)(& ( (BYTE*) p)[8]) )->data.keyboard.VKey;
                            inputPacket.keyboard.keyFlags = ( (RAWINPUT*)(& ( (BYTE*) p)[8]) )->data.keyboard.Flags;
                        }
                       else if(p->header.dwType ==  RIM_TYPEMOUSE)
                        { 
                            inputPacket.type              = 1;
                            inputPacket.mouse.x           = ( (RAWINPUT*)(& ( (BYTE*) p)[8]) )->data.mouse.lLastX;
                            inputPacket.mouse.y           = ( (RAWINPUT*)(& ( (BYTE*) p)[8]) )->data.mouse.lLastY;
                            inputPacket.mouse.buttonFlags = ( (RAWINPUT*)(& ( (BYTE*) p)[8]) )->data.mouse.usButtonFlags;
                            inputPacket.mouse.wheel       = ( (RAWINPUT*)(& ( (BYTE*) p)[8]) )->data.mouse.usButtonData;
                        }

                        CApp::g_rawInputMsgsQUeue.Push(inputPacket);
                    }
                }
                else
                    break;
            }
            while(PeekMessage(&msg, 0, WM_INPUT, WM_INPUT, PM_REMOVE)){};
            free(pri);
        }
        else
            break;
    }
    DestroyWindow(hDummy);

    return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //if(Globals::Get().inputManager.HandleWindowInputMsg(msg, wParam, lParam))  return true;

     if (TwEventWin(hwnd, msg, wParam, lParam))
        return 0; 

    switch( msg )
    {
        //case WM_INPUT:  // raw input msg
        //{
         //   Globals::Get().inputManager.HandleRawInput(lParam);
        //}
        case WM_SIZE:
        {
            Globals::Get().app.OnWmSize(lParam); 
        }
            return 0;
        case WM_MOVE:
            {
                Globals::Get().app.OnWmMove(lParam); 
            }
            return 0;
        case WM_ACTIVATEAPP:
            if (wParam)
                Globals::Get().app.OnFocusGain();
            else
                Globals::Get().app.OnFocusLost();

            return 0;
        case WM_CLOSE:
            {
                SetEvent(g_closeHandlerRawWnd);
                PostQuitMessage(0);
            }
            return 0;
        case WM_CREATE:
            {
                g_closeHandlerRawWnd = CreateEvent(0, 1, 0, 0);
                CreateThread(0, 0, &RawThread, (LPVOID) g_closeHandlerRawWnd, 0, &g_rawThreadId);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CApp::OnFocusGain()
{
    OutputDebugString(L"On focus gain");
}

void CApp::OnFocusLost()
{
    OutputDebugString(L"On focus loast");
}

void CApp::OnWmSize(LPARAM lParam)
{
    UNUSED(lParam);
    //m_size.x = HIWORD(lParam);
    //m_size.y  = LOWORD(lParam);
   // OutputDebugString(L"On wm size ");
}

void CApp::OnWmMove(LPARAM lParam)
{
    UNUSED(lParam);
   // OutputDebugString(L"On wm move ");
    //RECT desiredRect = { LOWORD( lParam ), HIWORD( lParam ), LOWORD( lParam ) + m_size.x, HIWORD( lParam ) + m_size.y };

    //AdjustWindowRectEx( &desiredRect, GetWindowLongPtr(m_hWnd, GWL_STYLE ), FALSE, GetWindowLongPtr(m_hWnd, GWL_EXSTYLE ) );

    //m_pos.x = desiredRect.left;
    //m_pos.y = desiredRect.top;
}

std::wstring CApp::GetWindowsVersion()
{
    OSVERSIONINFO info;
    OSVERSIONINFOEX ex;

    ZeroMemory( &info, sizeof( OSVERSIONINFO ) );
    ZeroMemory( &ex,   sizeof( OSVERSIONINFOEX ) );
    info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    ex.dwOSVersionInfoSize   = sizeof( OSVERSIONINFOEX );

    GetVersionEx( &info );
    GetVersionEx( ( OSVERSIONINFO * ) &ex );

    if( info.dwMajorVersion == 6 && info.dwMinorVersion == 0 && ex.wProductType == VER_NT_WORKSTATION )
        return L"(6.0) Windows Vista";

    if( info.dwMajorVersion == 6 && info.dwMinorVersion == 1 && ex.wProductType == VER_NT_WORKSTATION )
        return L"(6.1) Windows 7";

    return L"Windows";
}

long  CApp::GetTotalMemory()
{
    MEMORYSTATUS lpBuffer;
    lpBuffer.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus (&lpBuffer);

    return lpBuffer.dwTotalPhys/1024/1024;
}

void CApp::SetCursorVisible(bool visible)
{
    ShowCursor( visible );
}

int CApp::GetAppCursorPos(POINT &point)
{
    if ( !::GetCursorPos( &point ) )  
        return false;
    return ::ScreenToClient(m_hWnd, &point );
}

int CApp::GetCursorPos(POINT &point)
{
    return ::GetCursorPos(&point);
}

void CApp::SetAppName(const char *name)
{
    SetWindowTextA(m_hWnd, name);
}



/*

void WindowApplication::CenterCursor( void )
{
    POINT absPnt;
    POINT relPnt;
    POINT delta;
    ::GetCursorPos( &absPnt );
    ::GetCursorPos( &relPnt );
    ScreenToClient( m_hWnd, &relPnt );
    delta.x = absPnt.x - relPnt.x;
    delta.y = absPnt.y - relPnt.y;
    absPnt.x = delta.x + (m_appWidth / 2);
    absPnt.y = delta.y + (m_appHeight / 2);
    SetCursorPos( absPnt.x, absPnt.y );
}


*/


// taken from DXUT
void CApp::LimitThreadAffinityToCurrentProc()
{
    HANDLE hCurrentProcess = GetCurrentProcess();

    // Get the processor affinity mask for this process
    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;

    if( GetProcessAffinityMask( hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask ) != 0 && dwProcessAffinityMask )
    {
        // Find the lowest processor that our process is allows to run against
        DWORD_PTR dwAffinityMask = ( dwProcessAffinityMask & ( ( ~dwProcessAffinityMask ) + 1 ) );

        // Set this as the processor that our thread must always run against
        // This must be a subset of the process affinity mask
        HANDLE hCurrentThread = GetCurrentThread();
        if( INVALID_HANDLE_VALUE != hCurrentThread )
        {
            SetThreadAffinityMask( hCurrentThread, dwAffinityMask );
            CloseHandle( hCurrentThread );
        }
    }

    CloseHandle( hCurrentProcess );
}
