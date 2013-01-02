#include "stdafx.h"
#include "input_manager.h"



CInputManager::CInputManager()
{
    m_keyboard = nullptr;
    m_mouse    = nullptr;

}

CInputManager::~CInputManager()
{
    if(m_keyboard)
        delete m_keyboard;
    if(m_mouse)
        delete m_mouse;
}

// type of the device
enum EUsagePage
{
    EGenericDesktop = 0,
    ESimulation,
    EVR,
    ESport,
    EGame,
    EGenericDevice,
    EKeyboard,
    ELeds,
    EButton,
    EUsagePageCount
};

enum EUsage
{
    EPointer = 0,
    EMouse,
    EReserved,
    EJoystick,
    EGamePad,
   // EKeyboard,
    EKeypad,
    EMultiAxisController,
    ETabletPcControls
};

/*
RIDEV_APPKEYS   0x00000400         If set, the application command keys are handled. RIDEV_APPKEYS can be specified only if RIDEV_NOLEGACY is specified for a keyboard device.
RIDEV_CAPTUREMOUSE 0x00000200   If set, the mouse button click does not activate the other window.
RIDEV_DEVNOTIFY      0x00002000      If set, this enables the caller to receive WM_INPUT_DEVICE_CHANGE notifications for device arrival and device removal.
RIDEV_EXCLUDE   0x00000010      Windows XP:  This flag is not supported until Windows Vista
RIDEV_EXINPUTSINK  0x00001000   If set, this specifies the top level collections to exclude when reading a complete usage page. This flag only affects a TLC whose usage page is already specified with RIDEV_PAGEONLY.
0x00000100  RIDEV_INPUTSINK If set, this enables the caller to receive input in the background only if the foreground application does not process it. In other words, if the foreground application is not registered for raw input, then the background application that is registered will receive the input.
0x00000200   RIDEV_NOHOTKEYS    If set, this enables the caller to receive the input even when the caller is not in the foreground. Note that hwndTarget must be specified.
0x00000030    RIDEV_NOLEGACY If set, the application-defined keyboard device hotkeys are not handled. However, the system hotkeys; for example, ALT+TAB and CTRL+ALT+DEL, are still handled. By default, all keyboard hotkeys are handled. RIDEV_NOHOTKEYS can be specified even if RIDEV_NOLEGACY is not specified and hwndTarget is NULL.
0x00000020  RIDEV_PAGEONLY  If set, this prevents any devices specified by usUsagePage or usUsage from generating legacy messages. This is only for the mouse and keyboard. See Remarks.
*/
//0x00000001      RIDEV_REMOVE  If set, this specifies all devices whose top level collection is from the specified usUsagePage. Note that usUsage must be zero. To exclude a particular top level collection, use RIDEV_EXCLUDE.
// If set, this removes the top level collection from the inclusion list. This tells the operating system to stop reading from a device which matches the top level collection.

enum EFlags
{


};


bool CInputManager::Initialize()
{
   /* RAWINPUTDEVICE Rid[2] ;

    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage     = 0x02;
    Rid[0].dwFlags     = 0;     // (don't capture mouse)
    Rid[0].hwndTarget  = Globals::Get().app.m_hWnd;

    Rid[1].usUsagePage = 0x01;
    Rid[1].usUsage     = 0x06;  // keyboard
    Rid[1].dwFlags     = 0;     
    Rid[1].hwndTarget  = Globals::Get().app.m_hWnd;

    if(!RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])))
        assert(false);
    
    SetCursorPos(0, 0);
       */
    POINT mousePnt;
    GetCursorPos(&mousePnt);
   // ScreenToClient( hwnd, &mousePnt );

    m_keyboard = new CRawKeyboardInput();
    m_mouse    = new CRawMouseInput(mousePnt.x, mousePnt.y);

    return true;
}

