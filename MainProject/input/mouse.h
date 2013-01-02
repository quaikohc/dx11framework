#pragma once

#include "stdafx.h"

enum EMouseButtonState
{
    Released,
    Pressed 
};

struct SMouseState
{
    int        relX;
    int        relY;

    int        absX;
    int        absY;

    uint        wheel;

    EMouseButtonState   leftButton;
    EMouseButtonState   rightButton;
    EMouseButtonState   midButton;

    bool        isOverWnd;
};



struct MousePacket;

class CRawMouseInput
{

public:
    CRawMouseInput(int x, int y);
    ~CRawMouseInput();

    SMouseState m_currState;
    SMouseState m_prevState;

    const SMouseState& GetCurrentState()  const { return m_currState; }
    const SMouseState& GetPreviousState() const { return m_prevState; }

    void Update(const RAWINPUT* raw);
    void Update(int x, int y);
    void Update(const MousePacket& packet);

};