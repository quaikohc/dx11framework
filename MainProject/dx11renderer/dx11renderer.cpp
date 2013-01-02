#include "stdafx.h"
#include "dx11renderer.h"
#include "font_renderer_gdi.h"

#include "../utils/geometry_generator.h"
#include "../utils/DDSTextureLoader.h"
#include "statesAndSamplers.h"
#include "mesh.h"
#include "model.h"

#include <AntTweakBar.h>
#include "rtwShadowRenderer.h"

DxFont FontA;

CRtwShadowRenderer g_rtwShadowRenderer;

bool showDebug = true;

struct SDepthCBuffer
{
    XMMATRIX gWorldLightViewProj;
};


void CDX11Renderer::BuildShadowTransform() 
{
    float radius = GetGlobalState().GetSceneBounds().Radius;

   // XMVECTOR look = XMVectorNegate(XMVector3Normalize(XMLoadFloat3(&data.MainLight.Direction)));
     XMVECTOR lightDir = XMVector3Normalize(XMLoadFloat3(&GetGlobalState().GetSun().Direction));

    XMVECTOR lightPos   = -2.0f * radius * lightDir;
    XMVECTOR targetPos  = XMLoadFloat3(&GetGlobalState().GetSceneBounds().Center);
    XMVECTOR up         = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

    XMFLOAT3 spherePosLS;
    XMStoreFloat3(&spherePosLS, XMVector3TransformCoord(targetPos, V));    //  bounding sphere to light space.

    float left      = spherePosLS.x - radius;
    float right     = spherePosLS.x + radius;
                    
    float bottom    = spherePosLS.y - radius;
    float top       = spherePosLS.y + radius;

    float nearPlane = spherePosLS.z - radius;
    float farPlane  = spherePosLS.z + radius;

    XMMATRIX P = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearPlane, farPlane); // parameters are in camera space

    // currently not used (done in shader), bias is also not used here at the moment
    // transform NDC space [-1,+1] to texture space [0,1]
    XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f /* -bias  */, 1.0f);


    XMMATRIX S = V * P * T;

    XMStoreFloat4x4(&mLightView, V);
    XMStoreFloat4x4(&mLightProj, P);
    XMStoreFloat4x4(&mShadowTransform, S);

}


CDX11Renderer::CDX11Renderer()
{
}

CDX11Renderer::~CDX11Renderer()
{
    RasterizerStates::DestroyAll();

    BlendStates::DestroyAll();
    DepthStencilStates::DestroyAll();

    SamplerStates::DestroyAll();
}

void CDX11Renderer::InitParticles()
{
    m_testParticleSystem2.Initialize();
}

void CDX11Renderer::DrawParticles()
{
    m_testParticleSystem2.Draw();
}

void CDX11Renderer::InitShadowMap()
{
    HRESULT hr = S_OK;

    m_shadowMap = new ShadowMap(1024, 1024);

    std::ifstream vsStream("..\\assets\\shadowMap.cso", std::ifstream::binary);
    if(vsStream.fail()) assert(false);

    std::string vsString((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());

    int vsSize = vsString.size();

    hr = Globals::Get().device.m_device->CreateVertexShader(vsString.c_str(), vsSize, NULL, &m_depthVS);
    if(FAILED(hr)) assert(false);

    hr = Globals::Get().device.m_device->CreateInputLayout(layoutPosTexNormalBitangentTangent, ARRAYSIZE(layoutPosTexNormalBitangentTangent), vsString.c_str(), vsSize, &m_depthInputLayout);


    /////////////////////////////////////////////

    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage          = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth      = sizeof(SDepthCBuffer);
    cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;

    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_depthConstBuffer);
    if(FAILED(hr)) assert(false);


    ////////////////////////////

    D3D11_BUFFER_DESC cbbd2;
    ZeroMemory(&cbbd2, sizeof(D3D11_BUFFER_DESC));

    cbbd2.Usage          = D3D11_USAGE_DEFAULT;
    cbbd2.ByteWidth      = sizeof(SPerObjectCBuffer);
    cbbd2.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd2.CPUAccessFlags = 0;

    hr = Globals::Get().device.m_device->CreateBuffer(&cbbd2, NULL, &m_perObjectBuffer);
    if(FAILED(hr)) assert(false);



    //////////////////////////////
}

