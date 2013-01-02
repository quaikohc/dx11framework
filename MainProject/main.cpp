#include "stdafx.h"

#include "dx11renderer/dx11device.h"
#include "dx11renderer/dx11renderer.h"
#include "scene/scene.h"
#include "utils/timer.h"
#include "dx11renderer/font_renderer_gdi.h"

//#include "utils/debug_logger.h"

////////////////////////////////////////////////
////////////////////// TODO list ///////////////

// work on renderer:
//      - wrapper/helper class for texture, constant buffers and render target (+depthstencil)
//
//      REFACTOR/IMPROVE/OPTIMIZE font class
//      DEBUG HUD 
//      FINISH INPUT SYSTEM
//
//      WRITE GOOD TIMER CLASS  !!!
//      REFACTOR/IMPROVE/OPTIMIZE camera class !!!
//      BETTER PERFORMANCE TIMER

//      RESOURCE MANAGEMENT
//      ERROR HANDLING
//      SCENE MANAGEMENT & RENDER QUEUE

////////////////////////////////////////////////
////////////////////////////////////////////////



void MyGameLoop(float elapsedTime);
void InitializeAntTweakBarGui();



int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    UNUSED(nShowCmd);
    UNUSED(lpCmdLine);
    UNUSED(hPrevInstance);
    UNUSED(hInstance);

    // #if defined(DEBUG) | defined(_DEBUG)
    // _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    // #endif

    Globals::Get().app.Initialize(hInstance, 1600, 900, 100, 100);
    Globals::Get().cpuUsage.Initialize();

    Globals::Get().device.Initialize();

    Globals::Get().resourceManager.Initialize();

    Globals::Get().renderer.InitScene(&Globals::Get().device);
    Globals::Get().scene.InitializeScene();
    Globals::Get().app.SetUserGameLoop(MyGameLoop);

    Globals::Get().gui.Initialize(400, 300);

    Globals::Get().app.Run();

    return 0;
}

void MyGameLoop(float elapsedTime)
{
    const KeyboardState& currState = Globals::Get().inputManager.GetKeyboard().GetCurrentState();
    //const KeyboardState& prevState = Globals::Get().inputManager.GetKeyboard().GetPreviousState();

    const SMouseState& currMouseState = Globals::Get().inputManager.GetMouse().GetCurrentState();
    //const SMouseState& prevMouseState = Globals::Get().inputManager.GetMouse().GetCurrentState();
   // Sleep(20);
    float camSpeed = 65.0f;


    if(currMouseState.relX)
        GetCamera().RotateY(1.0f * currMouseState.relX /2.0f * elapsedTime);

    if(currMouseState.relY)
        GetCamera().Pitch(1.0f * currMouseState.relY / 2.0f * elapsedTime);

    if(currState.IsKeyDown(Key::W))
    {
        GetCamera().Walk(1.0f * camSpeed * elapsedTime);
    }
    if(currState.IsKeyDown(Key::S))
    {
        GetCamera().Walk(-1.0f  * camSpeed * elapsedTime);
    }
    if(currState.IsKeyDown(Key::D))
    {
        GetCamera().Strafe(1.0f * camSpeed * elapsedTime);
    }
    if(currState.IsKeyDown(Key::A))
    {
        GetCamera().Strafe(-1.0f * camSpeed * elapsedTime);
    }
     if(currState.IsKeyDown(Key::Z))
    {
        GetCamera().RotateY(1.0f * camSpeed / 20.0f * elapsedTime);
    }
     if(currState.IsKeyDown(Key::X))
    {
        GetCamera().RotateY(-1.0f * camSpeed / 20.0f * elapsedTime);
    }
     if(currState.IsKeyDown(Key::E))
    {
        GetCamera().Pitch(1.0f * camSpeed / 20.0f * elapsedTime);
    }
    if(currState.IsKeyDown(Key::Q))
    {
        GetCamera().Pitch(-1.0f * camSpeed / 20.0f * elapsedTime);
    }

      
    // if(currState.IsKeyDown(Key::A) && prevState.IsKeyUp(Key::A) )
    // if(currMouseState.leftButton == Pressed)


    //  wchar_t tmpBuf[100];
    //  int offset = wsprintf(tmpBuf, L"x: %d y: %d", currMouseState.absX,  currMouseState.absY);
    //    Globals::Get().renderer.SetDebugText(tmpBuf);

}