#include "stdafx.h"
#include "material.h"
#include "../utils/DDSTextureLoader.h"
#include "statesAndSamplers.h"

CMaterial::CMaterial() : m_diffuseRV(nullptr),
                         m_specularRV(nullptr),
                         m_normalRV(nullptr),
                         m_heightRV(nullptr)
{
}

CMaterial::~CMaterial()
{
    if(m_diffuseRV)
        m_diffuseRV->Release();
    if(m_specularRV)
        m_specularRV->Release();
    if(m_normalRV)
        m_normalRV->Release();
    if(m_heightRV)
        m_heightRV->Release();
}

bool CMaterial::Initialize(const wchar_t* diffusePath, const wchar_t* normalPath, const wchar_t* specularPath, const wchar_t* heightPath)
{
    HRESULT hr = S_OK;

    hr = CreateDDSTextureFromFile(GetDX11Device(), diffusePath,  NULL, &m_diffuseRV);
    if(FAILED(hr)) assert(false);

    hr = CreateDDSTextureFromFile(GetDX11Device(), normalPath,   NULL, &m_normalRV);
    if(FAILED(hr)) assert(false);

    hr = CreateDDSTextureFromFile(GetDX11Device(), specularPath, NULL, &m_specularRV);
    if(FAILED(hr)) assert(false);

    hr = CreateDDSTextureFromFile(GetDX11Device(), heightPath,   NULL, &m_heightRV);
    if(FAILED(hr)) assert(false);

    return true;
}


void CMaterial::Bind()
{
    Globals::Get().device.m_context->PSSetSamplers(0, 1, &SamplerStates::DefaultSamplerWrap);

    Globals::Get().device.m_context->PSSetShaderResources(0, 1, &m_diffuseRV);
   // Globals::Get().device.m_context->PSSetShaderResources(1, 1, &m_specularRV);
   // Globals::Get().device.m_context->PSSetShaderResources(2, 1, &m_normalRV);
    //Globals::Get().device.m_context->PSSetShaderResources(3, 1, &m_heightRV);
}