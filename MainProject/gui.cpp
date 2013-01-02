#include "stdafx.h"
#include "gui.h"



struct Vector3
{
    float v[3];
    static Vector3 ZERO;
};

Vector3 Vector3::ZERO = { 0, 0, 0 };

//struct Point { float X, Y, Z; };
//Point g_Positions[1] = { {0.0f, 0.5f, 0.5f} };

static TwStructMember vector3Members[] = { 
    { "X", TW_TYPE_FLOAT, offsetof(XMFLOAT3, x), " Min=-10000 Max=10000 Step=10 " },
    { "Y", TW_TYPE_FLOAT, offsetof(XMFLOAT3, y), " Min=-10000 Max=10000 Step=10 " }, 
    { "Z", TW_TYPE_FLOAT, offsetof(XMFLOAT3, z), " Min=-10000 Max=10000 Step=10 " }, 
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
//  Callback function called when the 'AutoRotate' variable value of the tweak bar has changed
void TW_CALL SetAutoRotateCB(const void *value, void *clientData)
{
    SubWindowData *win;
    
    win = (SubWindowData *)clientData;
    win->AutoRotate = *(const int *)value; // copy value to win->AutoRotate

    if (win->AutoRotate != 0) 
    {
        // init rotation
        win->RotateTime = GetTimeMs();
        win->RotateStart[0] = win->Rotation[0];
        win->RotateStart[1] = win->Rotation[1];
        win->RotateStart[2] = win->Rotation[2];
        win->RotateStart[3] = win->Rotation[3];
    }

    // make Rotation variable read-only or read-write
    TwSetCurrentWindow(win->WinID);
    TwSetParam(win->Bar, "ObjRotation", "readonly", TW_PARAM_INT32, 1, &win->AutoRotate);
}


//  Callback function called by the tweak bar to get the 'AutoRotate' value
void TW_CALL GetAutoRotateCB(void *value, void *clientData)
{
    SubWindowData *win;
    
    win = (SubWindowData *)clientData;
    *(int *)value = win->AutoRotate; // copy win->AutoRotate to value
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TW_CALL OnLightDirChanged(void *value, void *clientData)
{

}



CGui::CGui()
{

}

CGui::~CGui()
{
}    

bool CGui::Initialize(uint width, uint height)
{
    if (!TwInit(TW_DIRECT3D11, GetDX11Device()))
        assert(false);

    TwBar* bar = TwNewBar("Main");
    int barSize[2] = {width, height};
    int asdfsadf[1] = {260};
    TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
    TwSetParam(bar, NULL, "valueswidth", TW_PARAM_INT32, 1, asdfsadf);
    
    static XMFLOAT3 asdf;
    asdf.x = asdf.y = asdf.z = 0.0f;


    TwType vector3Type = TwDefineStruct("VECTOR3", vector3Members, 3, sizeof(XMFLOAT3), NULL, NULL);

    static float g_BackgroundColor[] = {0, 0, 0.5f, 1}; 

    TwAddVarRW(bar, "Sun color", TW_TYPE_COLOR4F, &g_BackgroundColor, "colormode=rgb");

    TwAddVarRW(bar, "Light direction", TW_TYPE_DIR3F, &GetGlobalState().GetSun().Direction, "opened=true axisz=-z showval=false");

    TwAddVarRW(bar, "Scene radius", TW_TYPE_FLOAT, &GetGlobalState().GetSceneBounds().Radius, "min=100 max=4000 step=5.0 keyincr=PGUP keydecr=PGDOWN");
   
    TwAddVarRW(bar, "Scene center", vector3Type, &GetGlobalState().GetSceneBounds().Center, "");


    // callback example
    // TwAddVarCB(bar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, win, " label='Auto-rotate' key=space help='Toggle auto-rotate mode.' ");

    return true;
}

void CGui::Draw()
{
    TwDraw();
}