void CDX11Renderer::InitSkinned()
{
    m_skinnedModel = new SkinnedModel("..\\assets\\leopard.dae");
    m_skinnedModelInstance.Model       = m_skinnedModel;

    m_skinnedModelInstance.TimePos     = 0.0f;
    m_skinnedModelInstance.ClipName    = "Take1";
    m_skinnedModelInstance.FinalTransforms.resize(40);//mCharacterModel->m_skinnedData.BoneCount());
   

   // mCharacterInstance1.World = XMMatrixIdentity();
//    XMStoreFloat4x4(&mCharacterInstance1.World, XMMatrixIdentity());

  //  XMMATRIX modelScale = XMMatrixScaling(0.05f, 0.05f, -0.05f);
   // XMMATRIX modelRot   = XMMatrixRotationY(3.14f);
   // XMMATRIX modelOffset = XMMatrixTranslation(-2.0f, 0.0f, -7.0f);
   // XMStoreFloat4x4(&mCharacterInstance1.World, modelScale*modelRot*modelOffset);

}

void CDX11Renderer::DrawSkinned()
{
    m_skinnedModelInstance.Draw();
}


void CDX11Renderer::DrawModelsToShadowMap(CModel* model)
{
    D3DPERF_BeginEvent( D3DCOLOR_XRGB( 255, 0, 0  ),  L"DRAW_TO_SHADOWMAP" ); //   D3DPERF_EndEvent();

    XMMATRIX lightView     = XMLoadFloat4x4(&mLightView);
    XMMATRIX lightProj     = XMLoadFloat4x4(&mLightProj);
    XMMATRIX lightViewProj = XMMatrixMultiply(lightView, lightProj);

    XMMATRIX world = model->m_transformation.GetWorld();

    SDepthCBuffer cbuffer;
    cbuffer.gWorldLightViewProj = XMMatrixTranspose(world * lightViewProj);

    GetDX11Context()->UpdateSubresource(m_depthConstBuffer, 0, NULL, &cbuffer, 0, 0);

    GetDX11Context()->VSSetConstantBuffers(0, 1, &m_depthConstBuffer);

    uint offset = 0;
    GetDX11Context()->IASetIndexBuffer(model->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    GetDX11Context()->IASetVertexBuffers(0, 1, &model->m_mesh->m_vertexBuffer, &model->m_mesh->m_stride, &offset);

    GetDX11Context()->DrawIndexed(model->m_mesh->m_indexCount, 0, 0 );

    D3DPERF_EndEvent();
}

void CDX11Renderer::DrawModels(CModel* model)
{
    D3DPERF_BeginEvent(D3DCOLOR_XRGB(255, 0, 0), L"DRAW_MODELS_FINAL");

    model->m_material->Bind();

    GetDX11Context()->PSSetSamplers(1, 1, &SamplerStates::ShadowSampler);
    ID3D11ShaderResourceView* adasf = m_shadowMap->DepthMapSRV();
    GetDX11Context()->PSSetShaderResources(1, 1, &adasf);

    //////////////////////////
    Material material;
    material.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
    material.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    XMMATRIX objectWorld = model->m_transformation.GetWorld();
    XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

    SPerObjectCBuffer perObject;
    perObject.gWorld             = XMMatrixTranspose(objectWorld);
    perObject.gWorldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(objectWorld),objectWorld));
    perObject.gWorldViewProj     = XMMatrixTranspose( objectWorld * GetCamera().GetView() * GetCamera().GetProj());
    perObject.gTexTransform      = XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f));
    perObject.gMaterial          = material;
    perObject.gShadowTransform   = XMMatrixTranspose(shadowTransform);

    Globals::Get().device.m_context->UpdateSubresource(m_perObjectBuffer, 0, NULL, &perObject, 0, 0);

    /////////////////////////////

    uint offset = 0;
    GetDX11Context()->IASetIndexBuffer(model->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    GetDX11Context()->IASetVertexBuffers(0, 1, &model->m_mesh->m_vertexBuffer, &model->m_mesh->m_stride, &offset);

   // model->m_effect->Bind();

    GetDX11Context()->PSSetConstantBuffers(0, 1, &m_perObjectBuffer);
    GetDX11Context()->VSSetConstantBuffers(0, 1, &m_perObjectBuffer);

    GetDX11Context()->DrawIndexed(model->m_mesh->m_indexCount, 0, 0 );
    D3DPERF_EndEvent();
}


