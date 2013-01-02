#include "stdafx.h"
#include "ShadowMap.h"



ShadowMap::ShadowMap(uint shadowMapWidth, uint shadowMapHeight) : m_shadowWidth(shadowMapWidth), m_shadowHeight(shadowMapHeight), m_depthMap(0), m_depthStencilView(0)
{
    m_viewport.TopLeftX         = 0.0f;
    m_viewport.TopLeftY         = 0.0f;
    m_viewport.Width            = static_cast<float>(shadowMapWidth);
    m_viewport.Height           = static_cast<float>(shadowMapHeight);
    m_viewport.MinDepth         = 0.0f;
    m_viewport.MaxDepth         = 1.0f;

    //////////////////////////////////   INIT SHADOW TEXTURE   ///////////////////////////////////////

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width              = shadowMapWidth;
    texDesc.Height             = shadowMapHeight;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.SampleDesc.Count   = 1;  
    texDesc.SampleDesc.Quality = 0;  
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags     = 0; 
    texDesc.MiscFlags          = 0;

    ID3D11Texture2D* depthMap  = 0;
    GetDX11Device()->CreateTexture2D(&texDesc, 0, &depthMap);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags              = 0;
    dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    GetDX11Device()->CreateDepthStencilView(depthMap, &dsvDesc, &m_depthStencilView);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format                    = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels       = texDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    GetDX11Device()->CreateShaderResourceView(depthMap, &srvDesc, &m_depthMap);

    depthMap->Release();
}

ShadowMap::~ShadowMap()
{
}

ID3D11ShaderResourceView* ShadowMap::DepthMapSRV()
{
    return m_depthMap;
}

void ShadowMap::BindDsvAndSetNullRenderTarget()
{
    GetDX11Context()->RSSetViewports(1, &m_viewport);

    ID3D11RenderTargetView* renderTargets[1] = { nullptr };
    GetDX11Context()->OMSetRenderTargets(1, renderTargets, m_depthStencilView);
    
    GetDX11Context()->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

  //  static float bgColor[4] = {(1.0f, 0.0f, 0.0f, 0.0f)};

  //  GetContext()->ClearRenderTargetView(m_importanceRTView, bgColor);
}