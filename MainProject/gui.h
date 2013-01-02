#pragma once

#include <AntTweakBar.h>



// AntTweakGui library helper class

class CGui
{


public:
    CGui();
    ~CGui();

    bool Initialize(uint width, uint height);
    void Draw();

};