#include "stdafx.h"
#include "rtwShadowRenderer.h"
#include "model.h"
#include "statesAndSamplers.h"


struct QuadVertex
{
    XMFLOAT4 Position;
    XMFLOAT2 TexCoord;
};

void InitQuad()
{
    ID3D11VertexShader*             m_fullScreenQuadVS;
    ID3D11InputLayout*              m_fullScreenQuadInputLayout;
    ID3D11Buffer*                   m_quadVB;
    ID3D11Buffer*                   m_quadIB;

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    HRESULT hr;

    {
        std::ifstream vsStream("..\\assets\\fullScreenQuadVS.cso", std::ifstream::binary);
        if(vsStream.fail()) assert(false);

        std::string vsString((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());

        int vsSize = vsString.size();

        hr = Globals::Get().device.m_device->CreateVertexShader(vsString.c_str(), vsSize, NULL, &m_fullScreenQuadVS);
        ASSERT_ON_FAIL(hr);

        hr = Globals::Get().device.m_device->CreateInputLayout(layout, ARRAYSIZE(layoutPosTexNormalBitangentTangent), vsString.c_str(), vsSize, &m_fullScreenQuadInputLayout);
        ASSERT_ON_FAIL(hr);
    }

   QuadVertex verts[4] =
    {
        { XMFLOAT4( 1,  1, 1, 1), XMFLOAT2(1, 0) },
        { XMFLOAT4( 1, -1, 1, 1), XMFLOAT2(1, 1) },
        { XMFLOAT4(-1, -1, 1, 1), XMFLOAT2(0, 1) },
        { XMFLOAT4(-1,  1, 1, 1), XMFLOAT2(0, 0) }
    };

    D3D11_BUFFER_DESC desc;
    desc.Usage          = D3D11_USAGE_IMMUTABLE;
    desc.ByteWidth      = sizeof(verts);
    desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags      = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem          = verts;
    initData.SysMemPitch      = 0;
    initData.SysMemSlicePitch = 0;
    hr = GetDX11Device()->CreateBuffer(&desc, &initData, &m_quadVB);

    unsigned short indices[6] = { 0, 1, 2, 2, 3, 0 };

    desc.Usage          = D3D11_USAGE_IMMUTABLE;
    desc.ByteWidth      = sizeof(indices);
    desc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    initData.pSysMem    = indices;
    hr = GetDX11Device()->CreateBuffer(&desc, &initData, &m_quadIB);

    //////////////////////////

    GetDX11Context()->IASetInputLayout(m_fullScreenQuadInputLayout);

    UINT stride = sizeof(QuadVertex);
    UINT offset = 0;
    ID3D11Buffer* vertexBuffers[1] = { m_quadVB };
    GetDX11Context()->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

    GetDX11Context()->IASetIndexBuffer(m_quadIB, DXGI_FORMAT_R16_UINT, 0);

    GetDX11Context()->DrawIndexed(6, 0, 0);

}


CRtwShadowRenderer::CRtwShadowRenderer() : m_finalDS(nullptr), m_finalHS(nullptr)
{
    
}

CRtwShadowRenderer::~CRtwShadowRenderer()
{
}

bool CRtwShadowRenderer::Initialize()
{
    InitImportanceTex();

    InitViewports();

    InitImportanceShaders();

    CreateCBuffers();

    InitComputeShader();

    return true;
}


void  CRtwShadowRenderer::InitImportanceTex()
{
    HRESULT hr = S_OK;
    {
        D3D11_TEXTURE2D_DESC importanceTexDesc;
        ZeroMemory(&importanceTexDesc, sizeof(importanceTexDesc));

        importanceTexDesc.Width            = INITIAL_SHADOWMAP_SIZE;
        importanceTexDesc.Height           = INITIAL_SHADOWMAP_SIZE;
        importanceTexDesc.MipLevels        = 1;
        importanceTexDesc.ArraySize        = 1;
        importanceTexDesc.Format           = DXGI_FORMAT_R32_FLOAT;
        importanceTexDesc.SampleDesc.Count = 1;
        importanceTexDesc.Usage            = D3D11_USAGE_DEFAULT;
        importanceTexDesc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        importanceTexDesc.CPUAccessFlags   = 0;
        importanceTexDesc.MiscFlags        = 0;

        hr = GetDX11Device()->CreateTexture2D(&importanceTexDesc, NULL, &m_importanceTex);
         if(FAILED(hr)) assert(false);

        D3D11_RENDER_TARGET_VIEW_DESC importanceViewDesc;
        importanceViewDesc.Format             = importanceTexDesc.Format;
        importanceViewDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
        importanceViewDesc.Texture2D.MipSlice = 0;

        hr = GetDX11Device()->CreateRenderTargetView(m_importanceTex, &importanceViewDesc, &m_importanceRTView);
         if(FAILED(hr)) assert(false);

        D3D11_SHADER_RESOURCE_VIEW_DESC importanceSRVDesc;
        importanceSRVDesc.Format                    = importanceTexDesc.Format;
        importanceSRVDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        importanceSRVDesc.Texture2D.MostDetailedMip = 0;
        importanceSRVDesc.Texture2D.MipLevels       = 1;

        hr = GetDX11Device()->CreateShaderResourceView(m_importanceTex, &importanceSRVDesc, &m_importanceMap);
         if(FAILED(hr)) assert(false);
    }

    ///////////////////////////// INIT WARPED SHADOWMAP TEXTURE SRV
    //{ kShadowWidth, kShadowWidth, msl::gu::RenderBufferParam::TYPE_NONE, DXGI_FORMAT_UNKNOWN,msl::gu::DepthBufferParam::TYPE_NEW, NULL, DXGI_FORMAT_R24G8_TYPELESS },
    {
        D3D11_TEXTURE2D_DESC rtwShadowTexDesc;
        ZeroMemory(&rtwShadowTexDesc, sizeof(rtwShadowTexDesc));

        rtwShadowTexDesc.Width            = FINAL_SHADOWMAP_SIZE;
        rtwShadowTexDesc.Height           = FINAL_SHADOWMAP_SIZE;
        rtwShadowTexDesc.MipLevels        = 1;
        rtwShadowTexDesc.ArraySize        = 1;
        rtwShadowTexDesc.Format           = DXGI_FORMAT_R32_FLOAT;
        rtwShadowTexDesc.SampleDesc.Count = 1;
        rtwShadowTexDesc.Usage            = D3D11_USAGE_DEFAULT;
        rtwShadowTexDesc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        rtwShadowTexDesc.CPUAccessFlags   = 0;
        rtwShadowTexDesc.MiscFlags        = 0;

        hr = GetDX11Device()->CreateTexture2D(&rtwShadowTexDesc, NULL, &m_warpedShadowTex);
        if(FAILED(hr)) assert(false);

        D3D11_RENDER_TARGET_VIEW_DESC warpedViewDesc;
        warpedViewDesc.Format             = rtwShadowTexDesc.Format;
        warpedViewDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
        warpedViewDesc.Texture2D.MipSlice = 0;

        hr = GetDX11Device()->CreateRenderTargetView(m_warpedShadowTex, &warpedViewDesc, &m_warpedShadowRTView);
        if(FAILED(hr)) assert(false);

        D3D11_SHADER_RESOURCE_VIEW_DESC rtwSRVDesc;
        rtwSRVDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        rtwSRVDesc.Format                    = DXGI_FORMAT_R32_FLOAT;   //DXGI_FORMAT_R24G8_TYPELESS;
        rtwSRVDesc.Texture2D.MostDetailedMip = 0;
        rtwSRVDesc.Texture2D.MipLevels       = 1;

        hr = GetDX11Device()->CreateShaderResourceView(m_warpedShadowTex, &rtwSRVDesc, &m_warpedShadowMap);
        if(FAILED(hr)) assert(false);

        ///////////////////////////////////////////////////

        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.Width              = FINAL_SHADOWMAP_SIZE;
        texDesc.Height             = FINAL_SHADOWMAP_SIZE;
        texDesc.MipLevels          = 1;
        texDesc.ArraySize          = 1;
        texDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
        texDesc.SampleDesc.Count   = 1;  
        texDesc.SampleDesc.Quality = 0;  
        texDesc.Usage              = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL| D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags     = 0; 
        texDesc.MiscFlags          = 0;

        ID3D11Texture2D* depthMap  = 0;
        GetDX11Device()->CreateTexture2D(&texDesc, 0, &depthMap);
        if(FAILED(hr)) assert(false);


        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Flags              = 0;
        dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;//DXGI_FORMAT_R24G8_TYPELESS
        dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;

        hr = GetDX11Device()->CreateDepthStencilView(depthMap, &dsvDesc, &m_depthStencilView);
        if(FAILED(hr)) assert(false);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format                    = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels       = texDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        GetDX11Device()->CreateShaderResourceView(depthMap, &srvDesc, &m_depthMap);
    }

}


void CRtwShadowRenderer::InitComputeShader()
{
    ID3DBlob*   horizBuffer;
    ID3DBlob*   vertBuffer;

    HRESULT hr;
    hr = Globals::Get().device.CompileShaderFromFile(L"../assets/terrain/warpMap.hlsl", "DispatchMaxBlurWarpX", "cs_5_0", nullptr, &horizBuffer);
    if(FAILED(hr)) assert(false);

    hr = Globals::Get().device.CompileShaderFromFile(L"../assets/terrain/warpMap.hlsl", "DispatchMaxBlurWarpY", "cs_5_0", nullptr, &vertBuffer);
    if(FAILED(hr)) assert(false);

    hr = GetDX11Device()->CreateComputeShader(horizBuffer->GetBufferPointer(), horizBuffer->GetBufferSize(), 0, &m_warpHorizontal);
    if(FAILED(hr)) assert(false);

    hr = GetDX11Device()->CreateComputeShader(vertBuffer->GetBufferPointer(), vertBuffer->GetBufferSize(), 0, &m_warpVertical);
    if(FAILED(hr)) assert(false);

    ////////////////////////////////////////////////

    D3D11_TEXTURE1D_DESC textureDesc;
    ZeroMemory( &textureDesc, sizeof( textureDesc ) );
    textureDesc.Width          = INITIAL_SHADOWMAP_SIZE;
    textureDesc.MipLevels      = 1;
    textureDesc.ArraySize      = 1;
    textureDesc.Usage          = D3D11_USAGE_DEFAULT;
    textureDesc.Format         = DXGI_FORMAT_R32_FLOAT;
    textureDesc.BindFlags      = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE ;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags      = 0;

    hr =  GetDX11Device()->CreateTexture1D(&textureDesc, NULL, &m_horizontalWarpingTex); 
    ASSERT_ON_FAIL(hr);
    hr =  GetDX11Device()->CreateTexture1D(&textureDesc, NULL, &m_verticalWarpingTex);
    ASSERT_ON_FAIL(hr);

    D3D11_UNORDERED_ACCESS_VIEW_DESC viewDescUAV;
    ZeroMemory( &viewDescUAV, sizeof( viewDescUAV ) );
    viewDescUAV.Format              = DXGI_FORMAT_R32_FLOAT;
    viewDescUAV.ViewDimension       = D3D11_UAV_DIMENSION_TEXTURE1D;
    viewDescUAV.Buffer.FirstElement = 0;
    viewDescUAV.Texture1D.MipSlice  = 0;

    hr =  GetDX11Device()->CreateUnorderedAccessView(m_horizontalWarpingTex, &viewDescUAV, &m_horizUAV);
    ASSERT_ON_FAIL(hr);
    hr =  GetDX11Device()->CreateUnorderedAccessView(m_verticalWarpingTex,   &viewDescUAV, &m_vertUAV);
    ASSERT_ON_FAIL(hr);

    D3D11_SHADER_RESOURCE_VIEW_DESC warpSRVDesc;
    warpSRVDesc.Format                    = textureDesc.Format;
    warpSRVDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE1D;
    warpSRVDesc.Texture2D.MostDetailedMip = 0;
    warpSRVDesc.Texture2D.MipLevels       = 1;

    hr = GetDX11Device()->CreateShaderResourceView(m_horizontalWarpingTex, &warpSRVDesc, &m_horizWarpSRV);
    ASSERT_ON_FAIL(hr);
    hr = GetDX11Device()->CreateShaderResourceView(m_verticalWarpingTex, &warpSRVDesc, &m_vertWarpSRV);
    ASSERT_ON_FAIL(hr);


    ////////////////////////// INIT DOMAIN AND HULL SHADERS FOR SHADOWMAP RENDERING

    {
      /*  ID3DBlob*   domainShaderBuffer;
        ID3DBlob*   hullShaderBuffer;

        hr = Globals::Get().device.CompileShaderFromFile(L"../assets/rtwShadows/renderShadowmapWithTess.hlsl", "RenderRtwShadowDS", "ds_5_0", nullptr, &domainShaderBuffer);
        ASSERT_ON_FAIL(hr);

        hr = Globals::Get().device.CompileShaderFromFile(L"../assets/rtwShadows/renderShadowmapWithTess.hlsl", "RenderHS", "hs_5_0", nullptr, &hullShaderBuffer);
        ASSERT_ON_FAIL(hr);

        hr = GetDevice()->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), 0, &m_finalDS);
        ASSERT_ON_FAIL(hr);

        hr = GetDevice()->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), 0, &m_finalHS);
        ASSERT_ON_FAIL(hr);*/
    }

        ////////////////////////// INIT DOMAIN AND HULL SHADERS FOR FINAL RENDERING
    {
        ID3DBlob*   domainShaderBuffer;
        ID3DBlob*   hullShaderBuffer;

        hr = Globals::Get().device.CompileShaderFromFile(L"../assets/rtwShadows/renderFinalVS.hlsl", "FinalDS", "ds_5_0", nullptr, &domainShaderBuffer);
        ASSERT_ON_FAIL(hr);

        hr = Globals::Get().device.CompileShaderFromFile(L"../assets/rtwShadows/renderFinalVS.hlsl", "FinalHS", "hs_5_0", nullptr, &hullShaderBuffer);
        ASSERT_ON_FAIL(hr);

        hr = GetDX11Device()->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), 0, &m_finalDS);
        ASSERT_ON_FAIL(hr);

        hr = GetDX11Device()->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), 0, &m_finalHS);
        ASSERT_ON_FAIL(hr);
    }

}