void CInputManager::UpdateInputState()
{
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient( Globals::Get().app.m_hWnd, &pos);
          
    m_mouse->m_currState.absX = pos.x;
    m_mouse->m_currState.absY = pos.y;
    m_mouse->m_currState.relX = pos.x - m_mouse->m_prevState.absX;
    m_mouse->m_currState.relY = pos.y - m_mouse->m_prevState.absY;

    if( (GetKeyState(VK_LBUTTON) & 0x8000) > 0)
        m_mouse->m_currState.leftButton = Pressed;
    else
        m_mouse->m_currState.leftButton = Released;

    if( (GetKeyState(VK_RBUTTON) & 0x8000) > 0)
        m_mouse->m_currState.rightButton = Pressed;
    else
        m_mouse->m_currState.rightButton = Released;

    if( (GetKeyState(VK_MBUTTON) & 0x8000) > 0)
        m_mouse->m_currState.midButton = Pressed;
    else
        m_mouse->m_currState.midButton = Released;

   // RECT clientRect;
   // GetClientRect(Globals::Get().app->m_hWnd, &clientRect);
   // m_mouse->m_currState.isOverWnd = (pos.x >= 0 && pos.x < clientRect.right && pos.y >= 0 && pos.y < clientRect.bottom);

}






    //case WM_INPUT: {
    //		static std::vector<BYTE> rawInputData;
    //		UINT dwSize;
    //		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    //		rawInputData.resize(dwSize);
    //		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawInputData[0], &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
    //			RAWINPUT* raw = (RAWINPUT*)&rawInputData[0];
    //			if (raw->header.dwType == RIM_TYPEMOUSE)
    //				rawMouse(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
    //		}
    //		return 0;
    //	}


// standard windows input messages processing
bool CInputManager::HandleWindowInputMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);

    switch( msg )
    {
        case WM_MOUSEMOVE:
        {
            m_mouse->m_currState.absX = LOWORD(lParam); 
            m_mouse->m_currState.absY = HIWORD(lParam); 
         //   if (wParam & MK_LBUTTON) // (wParam & MK_RBUTTON) 
        }
        break;
        case WM_LBUTTONUP:
            m_mouse->m_currState.leftButton = Released;
        break;
        case  WM_LBUTTONDOWN:
            m_mouse->m_currState.leftButton = Pressed;
        break;
        case  WM_RBUTTONUP:
            m_mouse->m_currState.rightButton = Released;
        break;
        case  WM_RBUTTONDOWN:
            m_mouse->m_currState.rightButton = Pressed;
        break;
        case  WM_MBUTTONUP:
            m_mouse->m_currState.midButton = Released;
        break;
        case  WM_MBUTTONDOWN:
            m_mouse->m_currState.midButton = Pressed;
        break;
        case WM_KEYDOWN:
        {
        }
        break;
        default:
            return false;
    }
    return true;
}


void CInputManager::HandleRawInput(LPARAM lparam)
{
    // ifndef multithreded_input
  /*  UINT dwSize;

    GetRawInputData((HRAWINPUT) lparam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    assert(dwSize <= m_maxBufferSize);

    UINT readSize = GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &inputBuff, &dwSize, sizeof(RAWINPUTHEADER));
    assert(readSize == dwSize);

    RAWINPUT* raw = (RAWINPUT*)inputBuff;
    */

    // TEST
    /*if(CApp::g_rawInputMsgsQUeue.Size() > 1)
    {
        InputPacket inputPacket;
        InputPacket inputPacket2;

        CApp::g_rawInputMsgsQUeue.Pop(inputPacket);
        CApp::g_rawInputMsgsQUeue.Pop(inputPacket2);
        
        //        ushort key0 = inputPacket.keyboard.key;
        //ushort key1 = inputPacket.keyboard.key;

        //// OutputDebugStringA("up:  "); 
        //OutputDebugStringA(std::to_string(key0).c_str()); 
        //OutputDebugStringA("           ");
        //OutputDebugStringA(std::to_string(key1).c_str()); 
        //OutputDebugStringA("\n");



        float asdf = 34;
        asdf++;
    }
    */
    while(CApp::g_rawInputMsgsQUeue.Size() > 0)
    {
       // RAWINPUT raw;
        InputPacket inputPacket;
        
        CApp::g_rawInputMsgsQUeue.Pop(inputPacket);

        size_t asdf = sizeof(inputPacket);
        assert(asdf == 16);
        
        if(inputPacket.type == 1)
            m_mouse->Update(inputPacket.mouse);
        else if(inputPacket.type == 0)
            m_keyboard->Update(inputPacket.keyboard);
       
        //if (raw.header.dwType == RIM_TYPEKEYBOARD)
          //  m_keyboard->Update(&raw);
       // else if (raw.header.dwType == RIM_TYPEMOUSE)
        //    m_mouse->Update(&raw);
    }

    /*if (raw->header.dwType == RIM_TYPEKEYBOARD)
        m_keyboard->Update(raw);
    else if (raw->header.dwType == RIM_TYPEMOUSE)
        m_mouse->Update(raw);
        */
}

void CInputManager::Update()
{
    //m_keyboard->m_prevState = m_keyboard->m_currState;

  //  memset(&m_keyboard->m_currState, false, sizeof(m_keyboard->m_currState));
    
   // m_mouse->m_prevState = m_mouse->m_currState;
   
    static const SMouseState nullMouseState = {0};
    m_mouse->m_currState = nullMouseState;
    
}