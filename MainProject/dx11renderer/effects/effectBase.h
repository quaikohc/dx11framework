#pragma once
#include "../constBuffer.h"

struct test
{
};

class CEffect
{

protected:
    CModel* m_model;


public:
    CEffect(int cbufferStructSize);


    ID3D11Buffer* m_constBuffer;

    virtual void Update(){};
    virtual void Bind(){};
};


class CTexturedEffect : public CEffect
{
    struct perObject
    {
        XMMATRIX    world;
    };

public:
    CTexturedEffect(CModel* model) : CEffect(sizeof(perObject))
    {
        m_model = model;
    };

    ~CTexturedEffect()
    {
    };

    void Update() override;
    void Bind()   override;
};

struct TMPTESTSTRUCT
{
};

class CParallaxEffect : public CEffect
{
    struct perEffect
    {

    };

    struct perObject
    {
        XMMATRIX      gWorld;
        XMMATRIX      gWorldInvTranspose;
        XMMATRIX      gWorldViewProj;
        XMMATRIX      gTexTransform;
        XMMATRIX      gShadowTransform; 
        Material      gMaterial;
    };

    
   // CConstBuffer<TMPTESTSTRUCT> m_cBuffer;

public:
    CParallaxEffect(CModel* model) : CEffect(sizeof(perObject))
    {
        m_model = model;
    };

    ~CParallaxEffect()
    {
    };

    void Update() override;
    void Bind()   override;

};