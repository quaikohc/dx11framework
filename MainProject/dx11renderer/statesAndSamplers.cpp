#include "stdafx.h"
#include "statesAndSamplers.h"


ID3D11RasterizerState*    RasterizerStates::WireframeRS         = nullptr;
ID3D11RasterizerState*    RasterizerStates::NoCullRS            = nullptr;
ID3D11RasterizerState*    RasterizerStates::DepthRS             = nullptr;
ID3D11RasterizerState*    RasterizerStates::CullFrontRS         = nullptr;
ID3D11RasterizerState*    RasterizerStates::CullBackRS          = nullptr;

ID3D11BlendState*         BlendStates::AlphaToCoverageBS        = nullptr;
ID3D11BlendState*         BlendStates::TransparentBS            = nullptr;
ID3D11BlendState*         BlendStates::MaxBS                    = nullptr;
ID3D11BlendState*         BlendStates::DisabledBS               = nullptr;
ID3D11BlendState*         BlendStates::AlphaBlendBS             = nullptr;

ID3D11DepthStencilState*  DepthStencilStates::DepthNoWriteDSS   = nullptr;
ID3D11DepthStencilState*  DepthStencilStates::DepthDisabledDSS  = nullptr;
ID3D11DepthStencilState*  DepthStencilStates::DepthTestDSS      = nullptr;

ID3D11SamplerState*       SamplerStates::PointWrapSampler       = nullptr;
ID3D11SamplerState*       SamplerStates::DefaultSamplerWrap     = nullptr;
ID3D11SamplerState*       SamplerStates::DefaultSamplerClamp    = nullptr;
ID3D11SamplerState*       SamplerStates::TerrainHeightSampler   = nullptr;
ID3D11SamplerState*       SamplerStates::ShadowSampler          = nullptr;



void RasterizerStates::InitAll()
{
    //
    // WireframeRS
    //
    D3D11_RASTERIZER_DESC wireframeDesc;
    ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
    wireframeDesc.FillMode              = D3D11_FILL_WIREFRAME;
    wireframeDesc.CullMode              = D3D11_CULL_BACK;
    wireframeDesc.FrontCounterClockwise = false;
    wireframeDesc.DepthClipEnable       = true;
    
    ASSERT_ON_FAIL(GetDX11Device()->CreateRasterizerState(&wireframeDesc, &WireframeRS));
    
    //
    // NoCullRS
    //
    D3D11_RASTERIZER_DESC noCullDesc;
    ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
    noCullDesc.FillMode              = D3D11_FILL_SOLID;
    noCullDesc.CullMode              = D3D11_CULL_NONE;
    noCullDesc.FrontCounterClockwise = false;
    noCullDesc.DepthClipEnable       = true;
    
    ASSERT_ON_FAIL(GetDX11Device()->CreateRasterizerState(&noCullDesc, &NoCullRS));
    
    //////////////

    D3D11_RASTERIZER_DESC cullFrontDesc;
    cullFrontDesc.FillMode              = D3D11_FILL_SOLID;
    cullFrontDesc.CullMode              = D3D11_CULL_FRONT;
   // cullFrontDesc.FrontCounterClockwise = false;
    ASSERT_ON_FAIL(GetDX11Device()->CreateRasterizerState(&cullFrontDesc, &CullFrontRS));

    //////////////

    D3D11_RASTERIZER_DESC cullBackDesc;
    cullBackDesc.FillMode              = D3D11_FILL_SOLID;
    cullBackDesc.CullMode              = D3D11_CULL_BACK;
   // cullBackDesc.FrontCounterClockwise = false;
    ASSERT_ON_FAIL(GetDX11Device()->CreateRasterizerState(&cullBackDesc, &CullBackRS));


    //
    // DepthRS
    //
    D3D11_RASTERIZER_DESC depthDesc;
    ZeroMemory(&depthDesc, sizeof(D3D11_RASTERIZER_DESC));
    depthDesc.FillMode              = D3D11_FILL_SOLID;
    depthDesc.CullMode              = D3D11_CULL_BACK;//
    depthDesc.DepthClipEnable       =  false;
    depthDesc.DepthBias             = 0.0f;//100000;
    depthDesc.DepthBiasClamp        = 0.0f;
    depthDesc.SlopeScaledDepthBias  = 1.0;//1.0f;
    ASSERT_ON_FAIL(GetDX11Device()->CreateRasterizerState(&depthDesc, &DepthRS));


}

void RasterizerStates::DestroyAll()
{
    SAFE_RELEASE(WireframeRS);
    SAFE_RELEASE(NoCullRS);
    SAFE_RELEASE(CullFrontRS);
    SAFE_RELEASE(CullBackRS);
    SAFE_RELEASE(DepthRS);
}


