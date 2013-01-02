#pragma once
#include "../shader.h"
#include "../utils/lruCache.h"

struct SParticleCBufferWithoutSO
{
    XMMATRIX      worldViewProj;
    XMFLOAT3      eyePos;
    float         pad;
};

struct SParticleDesc
{
    uint                        m_particlesCount;
    bool                        m_animateParticles;
    float                       m_emitRate;
    float                       m_maxSize;
    float                       m_minSize;
    float                       m_lifeSpan;
    float                       m_velocity;
};

class ParticleSystem
{
    CShader*                    m_shader;

    void                        InitializeShaders();
    void                        InitializeTextures();
    void                        InitializeVB();

    void                        EmitParticle(SVertexBasicParticle* pParticle);

    ID3D11Buffer*               m_particleConstBuffer;

    ID3D11Buffer*               m_vb;

    SVertexBasicParticle*       m_particlesData;

    ID3D11ShaderResourceView*   m_particleTexture;
    ID3D11ShaderResourceView*   m_colorGradientTexture;

    SParticleDesc               m_particleDesc;

public:
    ParticleSystem();
    ~ParticleSystem();

    void                        Initialize();

    void                        Update(double fTime, float fTimeDelta);
    void                        Draw();

    CLRUCache<int, float>       m_cache;
};