void CRtwShadowRenderer::InitImportanceShaders()
{
    HRESULT hr;
    {
        std::ifstream vsStream("..\\assets\\importanceMapVS.cso", std::ifstream::binary);
        if(vsStream.fail()) assert(false);

        std::string vsString((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());

        int vsSize = vsString.size();

        hr = Globals::Get().device.m_device->CreateVertexShader(vsString.c_str(), vsSize, NULL, &m_importanceVS);
        ASSERT_ON_FAIL(hr);

        hr = Globals::Get().device.m_device->CreateInputLayout(layoutPosTexNormalBitangentTangent, ARRAYSIZE(layoutPosTexNormalBitangentTangent), vsString.c_str(), vsSize, &m_inputLayout);
        ASSERT_ON_FAIL(hr);
    }

    ////////////////////////////////////////////
    {
        std::ifstream psStream("..\\assets\\importanceMapPS.cso", std::ifstream::binary);
        if(psStream.fail()) assert(false);

        std::string psString((std::istreambuf_iterator<char>(psStream)), std::istreambuf_iterator<char>());

        int psSize = psString.size();

        hr = GetDX11Device()->CreatePixelShader(psString.c_str(), psSize, NULL, &m_importancePS);
        ASSERT_ON_FAIL(hr);
        psStream.close();
    }
    /////////////////////////////////////////////

    {
        std::ifstream vsStream("..\\assets\\shadowmapRtwVS.cso", std::ifstream::binary);
        if(vsStream.fail()) assert(false);

        std::string vsString((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());

        int vsSize = vsString.size();

        hr = Globals::Get().device.m_device->CreateVertexShader(vsString.c_str(), vsSize, NULL, &m_shadowMapRtwVS);
        ASSERT_ON_FAIL(hr);
    }

    ////////////////////////////////////////////
    {
        std::ifstream psStream("..\\assets\\shadowmapRtwPS.cso", std::ifstream::binary);
        if(psStream.fail()) assert(false);

        std::string psString((std::istreambuf_iterator<char>(psStream)), std::istreambuf_iterator<char>());

        int psSize = psString.size();

        hr = GetDX11Device()->CreatePixelShader(psString.c_str(), psSize, NULL, &m_shadowMapRtwPS);
        ASSERT_ON_FAIL(hr);
        psStream.close();
    }

    ///////////////////////////////////////////

    {
        std::ifstream vsStream("..\\assets\\renderFinalVS.cso", std::ifstream::binary);
        if(vsStream.fail()) assert(false);

        std::string vsString((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());

        int vsSize = vsString.size();

        hr = Globals::Get().device.m_device->CreateVertexShader(vsString.c_str(), vsSize, NULL, &m_finalVS);
        ASSERT_ON_FAIL(hr);
    }

    ////////////////////////////////////////////
    {
        std::ifstream psStream("..\\assets\\renderFinalPS.cso", std::ifstream::binary);
        if(psStream.fail()) assert(false);

        std::string psString((std::istreambuf_iterator<char>(psStream)), std::istreambuf_iterator<char>());

        int psSize = psString.size();

        hr = GetDX11Device()->CreatePixelShader(psString.c_str(), psSize, NULL, &m_finalPS);
        ASSERT_ON_FAIL(hr);
        psStream.close();
    }
    {
        std::ifstream psStream("..\\assets\\warpMapPS.cso", std::ifstream::binary);
        if(psStream.fail()) assert(false);

        std::string psString((std::istreambuf_iterator<char>(psStream)), std::istreambuf_iterator<char>());

        int psSize = psString.size();

        hr = GetDX11Device()->CreatePixelShader(psString.c_str(), psSize, NULL, &m_calculateWarpPS);
        ASSERT_ON_FAIL(hr);
        psStream.close();
    }
    ////////////////////////////////////////////
    {
        std::ifstream vsStream("..\\assets\\depthOnly.cso", std::ifstream::binary);
        if(vsStream.fail()) assert(false);

        std::string vsString((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());

        int vsSize = vsString.size();

        hr = Globals::Get().device.m_device->CreateVertexShader(vsString.c_str(), vsSize, NULL, &m_depthVS);
        if(FAILED(hr)) assert(false);
    }
}

void CRtwShadowRenderer::CreateCBuffers()
{
    HRESULT hr;

    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage          = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth      = sizeof(SImportanceVSCBuffer);
    cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;


    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_importanceVSCBuffer);
    ASSERT_ON_FAIL(hr);

    ////////////////////////////

    cbbd.ByteWidth      = sizeof(SImportancePSCBuffer);

    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_importancePSCBuffer);
    ASSERT_ON_FAIL(hr);

    ////////////////////////////

    cbbd.ByteWidth      = sizeof(SShadowMapRTWCBuffer);

    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_rtwShadowMapCBuffer);
    ASSERT_ON_FAIL(hr);

    ////////////////////////////

    cbbd.ByteWidth      = sizeof(SRenderFinalVSCbuffer);

    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_finalVSCBuffer);
    ASSERT_ON_FAIL(hr);

    ////////////////////////////
    
    cbbd.ByteWidth      = sizeof(SRenderFinalPSCBuffer);

    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_finalPSCBuffer);
    ASSERT_ON_FAIL(hr);

    ////////////////////////////
    cbbd.ByteWidth      = sizeof(SDepthCBuffer2);

    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_depthConstBuffer);
    ASSERT_ON_FAIL(hr);
}

