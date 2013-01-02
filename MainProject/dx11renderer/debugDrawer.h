#pragma once

#include "shader.h"

struct SDebugCBuffer
{
    XMMATRIX    worldViewProj;
};


class CDebugDrawer
{
    CShader*                    m_shader;
    CShader*                    m_shader1D;

    ID3D11Buffer*               m_vb;
    ID3D11Buffer*               m_ib;
    ID3D11Buffer*           m_cBuffer;


public:
    CDebugDrawer();
    ~CDebugDrawer();


    void                        BuildGeometry();
    void                        Initialize();
    void                        DrawTexture(ID3D11ShaderResourceView* texture, uint slot = 0, float scale = 1.0f);
    void                        DrawTexture1D(ID3D11ShaderResourceView* texture, int slot);
};