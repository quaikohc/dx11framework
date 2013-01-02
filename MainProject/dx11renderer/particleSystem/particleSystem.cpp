#include "stdafx.h"
#include "particleSystem.h"
#include "../../utils/DDSTextureLoader.h"
#include "../material.h"
#include "../statesAndSamplers.h"




ParticleSystem::ParticleSystem(): 
    m_shader(nullptr),
    m_particleConstBuffer(nullptr),
    m_vb(nullptr),
    m_particlesData(nullptr),
    m_particleTexture(nullptr),
    m_colorGradientTexture(nullptr)
{
    m_particleDesc.m_particlesCount   = 500;
    m_particleDesc.m_maxSize          = 5.25f;
    m_particleDesc.m_minSize          = 1.0f;
    m_particleDesc.m_animateParticles = true;
    m_particleDesc.m_emitRate         = 0.015f;
    m_particleDesc.m_lifeSpan         = 5.0f;
    m_particleDesc.m_velocity         = 3.0f;

    m_cache.Insert(334, 34);
}

ParticleSystem::~ParticleSystem()
{
    delete m_shader;

    if(m_colorGradientTexture)
        m_colorGradientTexture->Release();
    if(m_particleTexture)
        m_particleTexture->Release();

    delete[] m_particlesData;
}

void ParticleSystem::Update(double fTime, float fTimeDelta)
{
    static double fLastEmitTime = 0;
    static uint iLastParticleEmitted = 0;

    if( !m_particleDesc.m_animateParticles )
    {
        fLastEmitTime = fTime;
        return;
    }

    uint NumParticlesToEmit = (uint)( (fTime - fLastEmitTime) / m_particleDesc.m_emitRate );
    if( NumParticlesToEmit > 0 )
    {
        for( uint i = 0; i < NumParticlesToEmit; i++ )
        {
            EmitParticle( &m_particlesData[iLastParticleEmitted] );
            iLastParticleEmitted = (iLastParticleEmitted + 1) % m_particleDesc.m_particlesCount;
        }
        fLastEmitTime = fTime;
    }

    XMFLOAT3 vel;
    float lifeSq = 0;

    for(uint i = 0; i < m_particleDesc.m_particlesCount; i++)
    {
        if( m_particlesData[i].Life > -1 )
        {
            lifeSq = m_particlesData[i].Life * m_particlesData[i].Life;

            float factor = (1 - 0.5f * lifeSq);
            vel.x = m_particlesData[i].Vel.x * factor;
            vel.y = m_particlesData[i].Vel.y * factor;
            vel.z = m_particlesData[i].Vel.z * factor;

            vel.y += 0.5f;

            vel.x *= fTimeDelta;
            vel.y *= fTimeDelta;
            vel.z *= fTimeDelta;

            m_particlesData[i].Pos.x += vel.x;
            m_particlesData[i].Pos.y += vel.y;
            m_particlesData[i].Pos.z += vel.z;

            m_particlesData[i].Life += fTimeDelta / m_particleDesc.m_lifeSpan;
            m_particlesData[i].Size =  m_particleDesc.m_minSize + (m_particleDesc.m_maxSize - m_particleDesc.m_minSize) * m_particlesData[i].Life;

            if( m_particlesData[i].Life > 0.99f )
                m_particlesData[i].Life = -1;
        }
    }
}