void CDX11Renderer::Draw()
{
    BuildShadowTransform();

//  m_skinnedModelInstance.Update(Globals::Get().app.m_mainTimer.GetDelta());
//  m_testParticleSystem.Update(Globals::Get().app.m_mainTimer.GetDelta(), Globals::Get().app.m_mainTimer.GetTotal());

   /* GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    GetContext()->RSSetState(RenderStates::DepthRS);

    m_shadowMap->BindDsvAndSetNullRenderTarget();

    GetContext()->VSSetShader(m_depthVS, 0, 0);
    GetContext()->PSSetShader(nullptr, 0, 0);

    GetContext()->IASetInputLayout(m_depthInputLayout);


    int modelsCount = Globals::Get().scene.m_models.size();
    for(int i = 0; i < modelsCount; i++)
    {
        DrawModelsToShadowMap(Globals::Get().scene.m_models[i]);
    }
    */
//////////////////////////////////////////////////////////////////

    //GetContext()->RSSetState(m_rasterizerStateCW);

    g_rtwShadowRenderer.Draw();


    GetDX11Context()->RSSetState(0);

    m_device->m_context->RSSetViewports(1, &viewport);

//////////////////////////////////////////////////////////////////


/*
    static float bgColor[4] = {(0.0f, 0.0f, 0.0f, 0.0f)};

    GetContext()->ClearRenderTargetView(m_device->m_renderTargetView, bgColor);
    GetContext()->ClearDepthStencilView(m_device->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    GetContext()->OMSetRenderTargets(1, &m_device->m_renderTargetView, m_device->m_depthStencilView);
  
    GetContext()->OMSetBlendState(0, 0, 0xffffffff);

    GetContext()->RSSetState(m_rasterizerStateCW);

    GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    XMMATRIX lightView       = XMLoadFloat4x4(&mLightView);
    XMMATRIX lightProj       = XMLoadFloat4x4(&mLightProj);
    XMMATRIX lightViewProj   = XMMatrixMultiply(lightView, lightProj);

    SGlobalPerFrameCBuffer perFrame;
    perFrame.startGuard      = XMFLOAT4(7.0f, 7.0f, 7.0f, 7.0f);
    perFrame.view            = XMMatrixTranspose(m_camera.GetView());
    perFrame.proj            = XMMatrixTranspose(m_camera.GetProj());
    perFrame.eyePos          = m_camera.GetPosition();
    perFrame.sunLight        = GetGlobalState().GetSun();
    perFrame.lightViewProj   = XMMatrixTranspose(lightViewProj);
    perFrame.endGuard        = XMFLOAT4(7.0f, 7.0f, 7.0f, 7.0f);

    for(int i = 0; i < Globals::Get().scene.m_models.size(); i++)
    {    
        Globals::Get().scene.m_models[i]->m_shader->Bind();
        //Globals::Get().scene.m_models[i]->m_shader->PerformVertexShaderReflection();

        GetContext()->UpdateSubresource(m_perFrameGlobalCBuffer, 0, NULL, &perFrame, 0, 0);

        GetContext()->PSSetConstantBuffers(1, 1, &m_perFrameGlobalCBuffer);

        DrawModels(Globals::Get().scene.m_models[i]);
    }
    */


///////////////////////////


//    GetContext()->RSSetState(m_noCullRS);


    //if( GetAsyncKeyState('1') & 0x8000 )


    m_debugDrawer.DrawTexture1D( g_rtwShadowRenderer.GetHorizontalWarpTex(), 0 );
    m_debugDrawer.DrawTexture1D( g_rtwShadowRenderer.GetVerticalWarpTex(),   1 );



  //  if( GetAsyncKeyState('2') & 0x8000 )
//        m_debugDrawer.DrawTexture(g_rtwShadowRenderer.GetDepthNormalMap());
/*
    if( GetAsyncKeyState('3') & 0x8000 )
        m_debugDrawer.DrawTexture(g_rtwShadowRenderer.GetImportanceMap());

    if( GetAsyncKeyState('4') & 0x8000 )
        m_debugDrawer.DrawTexture(g_rtwShadowRenderer.GetWarpedShadowMap());

    if( GetAsyncKeyState('5') & 0x8000 )
    {
        showDebug = !showDebug;
    }
    */


//    if(showDebug)
//    {
   // m_debugDrawer.DrawTexture(g_rtwShadowRenderer.GetDepthMap(),        1, 2.2f);
  //  m_debugDrawer.DrawTexture(g_rtwShadowRenderer.GetDepthNormalMap(),  2, 2.2f);


    m_debugDrawer.DrawTexture(g_rtwShadowRenderer.GetWarpedShadowMap(),  4, 2.2f);
    m_debugDrawer.DrawTexture(g_rtwShadowRenderer.GetImportanceMap(),   3, 2.2f);


//}

/*
    D3DPERF_BeginEvent(D3DCOLOR_XRGB(255, 0, 0), L"DRAW_GUI"); 

    GetGui().Draw();

    D3DPERF_EndEvent();
*/


   // Globals::Get().scene.GetTerrain()->Draw();
   // DrawSkinned();
   // DrawParticles();

/*
    const SMouseState& currMouseState = Globals::Get().inputManager.GetMouse().GetCurrentState();

    wchar_t tmpBuf[1244];
    wsprintfW(tmpBuf, L"x: %d y: %d",CApp::mouseX,CApp::mouseY);// currMouseState.absX,  currMouseState.absY);
    DrawString(20, 100, tmpBuf, 0, 0, 255, 255, FontA);
    
    wchar_t tmpBuf2[1244];
    wsprintfW(tmpBuf2, L"x: %d y: %d wheel: %d", currMouseState.absX,  currMouseState.absY, currMouseState.wheel);
    DrawString(20, 140, tmpBuf2, 0, 0, 255, 255, FontA);
*/

    ID3D11ShaderResourceView* nullSRV[16] = { 0 };
    GetDX11Context()->PSSetShaderResources(0, 16, nullSRV);

    m_device->m_swapChain->Present(0, 0);
}