void CRtwShadowRenderer::InitViewports()
{
    m_importanceViewport.TopLeftX      = 0.0f;
    m_importanceViewport.TopLeftY      = 0.0f;
    m_importanceViewport.Width         = static_cast<float>(INITIAL_SHADOWMAP_SIZE);
    m_importanceViewport.Height        = static_cast<float>(INITIAL_SHADOWMAP_SIZE);
    m_importanceViewport.MinDepth      = 0.0f;
    m_importanceViewport.MaxDepth      = 1.0f;

    m_finalShadowmapViewport.TopLeftX  = 0.0f;
    m_finalShadowmapViewport.TopLeftY  = 0.0f;
    m_finalShadowmapViewport.Width     = static_cast<float>(FINAL_SHADOWMAP_SIZE);
    m_finalShadowmapViewport.Height    = static_cast<float>(FINAL_SHADOWMAP_SIZE);
    m_finalShadowmapViewport.MinDepth  = 0.0f;
    m_finalShadowmapViewport.MaxDepth  = 1.0f;

    m_finalScreenViewport.TopLeftX     = 0.0f;
    m_finalScreenViewport.TopLeftY     = 0.0f;
    m_finalScreenViewport.Width        = static_cast<float>(SCREEN_SIZE_X);
    m_finalScreenViewport.Height       = static_cast<float>(SCREEN_SIZE_Y);
    m_finalScreenViewport.MinDepth     = 0.0f;
    m_finalScreenViewport.MaxDepth     = 1.0f;

    m_warpMapViewport.TopLeftX         = 0.0f;
    m_warpMapViewport.TopLeftY         = 0.0f;
    m_warpMapViewport.Width            = static_cast<float>(INITIAL_SHADOWMAP_SIZE);
    m_warpMapViewport.Height           = 1;
    m_warpMapViewport.MinDepth         = 0.0f;
    m_warpMapViewport.MaxDepth         = 1.0f;
}

