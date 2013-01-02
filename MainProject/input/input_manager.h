#pragma once

#include "mouse.h"
#include "keyboard.h"

#define USE_MULTITHREDED_INPUT

enum EKeyState
{
    Up,
    Down
};


struct MousePacket
{
    int     x;
    int     y;
    ushort  buttonFlags;
    short   wheel;
};

struct KeyboardPacket
{
    ushort  key;
    ushort  keyFlags;
};

struct InputPacket
{
    int     type;
    union 
    {
        MousePacket     mouse;
        KeyboardPacket  keyboard;
    };
};

static const uint m_maxBufferSize = 0x28;

class CInputManager
{
   
    BYTE                        inputBuff[m_maxBufferSize];

    CRawKeyboardInput*          m_keyboard;
    CRawMouseInput*             m_mouse;


public:
    CInputManager();
    ~CInputManager();


    bool                        Initialize();
    void                        HandleRawInput(LPARAM lParam);
    bool                        HandleWindowInputMsg(UINT msg, WPARAM wParam, LPARAM lParam);
    void                        UpdateInputState();

    void                        Update();

    const CRawKeyboardInput&    GetKeyboard() const {return *m_keyboard;}
    const CRawMouseInput&       GetMouse() const {return *m_mouse;}    

};




/*

private string GetDeviceDesc()
{
string deviceDesc = String.Empty;

try
{
string deviceName = GetDeviceName(hDevice);

if (deviceName != String.Empty)
{
deviceName = deviceName.Remove(deviceName.LastIndexOf('#'));
deviceName = deviceName.Substring(deviceName.LastIndexOf('\\') + 1);
deviceName = deviceName.Replace('#', '\\');
deviceName = "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\" + deviceName;
deviceName = (string)Registry.GetValue(deviceName, "DeviceDesc", String.Empty);
deviceDesc = deviceName.Substring(deviceName.LastIndexOf(';') + 1);
}
}
catch
{
#if DEBUG
throw;
#endif // DEBUG
}

return deviceDesc; // Will never return a null reference.
}


// Gets the hardware ID.
private static unsafe string GetDeviceName(IntPtr hDevice)
{
uint charCount = 0;

uint res = Win32.GetRawInputDeviceInfo(hDevice,
Win32.RIDI_DEVICENAME, IntPtr.Zero, ref charCount);

#if DEBUG
if (res != 0)
throw new Win32Exception(Marshal.GetLastWin32Error());
#endif // DEBUG

string deviceName = String.Empty;

if (charCount > 0)
{
char* pName = stackalloc char[(int)charCount + 1];
pName[0] = '\0';

res = Win32.GetRawInputDeviceInfo(hDevice,
Win32.RIDI_DEVICENAME, (IntPtr)pName, ref charCount);

#if DEBUG
if (res != charCount)
throw new Win32Exception(Marshal.GetLastWin32Error());
#endif // DEBUG

deviceName = new string(pName);
}

return deviceName; // Will never return a null reference.
}





















































*/