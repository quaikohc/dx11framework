#pragma once

enum Key
{
    D0 = 0x30,
    D1 = 0x31,
    D2 = 0x32,
    D3 = 0x33,
    D4 = 0x34,
    D5 = 0x35,
    D6 = 0x36,
    D7 = 0x37,
    D8 = 0x38,
    D9 = 0x39,
             
    A = 0x041,
    B = 0x042,
    C = 0x043,
    D = 0x044,
    E = 0x045,
    F = 0x046,
    G = 0x047,
    H = 0x048, 
    I = 0x049,
    J = 0x04A,
    K = 0x04B,
    L = 0x04C,
    M = 0x04D,
    N = 0x04E,
    O = 0x04F,
    P = 0x050,
    Q = 0x051,
    R = 0x052,
    S = 0x053,
    T = 0x054,
    U = 0x055,
    V = 0x056,
    W = 0x057,
    X = 0x058,
    Y = 0x059,
    Z = 0x05A,

    F1  = 0x70,
    F2  = 0x71,
    F3  = 0x72,
    F4  = 0x73,
    F5  = 0x74,
    F6  = 0x75,
    F7  = 0x76,
    F8  = 0x77,
    F9  = 0x78,
    F10 = 0x79,
    F11 = 0x7A,
    F12 = 0x7B,

    Space    = 0x20,

    End      = 0x23,
    Home     = 0x24,
    Insert   = 0x2D,
    Delete   = 0x2E,

    Left     = 0x25,
    //Up       = 0x26,
    Right    = 0x27,
  //  Down     = 0x28,

    Multiply  = 0x6A,
    Add       = 0x6B,
    Subtract  = 0x6D,
    Divide    = 0x6F,

    RShift     = 0xA1,
    LControl   = 0xA2,
    LShift     = 0xA0,
    RControl   = 0xA3,


    EKeysCount = 0xFF
};

/*

struct Key
{
	enum List
	{
		RShift = 0,
		Subtract
	};
};

Key::List    var;

/////////

namespace GameFlowType
{
    enum Enum
    {
        Invalid,
    }
}
GameFlowType::Enum var;

*/
struct KeyboardState
{
    // TODO:
    // constructor
    // copy constructor 
    // operator=

    bool keys[EKeysCount];

    bool IsKeyDown(Key key)const { return  keys[key]; }
    bool IsKeyUp(Key key)  const { return !keys[key]; }

    USHORT GetPressedKeysCount() { return 0; }
};

struct KeyboardPacket;

class CRawKeyboardInput
{
    // TODO:
    // add events

public:
    CRawKeyboardInput();
    ~CRawKeyboardInput();

    void Update(const RAWINPUT* raw);
    void Update(const KeyboardPacket& raw);

    KeyboardState m_currState;
    KeyboardState m_prevState;

    const KeyboardState& GetCurrentState()  const { return m_currState; }
    const KeyboardState& GetPreviousState() const { return m_prevState; }
};