void CRtwShadowRenderer::Draw()
{
    GetDX11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


   // DrawFirstPassDepth();

    DrawImportance();

    ID3D11RenderTargetView* renderTargets[1] = { nullptr };
    GetDX11Context()->OMSetRenderTargets(1, renderTargets, nullptr);

    GenerateWarpMapCS();

    DrawRtwShadows();

    DrawFinal();
}



// shadow - point, normaldepth- linear wrap
//pContext->SetDepthStencilState( pScene->GetDepthNoneState(), 0 );
//pContext->SetBlendState( pScene->GetBlendMaxState(), colorRef, 0xFFFFFFFF );
//pContext->SetRasterizerState( pScene->GetRasterFrontState() );
//{
//fill( FILL_SOLID ), 
//cull( CULL_BACK )
//frontCCW( false )
//depthBias( 0 ), depthBiasClamp( 0.0f ), depthBiasSlopeScale( 0.0f )
//depthClipEnable( true )
//scissorEnable( false )
//}
//pContext->SetClearColor( msl::math::Vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );
void CRtwShadowRenderer::DrawImportance()
{
    ID3D11DepthStencilState* originalStencilState;

    GetDX11Context()->OMGetDepthStencilState(&originalStencilState, NULL);

    GetDX11Context()->RSSetState(RasterizerStates::NoCullRS);
   
  //  GetContext()->OMSetDepthStencilState(DepthStencilStates::DepthTestDSS, 0);
    GetDX11Context()->OMSetDepthStencilState(DepthStencilStates::DepthDisabledDSS, 0);

    GetDX11Context()->OMSetBlendState(BlendStates::MaxBS, 0, 0xffffffff);


    GetDX11Context()->RSSetViewports(1, &m_importanceViewport);

    ID3D11RenderTargetView* renderTargets[1] = { m_importanceRTView };
    GetDX11Context()->OMSetRenderTargets(1, renderTargets, /*m_depthStencilView*/nullptr);

  // GetContext()->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    static float bgColor[4] = {(0.0f, 0.0f, 0.0f, 0.0f)};

    GetDX11Context()->ClearRenderTargetView(m_importanceRTView, bgColor);


    GetDX11Context()->VSSetShader(m_importanceVS, 0, 0);
    GetDX11Context()->PSSetShader(m_importancePS, 0, 0);

    GetDX11Context()->IASetInputLayout(m_inputLayout);

   // GetContext()->PSSetSamplers(0, 1, &SamplerStates::PointWrapSampler);
  //  GetContext()->PSSetShaderResources(0, 1, &m_depthMap);

    int modelsCount = Globals::Get().scene.m_models.size();

    for(int i = 0; i < modelsCount; i++)
    {
        D3DPERF_BeginEvent(D3DCOLOR_XRGB( 255, 0, 0  ),  L"DRAW_IMPORTANCE");

        XMMATRIX world =  Globals::Get().scene.m_models[i]->m_transformation.GetWorld();

        XMMATRIX lightView       = XMLoadFloat4x4(&GetRenderer().mLightView);
        XMMATRIX lightProj       = XMLoadFloat4x4(&GetRenderer().mLightProj);
        XMMATRIX lightViewProj   = XMMatrixMultiply(lightView, lightProj);

        SImportanceVSCBuffer importanceCB;
        importanceCB.mtxView     = XMMatrixTranspose(GetCamera().GetView());
        importanceCB.mtxProj     = XMMatrixTranspose(GetCamera().GetProj());
        importanceCB.mtxWorld    = XMMatrixTranspose(world);
        importanceCB.mtxShadowVP = XMMatrixTranspose(lightViewProj);

        float nearCam = GetCamera().GetNearZ();
        float farCam  = GetCamera().GetFarZ();
        importanceCB.screenParam = XMFLOAT4(nearCam, farCam, SCREEN_SIZE_X, SCREEN_SIZE_Y);

        SImportancePSCBuffer importancePSCB;
  
        importancePSCB.minEdgeValue     = 0.3f;
        importancePSCB.edgeThreshold    = 0.2f;
        importancePSCB.normalMultiValue = 2.0f;
        importancePSCB.depthOffset      = 0.01f;;

        GetDX11Context()->UpdateSubresource(m_importanceVSCBuffer, 0, NULL, &importanceCB, 0, 0);
        GetDX11Context()->VSSetConstantBuffers(0, 1, &m_importanceVSCBuffer);

        GetDX11Context()->UpdateSubresource(m_importancePSCBuffer, 0, NULL, &importancePSCB, 0, 0);
        GetDX11Context()->PSSetConstantBuffers(0, 1, &m_importancePSCBuffer);


        uint offset = 0;
        GetDX11Context()->IASetIndexBuffer(Globals::Get().scene.m_models[i]->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        GetDX11Context()->IASetVertexBuffers(0, 1, &Globals::Get().scene.m_models[i]->m_mesh->m_vertexBuffer, &Globals::Get().scene.m_models[i]->m_mesh->m_stride, &offset);

        GetDX11Context()->DrawIndexed(Globals::Get().scene.m_models[i]->m_mesh->m_indexCount, 0, 0 );

        D3DPERF_EndEvent();
    }

    GetDX11Context()->OMSetDepthStencilState(originalStencilState, 0);

    ID3D11ShaderResourceView* pReses[] = { NULL, NULL };
    GetDX11Context()->PSSetShaderResources( 0, 2, pReses );
}


void CRtwShadowRenderer::GenerateWarpMapPS()
{
    GetDX11Context()->RSSetViewports(1, &m_warpMapViewport);


    
}

void CRtwShadowRenderer::GenerateWarpMapCS()
{
    GetDX11Context()->CSSetShaderResources(0, 1, &m_importanceMap);

    {
        UINT uavInits[] = { 0 };
        GetDX11Context()->CSSetUnorderedAccessViews( 0, 1, &m_horizUAV, uavInits );
        GetDX11Context()->CSSetShader( m_warpHorizontal, NULL, 0 );

        GetDX11Context()->Dispatch(1, 1, 1 ); // params: xyz thread groups count
    }

    {
        UINT uavInits[] = { 0 };
        GetDX11Context()->CSSetUnorderedAccessViews( 0, 1, &m_vertUAV, uavInits );
        GetDX11Context()->CSSetShader( m_warpVertical, NULL, 0 );

        GetDX11Context()->Dispatch(1, 1, 1 );
    }

///////////////////// clear

    UINT uavInits[] = { 0 };
    ID3D11UnorderedAccessView* uavs[] = { NULL };
    GetDX11Context()->CSSetUnorderedAccessViews( 0, ARRAY_SIZE_IN_ELEMENTS(uavs), uavs, uavInits );
    ID3D11ShaderResourceView* views[] = { NULL };
    GetDX11Context()->CSSetShaderResources( 0, ARRAY_SIZE_IN_ELEMENTS(views), views );

    GetDX11Context()->CSSetShader(nullptr, 0, 0);

}

//pContext->SetClearColor( msl::math::Vec4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
//pContext->SetDepthStencilState( pScene->GetDepthTestState(), 0 );
//float   colorRef[4];
//pContext->SetBlendState( pScene->GetBlendNoneState(), colorRef, 0xFFFFFFFF );
//pContext->SetRasterizerState( pScene->GetRasterBackState() );        //			param.cull = CULL_FRONT;


void CRtwShadowRenderer::DrawRtwShadows()
{
    ID3D11DepthStencilState* originalStencilState;
    GetDX11Context()->OMGetDepthStencilState(&originalStencilState, NULL);


    GetDX11Context()->RSSetState(RasterizerStates::DepthRS);
    GetDX11Context()->RSSetViewports(1, &m_finalShadowmapViewport);

    ID3D11RenderTargetView* renderTargets[1] = { m_warpedShadowRTView };
    GetDX11Context()->OMSetRenderTargets(1, renderTargets, m_depthStencilView);
    GetDX11Context()->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    static float bgColor[4] = {(1.0f, 1.0f, 1.0f, 1.0f)};
    GetDX11Context()->ClearRenderTargetView(m_warpedShadowRTView, bgColor);


    GetDX11Context()->VSSetShader(m_shadowMapRtwVS, 0, 0);
    GetDX11Context()->PSSetShader(m_shadowMapRtwPS, 0, 0);

    GetDX11Context()->IASetInputLayout(m_inputLayout);

    GetDX11Context()->VSSetSamplers(0, 1, &SamplerStates::PointWrapSampler);

    ID3D11ShaderResourceView* horiz    = m_horizWarpSRV;
    ID3D11ShaderResourceView* vertical = m_vertWarpSRV;

    GetDX11Context()->VSSetShaderResources(0, 1, &horiz);
    GetDX11Context()->VSSetShaderResources(1, 1, &vertical);

    
    int modelsCount = Globals::Get().scene.m_models.size();
    for(int i = 0; i < modelsCount; i++)
    {
        D3DPERF_BeginEvent(D3DCOLOR_XRGB(255, 0, 0), L"DRAW_RTW_SHADOW"); 
        XMMATRIX world =  Globals::Get().scene.m_models[i]->m_transformation.GetWorld();

        XMMATRIX lightView     = XMLoadFloat4x4(&GetRenderer().mLightView);
        XMMATRIX lightProj     = XMLoadFloat4x4(&GetRenderer().mLightProj);
        XMMATRIX lightViewProj = XMMatrixMultiply(lightView, lightProj);

        SShadowMapRTWCBuffer rtwShadowCBuffer;
        rtwShadowCBuffer.mtxShadowVP = XMMatrixTranspose(lightViewProj);
        rtwShadowCBuffer.mtxWorld    = XMMatrixTranspose(world);

        GetDX11Context()->UpdateSubresource(m_rtwShadowMapCBuffer, 0, NULL, &rtwShadowCBuffer, 0, 0);
        GetDX11Context()->VSSetConstantBuffers(0, 1, &m_rtwShadowMapCBuffer);

        uint offset = 0;
        GetDX11Context()->IASetIndexBuffer(Globals::Get().scene.m_models[i]->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        GetDX11Context()->IASetVertexBuffers(0, 1, &Globals::Get().scene.m_models[i]->m_mesh->m_vertexBuffer, &Globals::Get().scene.m_models[i]->m_mesh->m_stride, &offset);

        GetDX11Context()->DrawIndexed(Globals::Get().scene.m_models[i]->m_mesh->m_indexCount, 0, 0 );

        D3DPERF_EndEvent();
    }


    ID3D11ShaderResourceView* resView[] = { NULL, NULL };
 
    GetDX11Context()->VSSetShaderResources(0, 2, resView);

    GetDX11Context()->PSSetShader(nullptr, 0, 0);
    GetDX11Context()->VSSetShader(nullptr, 0, 0);

    GetDX11Context()->OMSetDepthStencilState(originalStencilState, 0);
}

void CRtwShadowRenderer::DrawFinal()
{

    GetDX11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    GetDX11Context()->OMSetDepthStencilState(DepthStencilStates::DepthTestDSS, 0);


    if( GetAsyncKeyState('2') & 0x8000 )
        GetDX11Context()->RSSetState(RasterizerStates::WireframeRS);

    GetDX11Context()->OMSetBlendState(BlendStates::AlphaBlendBS, 0, 0xffffffff);

    GetDX11Context()->RSSetViewports(1, &m_finalScreenViewport);

    static float bgColor[4] = {(0.0f, 0.0f, 0.0f, 0.0f)};

    GetDX11Context()->ClearRenderTargetView(Globals::Get().device.m_renderTargetView, bgColor);
    GetDX11Context()->ClearDepthStencilView(Globals::Get().device.m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    GetDX11Context()->OMSetRenderTargets(1, &Globals::Get().device.m_renderTargetView, Globals::Get().device.m_depthStencilView);


    //////////

    GetDX11Context()->HSSetShader(m_finalHS, 0, 0);
    GetDX11Context()->DSSetShader(m_finalDS, 0, 0);

    //////////

    GetDX11Context()->VSSetShader(m_finalVS, 0, 0);
    GetDX11Context()->PSSetShader(m_finalPS, 0, 0);

    GetDX11Context()->IASetInputLayout(m_inputLayout);

    GetDX11Context()->PSSetSamplers(0, 1, &SamplerStates::PointWrapSampler);
    GetDX11Context()->PSSetSamplers(1, 1, &SamplerStates::DefaultSamplerWrap);
    GetDX11Context()->PSSetSamplers(2, 1, &SamplerStates::DefaultSamplerClamp);

    

    int modelsCount = Globals::Get().scene.m_models.size();

    for(int i = 0; i < modelsCount; i++)
    {
        D3DPERF_BeginEvent(D3DCOLOR_XRGB(255, 0, 0), L"DRAW_FINAL_WITH_SHADOW"); 

        XMMATRIX world =  Globals::Get().scene.m_models[i]->m_transformation.GetWorld();

        XMMATRIX lightView       = XMLoadFloat4x4(&GetRenderer().mLightView);
        XMMATRIX lightProj       = XMLoadFloat4x4(&GetRenderer().mLightProj);
        XMMATRIX lightViewProj   = XMMatrixMultiply(lightView, lightProj);


        SRenderFinalVSCbuffer cbufferVS;
        cbufferVS.mtxView        = XMMatrixTranspose(GetCamera().GetView());
        cbufferVS.mtxProj        = XMMatrixTranspose(GetCamera().GetProj());
        cbufferVS.mtxWorld       = XMMatrixTranspose(world);
        cbufferVS.mtxWorldIt     = XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(world), world));
        cbufferVS.mtxShadowVP    = XMMatrixTranspose(lightViewProj);
        cbufferVS.g_texScale     = Globals::Get().scene.m_models[i]->texScale;

        GetDX11Context()->UpdateSubresource(m_finalVSCBuffer, 0, NULL, &cbufferVS, 0, 0);
        GetDX11Context()->VSSetConstantBuffers(0, 1, &m_finalVSCBuffer);

        ///////
                GetDX11Context()->DSSetConstantBuffers(0, 1, &m_finalVSCBuffer);
        ///////

        SRenderFinalPSCBuffer cbufferPS;
        cbufferPS.lightDir.x = GetGlobalState().GetSun().Direction.x;
        cbufferPS.lightDir.y = GetGlobalState().GetSun().Direction.y;
        cbufferPS.lightDir.z = GetGlobalState().GetSun().Direction.z;

        GetDX11Context()->UpdateSubresource(m_finalPSCBuffer, 0, NULL, &cbufferPS, 0, 0);
        GetDX11Context()->PSSetConstantBuffers(0, 1, &m_finalPSCBuffer);

        /////////// set textures
        GetDX11Context()->PSSetShaderResources(0, 1, &Globals::Get().scene.m_models[i]->m_material->m_diffuseRV);
        GetDX11Context()->PSSetShaderResources(1, 1, &m_warpedShadowMap);
        GetDX11Context()->PSSetShaderResources(2, 1, &m_horizWarpSRV);
        GetDX11Context()->PSSetShaderResources(3, 1, &m_vertWarpSRV);


        uint offset = 0;
        GetDX11Context()->IASetIndexBuffer(Globals::Get().scene.m_models[i]->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        GetDX11Context()->IASetVertexBuffers(0, 1, &Globals::Get().scene.m_models[i]->m_mesh->m_vertexBuffer, &Globals::Get().scene.m_models[i]->m_mesh->m_stride, &offset);


        GetDX11Context()->DrawIndexed(Globals::Get().scene.m_models[i]->m_mesh->m_indexCount, 0, 0 );

        D3DPERF_EndEvent();
    }

    GetDX11Context()->HSSetShader(NULL, 0, 0);
    GetDX11Context()->DSSetShader(NULL, 0, 0);

        GetDX11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}





void CRtwShadowRenderer::DrawFirstPassDepth()
{
    GetDX11Context()->RSSetState(RasterizerStates::DepthRS);

    GetDX11Context()->RSSetViewports(1, &m_importanceViewport);

    ID3D11RenderTargetView* renderTargets[1] = { nullptr };
    GetDX11Context()->OMSetRenderTargets(1, renderTargets, m_depthStencilView);

    GetDX11Context()->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    GetDX11Context()->VSSetShader(m_depthVS, 0, 0);
    GetDX11Context()->PSSetShader(nullptr, 0, 0);

    GetDX11Context()->IASetInputLayout(m_inputLayout);

    XMMATRIX lightView     = XMLoadFloat4x4(&GetRenderer().mLightView);
    XMMATRIX lightProj     = XMLoadFloat4x4(&GetRenderer().mLightProj);
    XMMATRIX lightViewProj = XMMatrixMultiply(lightView, lightProj);

    int modelsCount = Globals::Get().scene.m_models.size();
    for(int i = 0; i < modelsCount; i++)
    {
        D3DPERF_BeginEvent(D3DCOLOR_XRGB( 255, 0, 0  ),  L"DRAW_DEPTH_FIRST_PASS");

        XMMATRIX world = Globals::Get().scene.m_models[i]->m_transformation.GetWorld();

        SDepthCBuffer2 cbuffer;
        cbuffer.gWorldLightViewProj = XMMatrixTranspose(world * lightViewProj);

        GetDX11Context()->UpdateSubresource(m_depthConstBuffer, 0, NULL, &cbuffer, 0, 0);

        GetDX11Context()->VSSetConstantBuffers(0, 1, &m_depthConstBuffer);

        uint offset = 0;
        GetDX11Context()->IASetIndexBuffer(Globals::Get().scene.m_models[i]->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        GetDX11Context()->IASetVertexBuffers(0, 1, &Globals::Get().scene.m_models[i]->m_mesh->m_vertexBuffer, &Globals::Get().scene.m_models[i]->m_mesh->m_stride, &offset);

        GetDX11Context()->DrawIndexed(Globals::Get().scene.m_models[i]->m_mesh->m_indexCount, 0, 0 );

        D3DPERF_EndEvent();
    }
}