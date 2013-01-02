#include "stdafx.h"
#include "keyboard.h"
#include "input_manager.h"




CRawKeyboardInput::CRawKeyboardInput()
{
    memset(&m_currState, false, sizeof(m_currState));
    memset(&m_prevState, false, sizeof(m_prevState));
}

CRawKeyboardInput::~CRawKeyboardInput()
{

}

void CRawKeyboardInput::Update(const KeyboardPacket& raw)
{
    if(raw.keyFlags == RI_KEY_MAKE) // key down
    {
        m_currState.keys[raw.key] = true;
        //OutputDebugStringA("down:  "); OutputDebugStringA(std::to_string(raw.key).c_str()); OutputDebugStringA("\n");
    }
    else if(raw.keyFlags  & RI_KEY_BREAK) // key up
    {
        m_currState.keys[raw.key] = false;
        //OutputDebugStringA("up:  "); OutputDebugStringA(std::to_string(raw.key).c_str()); OutputDebugStringA("\n");
    }
    else
    {
        assert(false);
    }
}

void CRawKeyboardInput::Update(const RAWINPUT* raw)
{

    // if (raw->data.keyboard.Message == WM_KEYDOWN || raw->data.keyboard.Message == WM_SYSKEYDOWN)
   /* 
    USHORT key = raw->data.keyboard.VKey;

    if(raw->data.keyboard.Flags == RI_KEY_MAKE) // key down
    {
        m_currState.keys[key] = true;
    }
    else if(raw->data.keyboard.Flags  & RI_KEY_BREAK) // key up
    {
        m_currState.keys[key] = false;
    }
    */


}