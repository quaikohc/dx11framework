#include "stdafx.h"
#include "mouse.h"
#include "input_manager.h"

CRawMouseInput::CRawMouseInput(int x, int y)
{
    m_currState.absX        = 0;
    m_currState.absY        = 0;
    m_currState.wheel       = 0;
    m_currState.leftButton  = Released;
    m_currState.midButton   = Released;
    m_currState.rightButton = Released;
}

CRawMouseInput::~CRawMouseInput()
{   

}

void CRawMouseInput::Update(const MousePacket& packet)
{
    m_currState.absX += packet.x;
    m_currState.absY += packet.y;

    m_currState.relX = packet.x;
    m_currState.relY = packet.y;

    if (packet.buttonFlags & RI_MOUSE_BUTTON_1_DOWN)
        m_currState.leftButton = Pressed;
    else if(packet.buttonFlags & RI_MOUSE_BUTTON_1_UP)
        m_currState.leftButton = Released;

    if (packet.buttonFlags & RI_MOUSE_BUTTON_3_DOWN)
        m_currState.midButton = Pressed;
    else if(packet.buttonFlags & RI_MOUSE_BUTTON_3_UP)
        m_currState.midButton = Released;

    if (packet.buttonFlags & RI_MOUSE_BUTTON_2_DOWN)
        m_currState.rightButton = Pressed;
    else if(packet.buttonFlags & RI_MOUSE_BUTTON_2_UP)
        m_currState.rightButton = Released;

    if (packet.buttonFlags & RI_MOUSE_WHEEL)
        m_currState.wheel += packet.wheel;

}

void CRawMouseInput::Update(const RAWINPUT* raw)
{
//    wchar_t tmpBuf[100];
 

 //   m_currState.relX =  ( (RAWINPUT*)(& ( (BYTE*) raw)[8]) )->data.mouse.lLastX;
//    m_currState.relY =  ( (RAWINPUT*)(& ( (BYTE*) raw)[8]) )->data.mouse.lLastY;

//    m_currState.absX += ( (RAWINPUT*)(& ( (BYTE*) raw)[8]) )->data.mouse.lLastX;
 //    m_currState.absY += ( (RAWINPUT*)(& ( (BYTE*) raw)[8]) )->data.mouse.lLastY;

    // m_currState.relX = raw->data.mouse.lLastX;
    // m_currState.relY = raw->data.mouse.lLastY;
    //
   //  m_currState.absX += raw->data.mouse.lLastX ;
   //  m_currState.absY += raw->data.mouse.lLastY ;
    //
    /*if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN)
        m_currState.leftButton = Pressed;
    else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP )
        m_currState.leftButton = Released;

    if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN)
        m_currState.midButton = Pressed;
    else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP)
        m_currState.midButton = Released;

    if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN  )
        m_currState.rightButton = Pressed;
    else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP  )
        m_currState.rightButton = Released;
        */

//    if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL != 0 );

}