bool CDX11Renderer::InitScene(CDx11Device* device)
{
    m_device = device;

    RasterizerStates::InitAll();
    DepthStencilStates::InitAll();
    BlendStates::InitAll();

    SamplerStates::InitAll();

    m_camera.SetLens(0.25f * 3.14f, 1600.0f / 900.0f, 1.0f, 1000.0f);
    m_camera.SetPosition(0.0f, 4.0f, -15.0f);
    m_camera.UpdateViewMatrix();

    FontA.Initialize(L"Arial", 22.0f, DxFont::STYLE_NORMAL, true);

    D3DSprite.Initialize();

    CreateViewport();


    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage          = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth      = sizeof(SGlobalPerFrameCBuffer);
    cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;
    //  cbbd.MiscFlags      = 0;

    HRESULT hr = Globals::Get().device.m_device->CreateBuffer(&cbbd, NULL, &m_perFrameGlobalCBuffer);
    if(FAILED(hr)) assert(false);

    InitShadowMap();

  //  InitSkinned();

   // InitParticles();

  //  InitGeometry();

    
    m_debugDrawer.Initialize();
    DirectionalLight test;


    GetGlobalState().GetSun().Ambient   = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    GetGlobalState().GetSun().Diffuse   = XMFLOAT4(0.7f, 0.7f, 0.6f, 1.0f);
    GetGlobalState().GetSun().Specular  = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);

    GetGlobalState().GetSun().Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);



    GetGlobalState().GetSceneBounds().Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
    GetGlobalState().GetSceneBounds().Radius = 200;



    g_rtwShadowRenderer.Initialize();


    return true;
}

void CDX11Renderer::CreateViewport()
{
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX  = 0;
    viewport.TopLeftY  = 0;
    viewport.Width     = 1600;
    viewport.Height    = 900;
    viewport.MinDepth  = 0.0f;
    viewport.MaxDepth  = 1.0f;

    m_device->m_context->RSSetViewports(1, &viewport);
}

