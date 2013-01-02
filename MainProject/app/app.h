#pragma once

#include "stdafx.h"
#include "physics\physics.h"
#include "utils\spsc_queue.h"




static HANDLE                   g_closeHandlerRawWnd;
static DWORD                    g_rawThreadId;

static HINSTANCE                g_hInstance;



class CApp
{
    POINT               m_size;
    POINT               m_pos;

    int                 m_screenResolutionX;
    int                 m_screenResolutionY;

    bool                InitializeWindow(HINSTANCE hInstance, int ShowWnd, bool windowed);



    LRESULT CALLBACK    MessageHandler(HWND, UINT, WPARAM, LPARAM);
    void                ProcessFrame();


    CPhysics            m_physics;


public:
    CApp();
    ~CApp();
    HWND                m_hWnd;

    CTimer              m_mainTimer;

    bool                Initialize(HINSTANCE hInstance, uint width, uint height, uint posX, uint posY);


    static              int mouseX;
    static              int mouseY;

    static              SPSCQueue<InputPacket, 128>       g_rawInputMsgsQUeue;

    void                LimitThreadAffinityToCurrentProc();

    void                Run();
    
    void                SetUserGameLoop(void (*gameLoop)(float));

    std::wstring        GetWindowsVersion();
    long                GetTotalMemory();

    void                SetCursorVisible(bool visible);
    int                 GetAppCursorPos(POINT &point);
    int                 GetCursorPos(POINT &point);

    void                SetAppName(const char *name);

    int                 GetWidth()    const { return m_size.x; }
    int                 GetHeight()   const { return m_size.y; }
    int                 GetPosX()     const { return m_pos.x; }
    int                 GetPosY()     const { return m_pos.y; }

    const POINT&        GetSize()     const { return m_size; };
    const POINT&        GetPosition() const { return m_pos;  };

    void                OnWmSize(LPARAM lParam);
    void                OnWmMove(LPARAM lParam);

    void                OnFocusGain();
    void                OnFocusLost();
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