void BlendStates::InitAll()
{    
    D3D11_BLEND_DESC alphaBlendDesc = {0};
    
    alphaBlendDesc.AlphaToCoverageEnable  = false;
    alphaBlendDesc.IndependentBlendEnable = false;

    alphaBlendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    alphaBlendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    alphaBlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    alphaBlendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    alphaBlendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    alphaBlendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    alphaBlendDesc.RenderTarget[0].BlendEnable           = true;
    alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

    ASSERT_ON_FAIL(GetDX11Device()->CreateBlendState(&alphaBlendDesc, &AlphaBlendBS));

    //
    // AlphaToCoverageBS
    //
    D3D11_BLEND_DESC alphaToCoverageDesc = {0};
    alphaToCoverageDesc.AlphaToCoverageEnable                 = true;
    alphaToCoverageDesc.IndependentBlendEnable                = false;
    alphaToCoverageDesc.RenderTarget[0].BlendEnable           = false;
    alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    
    ASSERT_ON_FAIL(GetDX11Device()->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));
    
    //
    // TransparentBS
    //
    D3D11_BLEND_DESC transparentDesc = {0};
    transparentDesc.AlphaToCoverageEnable = false;
    transparentDesc.IndependentBlendEnable = false;
    
    transparentDesc.RenderTarget[0].BlendEnable = true;
    transparentDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
    transparentDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
    transparentDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
    transparentDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
    transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    transparentDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
    transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    
    ASSERT_ON_FAIL(GetDX11Device()->CreateBlendState(&transparentDesc, &TransparentBS));

    ///////////////////////////////

    D3D11_BLEND_DESC maxBlendDesc = {0};
    maxBlendDesc.AlphaToCoverageEnable  = false;
    maxBlendDesc.IndependentBlendEnable = false;

    maxBlendDesc.RenderTarget[0].BlendEnable    = false;
    maxBlendDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
    maxBlendDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
    maxBlendDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_ONE;
    maxBlendDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_ZERO;
    maxBlendDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
    maxBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    maxBlendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;// D3D11_COLOR_WRITE_ENABLE_ALL;

    ASSERT_ON_FAIL(GetDX11Device()->CreateBlendState(&maxBlendDesc, &MaxBS));

    //////////////////////////////////
    
    D3D11_BLEND_DESC blendDisabledDesc = {0};
    blendDisabledDesc.AlphaToCoverageEnable = false;
    blendDisabledDesc.IndependentBlendEnable = false;

    blendDisabledDesc.RenderTarget[0].BlendEnable           = true;
    blendDisabledDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MAX;
    blendDisabledDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MAX;
    blendDisabledDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
    blendDisabledDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
    blendDisabledDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blendDisabledDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
    blendDisabledDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ASSERT_ON_FAIL(GetDX11Device()->CreateBlendState(&blendDisabledDesc, &DisabledBS));
}

void BlendStates::DestroyAll()
{
    SAFE_RELEASE(AlphaBlendBS);
    SAFE_RELEASE(AlphaToCoverageBS);
    SAFE_RELEASE(TransparentBS);
    SAFE_RELEASE(MaxBS);
    SAFE_RELEASE(DisabledBS);
}

void DepthStencilStates::InitAll()
{
    HRESULT hr = S_OK;

    //
    // Depth stencil no write
    //
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = true;

    depthStencilDesc.DepthWriteMask                = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc                     = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable                 = FALSE;
    depthStencilDesc.StencilReadMask               = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask              = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depthStencilDesc.FrontFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.FrontFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp  = D3D11_STENCIL_OP_KEEP;

    depthStencilDesc.BackFace.StencilFunc          = D3D11_COMPARISON_NEVER;
    depthStencilDesc.BackFace.StencilPassOp        = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFailOp        = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp   = D3D11_STENCIL_OP_KEEP;

    hr = Globals::Get().device.m_device->CreateDepthStencilState(&depthStencilDesc, &DepthNoWriteDSS);
    if(FAILED(hr)) assert(false);

    //
    // Depth stencil no write
    //
    D3D11_DEPTH_STENCIL_DESC depthDisableDesc;
    depthDisableDesc.DepthEnable                   = false;
    depthDisableDesc.DepthWriteMask                = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthDisableDesc.DepthFunc                     = D3D11_COMPARISON_NEVER;
    depthDisableDesc.StencilEnable                 = FALSE;
    depthDisableDesc.StencilReadMask               = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthDisableDesc.StencilWriteMask              = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    hr = Globals::Get().device.m_device->CreateDepthStencilState(&depthDisableDesc, &DepthDisabledDSS);
    if(FAILED(hr)) assert(false);

    /////////////

    D3D11_DEPTH_STENCIL_DESC depthTestDesc;
    depthTestDesc.DepthEnable                   = true;
    depthTestDesc.DepthWriteMask                = D3D11_DEPTH_WRITE_MASK_ALL;
    depthTestDesc.DepthFunc                     = D3D11_COMPARISON_LESS;
    depthTestDesc.StencilEnable                 = FALSE;
    depthTestDesc.StencilReadMask               = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthTestDesc.StencilWriteMask              = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    hr = Globals::Get().device.m_device->CreateDepthStencilState(&depthTestDesc, &DepthTestDSS);
    if(FAILED(hr)) assert(false);

    /////////// 
}

