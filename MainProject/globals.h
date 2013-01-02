#pragma once

#include "utils/timer.h"
#include "dx11renderer/dx11device.h"
#include "dx11renderer/dx11renderer.h"
#include "scene/scene.h"
#include "input/input_manager.h"

#include "scene/visibility_system.h"
#include "physics/physics.h"
#include "app/app.h"

#include "utils/file_system.h"
#include "dx11renderer/resourceManager.h"
#include "perfUtils/cpuUsage.h"
#include "dx11renderer/lights/light.h"
#include "gui.h"

class Aligned
{
public:
    Aligned(){};
    ~Aligned(){};

    void *operator new(unsigned int size, int alignment) /// size is in bytes, alignment must be power of two
    {
        return _aligned_malloc(size, alignment);
    }

    void operator delete(void* p)
    {
        _aligned_free(p);
    }
};


class CGlobalState
{    
    DirectionalLight g_sunLight;
    BoundingSphere   g_sceneBounds;
public:

    DirectionalLight&   GetSun()         { return g_sunLight;    }
    BoundingSphere&     GetSceneBounds() { return g_sceneBounds; }
};

// TODO: get rid of the singleton and switch to regurlar globals

class Globals 
{

public:
    static Globals& Get() 
    {
        static Globals theSingleton;
        return theSingleton;
    }

    CDx11Device         device;

    CDX11Renderer       renderer;
    CScene              scene;
    CInputManager       inputManager;

    CPhysics            physics;

    CApp                app;
    CFileSystem         fileSystem;

    CResourceManager    resourceManager;

    CpuUsage            cpuUsage;
    CGlobalState        globalState;
    CGui                gui;

private:
    Globals(){}; 
    Globals(Globals const&){};
    Globals& operator=(Globals const&){}; 
    ~Globals(){};

};

#define GetDevice()         Globals::Get().device
#define GetDX11Device()     Globals::Get().device.m_device
#define GetDX11Context()    Globals::Get().device.m_context

#define GetRenderer()       Globals::Get().renderer
#define GetApp()            Globals::Get().app
#define GetCamera()         Globals::Get().renderer.m_camera
#define GetGlobalState()    Globals::Get().globalState
#define GetGui()            Globals::Get().gui


///////////////////////////////////////////////////////////////////////


