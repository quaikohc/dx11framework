#pragma once

#include "../dx11renderer/material.h"
#include "../dx11renderer/mesh.h"
#include "../globals.h"
#include "../dx11renderer/shader.h"
#include "../dx11renderer/effects/effectBase.h"
#include "../utils/math/math.h"

class Aligned2
{
public:
    Aligned2(){};
    ~Aligned2(){};

    void *operator new(unsigned int size, int alignment) /// size is in bytes, alignment must be power of two
    {
        return _aligned_malloc(size, alignment);
    }

    void operator delete(void* p)
    {
        _aligned_free(p);
    }
};


class CModel: public Aligned2
{
    
public:
    CModel();
    ~CModel();

    bool Initialize(EEffectType effectType);

    CMesh*                  m_mesh;

    CMaterial*              m_material;
    ID3D11InputLayout*      m_inputLayout;
    CShader*                m_shader;

    CEffect*                m_effect;
    CTransform              m_transformation;

    float texScale;
};