void DepthStencilStates::DestroyAll()
{
    SAFE_RELEASE(DepthNoWriteDSS);
    SAFE_RELEASE(DepthDisabledDSS);
    SAFE_RELEASE(DepthTestDSS);
}


void SamplerStates::InitAll()
{
    HRESULT hr = S_OK;

    //
    // PointWrap Sampler
    //
    D3D11_SAMPLER_DESC pointWrapSampDesc;
    ZeroMemory(&pointWrapSampDesc, sizeof(pointWrapSampDesc));

    pointWrapSampDesc.Filter    = D3D11_FILTER_MIN_MAG_MIP_POINT;
    pointWrapSampDesc.AddressU  = D3D11_TEXTURE_ADDRESS_WRAP;
    pointWrapSampDesc.AddressV  = D3D11_TEXTURE_ADDRESS_WRAP;
    pointWrapSampDesc.AddressW  = D3D11_TEXTURE_ADDRESS_WRAP;

    hr = Globals::Get().device.m_device->CreateSamplerState(&pointWrapSampDesc, &PointWrapSampler);
    if(FAILED(hr)) assert(false);

    //
    // Default Sampler (wrap linear)
    //
    D3D11_SAMPLER_DESC defaultSampWrapDesc;
    ZeroMemory( &defaultSampWrapDesc, sizeof(defaultSampWrapDesc) );

    defaultSampWrapDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    defaultSampWrapDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;//CLAMP;
    defaultSampWrapDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;//CLAMP;
    defaultSampWrapDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;//CLAMP;
    defaultSampWrapDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
    defaultSampWrapDesc.MinLOD         = 0;
    defaultSampWrapDesc.MaxLOD         = D3D11_FLOAT32_MAX;
    
    hr = Globals::Get().device.m_device->CreateSamplerState(&defaultSampWrapDesc, &DefaultSamplerWrap);
    if(FAILED(hr)) assert(false);

    //
    // Default Sampler (clamp linear)
    //
    D3D11_SAMPLER_DESC defaultSampClampDesc;
    ZeroMemory( &defaultSampClampDesc, sizeof(defaultSampClampDesc) );

    defaultSampClampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    defaultSampClampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
    defaultSampClampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
    defaultSampClampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
    defaultSampClampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    defaultSampClampDesc.MinLOD         = 0;
    defaultSampClampDesc.MaxLOD         = D3D11_FLOAT32_MAX;

    hr = Globals::Get().device.m_device->CreateSamplerState(&defaultSampClampDesc, &DefaultSamplerClamp);
    if(FAILED(hr)) assert(false);

    //
    // Terrain height Sampler
    //
    D3D11_SAMPLER_DESC heightSampDesc;
    ZeroMemory(&heightSampDesc, sizeof(heightSampDesc));

    heightSampDesc.Filter          = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    heightSampDesc.AddressU        = D3D11_TEXTURE_ADDRESS_CLAMP;
    heightSampDesc.AddressV        = D3D11_TEXTURE_ADDRESS_CLAMP;
    heightSampDesc.AddressW        = D3D11_TEXTURE_ADDRESS_CLAMP;
    heightSampDesc.ComparisonFunc  = D3D11_COMPARISON_NEVER;
    heightSampDesc.MinLOD          = 0;
    heightSampDesc.MaxLOD          = D3D11_FLOAT32_MAX;

    hr = Globals::Get().device.m_device->CreateSamplerState(&heightSampDesc, &TerrainHeightSampler);
    if(FAILED(hr)) assert(false);

    //
    // Shadow comparision sampler
    //
    D3D11_SAMPLER_DESC shadowSampDesc;
    ZeroMemory(&shadowSampDesc, sizeof(shadowSampDesc));

    shadowSampDesc.Filter          = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    shadowSampDesc.AddressU        = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowSampDesc.AddressV        = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowSampDesc.AddressW        = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowSampDesc.ComparisonFunc  = D3D11_COMPARISON_LESS_EQUAL;
    shadowSampDesc.MinLOD          = 0;
    shadowSampDesc.MaxLOD          = D3D11_FLOAT32_MAX;

    shadowSampDesc.BorderColor[0] = 0.0f; 
    shadowSampDesc.BorderColor[1] = 0.0f; 
    shadowSampDesc.BorderColor[2] = 0.0f; 
    shadowSampDesc.BorderColor[3] = 0.0f; 
                  
    hr = Globals::Get().device.m_device->CreateSamplerState(&shadowSampDesc, &ShadowSampler);
    if(FAILED(hr)) assert(false);

    
}

void SamplerStates::DestroyAll()
{
    SAFE_RELEASE(PointWrapSampler);
    SAFE_RELEASE(DefaultSamplerWrap);
    SAFE_RELEASE(DefaultSamplerClamp);
    SAFE_RELEASE(TerrainHeightSampler);
    SAFE_RELEASE(ShadowSampler);
}