void ParticleSystem::Draw()
{
    Update(GetApp().m_mainTimer.GetTotal(), GetApp().m_mainTimer.GetDelta());

    GetDX11Context()->UpdateSubresource(m_vb, NULL, NULL, m_particlesData, 0, 0);

    ID3D11DepthStencilState* originalStencilState;
    GetDX11Context()->OMGetDepthStencilState(&originalStencilState, NULL);

    GetDX11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    m_shader->Bind();

    XMMATRIX VP = GetCamera().GetView() * GetCamera().GetProj();

    SParticleCBufferWithoutSO cbuffer;
    cbuffer.worldViewProj = XMMatrixTranspose(VP);
    cbuffer.eyePos        = GetRenderer().m_camera.GetPosition();

    GetDX11Context()->UpdateSubresource(m_particleConstBuffer, 0, NULL, &cbuffer, 0, 0);

    GetDX11Context()->GSSetConstantBuffers(0, 1, &m_particleConstBuffer);
    GetDX11Context()->GSSetShaderResources(0, 1, &m_colorGradientTexture);
    GetDX11Context()->GSSetSamplers(0, 1,        &SamplerStates::DefaultSamplerClamp);

    GetDX11Context()->PSSetShaderResources(0, 1, &m_particleTexture);
    GetDX11Context()->PSSetSamplers(0, 1,        &SamplerStates::PointWrapSampler);

    GetDX11Context()->OMSetDepthStencilState(DepthStencilStates::DepthNoWriteDSS, 0);
    GetDX11Context()->OMSetBlendState(BlendStates::TransparentBS, 0, 0xffffffff);

    UINT stride = sizeof(SVertexBasicParticle);
    UINT offset = 0;

    GetDX11Context()->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);

    GetDX11Context()->Draw(m_particleDesc.m_particlesCount, 0);
    GetDX11Context()->OMSetDepthStencilState(originalStencilState, 0);
}


void ParticleSystem::InitializeShaders()
{
    HRESULT hr = S_OK;

    m_shader = new CShader();

    m_shader->CreateFromCompiledFile(EInputLayoutType::BasicParticle, L"../assets/particleBillboardPS.cso", L"../assets/particleBillboardVS.cso", L"../assets/particleBillboardGS.cso");

    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage          = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth      = sizeof(SParticleCBufferWithoutSO);
    cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;

    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_particleConstBuffer);
    if(FAILED(hr)) assert(false);
}

void ParticleSystem::InitializeTextures()
{
    HRESULT hr = S_OK;

    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, L"..\\assets\\textures\\smokevol1.dds",  NULL, &m_particleTexture);
    if(FAILED(hr)) assert(false);
    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, L"..\\assets\\textures\\colorgradient.dds",  NULL, &m_colorGradientTexture);
    if(FAILED(hr)) assert(false);
}

void ParticleSystem::InitializeVB()
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC vbdesc;
    vbdesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
    vbdesc.ByteWidth        = m_particleDesc.m_particlesCount * sizeof(SVertexBasicParticle);
    vbdesc.CPUAccessFlags   = 0;
    vbdesc.MiscFlags        = 0;
    vbdesc.Usage            = D3D11_USAGE_DEFAULT;

    hr = Globals::Get().device.m_device->CreateBuffer(&vbdesc, NULL, &m_vb) ;

    m_particlesData = new SVertexBasicParticle[m_particleDesc.m_particlesCount];

    for(uint i = 0; i < m_particleDesc.m_particlesCount; i++)
    {
        m_particlesData[i].Life = -1;
    }
}

void ParticleSystem::Initialize()
{
    InitializeVB();
    InitializeShaders();
    InitializeTextures();
}

float RandomPercent()
{
    float ret = (float)((rand() % 20000) - 10000 );
    return ret / 10000.0f;
}

void ParticleSystem::EmitParticle(SVertexBasicParticle* pParticle)
{
    pParticle->Pos.x = 0.0f;
    pParticle->Pos.y = 0.7f;
    pParticle->Pos.z = 3.0f;

    pParticle->Vel.x = 0.4f;
    pParticle->Vel.y = 0.3f * RandomPercent();
    pParticle->Vel.z = 0.3f * RandomPercent();

    XMVECTOR vel = XMVectorSet(pParticle->Vel.x, pParticle->Vel.y, pParticle->Vel.z, 1.0f);

    XMVector3Normalize(vel);

    pParticle->Vel.x *= m_particleDesc.m_velocity;
    pParticle->Vel.y *= m_particleDesc.m_velocity;
    pParticle->Vel.z *= m_particleDesc.m_velocity;

    pParticle->Life = 0.0f;
    pParticle->Size = 0.0f;
}
