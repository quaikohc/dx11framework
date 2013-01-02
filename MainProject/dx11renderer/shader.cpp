#include "stdafx.h"
#include "shader.h"

#include "inputLayouts.h"
#include "shader_reflection.h"


struct MyIncludeCallback : public ID3DInclude 
{
    std::string shaderContent;

    virtual HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) 
    {
        std::ifstream t(L"../assets/common.hlsl"); // TODO: "unhardcode" this
        std::stringstream buffer;
        buffer << t.rdbuf();
        shaderContent = buffer.str();

        *ppData = shaderContent.c_str();
        *pBytes = static_cast<uint>( shaderContent.size() );

        return S_OK;
    }

    virtual HRESULT __stdcall Close(LPCVOID pData) 
    {
        return S_OK;
    }
}; 

ID3DInclude* include = new MyIncludeCallback();


CShader::CShader() : m_gs(nullptr), m_ps(nullptr), m_vs(nullptr), m_ds(nullptr), m_hs(nullptr), m_psBlob(nullptr), m_vsBlob(nullptr)
{
} 

CShader::~CShader()
{
}

bool CShader::CompileFromFile(EInputLayoutType type, const wchar* psPath, const wchar* vsPath, const wchar* gsPath)
{
    HRESULT hr;
   //D3DReadFileToBlob ("main", m_vsBlob);


    hr = Globals::Get().device.CompileShaderFromFile(vsPath, "main", "vs_5_0", include, &m_vsBlob);
    if(FAILED(hr)) assert(false);

    hr = GetDX11Device()->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), NULL, &m_vs);
    if(FAILED(hr)) assert(false);

    hr = Globals::Get().device.CompileShaderFromFile(psPath, "main", "ps_5_0", include, &m_psBlob);
    if(FAILED(hr)) assert(false);

    hr = GetDX11Device()->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), NULL, &m_ps);
    if(FAILED(hr)) assert(false);


    hr = GetDX11Device()->CreateInputLayout(layoutPosTexNormalBitangentTangent, ARRAYSIZE(layoutPosTexNormalBitangentTangent), m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), &m_inputLayout);
    if(FAILED(hr)) assert(false);


    return true;
}

bool CShader::CreateFromCompiledFile(EInputLayoutType type, const wchar* psPath, const wchar* vsPath, const wchar* gsPath, const wchar* dsPath, const wchar* hsPath)
{
    HRESULT hr;
    CompileFromFile(type, L"../assets/parallaxPS.fx", L"../assets/parallaxVS.fx");

//    D3DReadFileToBlob(vsPath, &m_vsBlob);

    std::ifstream vsStream(vsPath, std::ifstream::binary);
    if(vsStream.fail()) assert(false);

    std::string vsString((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());

    int vsSize = vsString.size();

  //      hr = GetDevice()->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), NULL, &m_vs);

    hr = GetDX11Device()->CreateVertexShader(vsString.c_str(), vsSize, NULL, &m_vs);


    if(FAILED(hr)) assert(false);

    switch(type)
    {
        case EInputLayoutType::Position:
            hr = GetDX11Device()->CreateInputLayout(layoutPos, ARRAYSIZE(layoutPos), vsString.c_str(), vsSize, &m_inputLayout);
        break;
        case EInputLayoutType::PositionTexture:
            hr = GetDX11Device()->CreateInputLayout(layoutPosTex, ARRAYSIZE(layoutPosTex), vsString.c_str(), vsSize, &m_inputLayout);
        break;
        case EInputLayoutType::PositionTextureNormal:
            hr = GetDX11Device()->CreateInputLayout(layoutPosTexNorm, ARRAYSIZE(layoutPosTexNorm), vsString.c_str(), vsSize, &m_inputLayout);
        break;
        case EInputLayoutType::PositionTextureNormalTangentBinormal:
            hr = GetDX11Device()->CreateInputLayout(layoutPosTexNormalBitangentTangent, ARRAYSIZE(layoutPosTexNormalBitangentTangent), vsString.c_str(), vsSize, &m_inputLayout);
        break;
        case EInputLayoutType::PositionTextureNormalTangentSkinned:
            hr = GetDX11Device()->CreateInputLayout(layoutPosTexNormalTangentSkinned, ARRAYSIZE(layoutPosTexNormalTangentSkinned), vsString.c_str(), vsSize, &m_inputLayout);
            break;
        case EInputLayoutType::BasicParticle:
            hr = GetDX11Device()->CreateInputLayout(basicParticlelayout, ARRAYSIZE(basicParticlelayout), vsString.c_str(), vsSize, &m_inputLayout);
        break;
        case EInputLayoutType::TerrainLayout:
            hr = GetDX11Device()->CreateInputLayout(layoutTerrain, ARRAYSIZE(layoutTerrain), vsString.c_str(), vsSize, &m_inputLayout);
        break;
    }
    
    if(FAILED(hr)) assert(false);
    vsStream.close();

    ////////////////////////////////////////////

    std::ifstream psStream(psPath, std::ifstream::binary);
    if(psStream.fail()) assert(false);

    std::string psString((std::istreambuf_iterator<char>(psStream)), std::istreambuf_iterator<char>());

    int psSize = psString.size();

    hr = GetDX11Device()->CreatePixelShader(psString.c_str(), psSize, NULL, &m_ps);
    if(FAILED(hr)) assert(false);
    psStream.close();

    /////////////////////////////////////////////

    if(gsPath != nullptr)
    {
        std::ifstream gsStream(gsPath, std::ifstream::binary);
        if(gsStream.fail()) assert(false);

        std::string gsString((std::istreambuf_iterator<char>(gsStream)), std::istreambuf_iterator<char>());

        int gsSize = gsString.size();
        hr = GetDX11Device()->CreateGeometryShader(gsString.c_str(), gsSize, NULL, &m_gs);

        if(FAILED(hr)) assert(false);
        gsStream.close();
    }

    if(hsPath != nullptr)
    {
        std::ifstream hsStream(hsPath, std::ifstream::binary);
        if(hsStream.fail()) assert(false);

        std::string hsString((std::istreambuf_iterator<char>(hsStream)), std::istreambuf_iterator<char>());

        int hsSize = hsString.size();
        hr = GetDX11Device()->CreateHullShader(hsString.c_str(), hsSize, NULL, &m_hs);

        if(FAILED(hr)) assert(false);
        hsStream.close();
    }

    if(dsPath != nullptr)
    {
        std::ifstream dsStream(dsPath, std::ifstream::binary);
        if(dsStream.fail()) assert(false);

        std::string dsString((std::istreambuf_iterator<char>(dsStream)), std::istreambuf_iterator<char>());

        int dsSize = dsString.size();
        hr = GetDX11Device()->CreateDomainShader(dsString.c_str(), dsSize, NULL, &m_ds);

        if(FAILED(hr)) assert(false);
        dsStream.close();
    }
    return true;
}

void CShader::Bind()
{
    GetDX11Context()->IASetInputLayout(m_inputLayout);

    GetDX11Context()->VSSetShader(m_vs, 0, 0);
    GetDX11Context()->PSSetShader(m_ps, 0, 0);

    if(m_gs)
        GetDX11Context()->GSSetShader(m_gs, 0, 0);
    if(m_ds)
        GetDX11Context()->DSSetShader(m_ds, 0, 0);
    if(m_hs)
        GetDX11Context()->HSSetShader(m_hs, 0, 0);
}


void CShader::PerformVertexShaderReflection()
{
    CShaderReflection reflection;
    reflection.Initialize(m_vsBlob);

}