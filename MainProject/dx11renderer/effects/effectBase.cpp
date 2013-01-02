#include "stdafx.h"
#include "effectBase.h"
#include "../../globals.h"
#include "../model.h"




CEffect::CEffect(int cbufferStructSize)
{
   /* D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage          = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth      = cbufferStructSize;
    cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;

    HRESULT hr = Globals::Get().device.m_device->CreateBuffer(&cbbd, NULL, &m_constBuffer);
    if(FAILED(hr)) assert(false);*/

}

///////////////////////////////////////////////////////

        //XMMATRIX      gShadowTransform; 
        //Material      gMaterial;

void CParallaxEffect::Update()
{
  /*  Material material;
    material.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
    material.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    XMMATRIX objectWorld = m_model->m_transformation.GetWorld();
    XMMATRIX shadowTransform = XMMatrixIdentity();
    XMStoreFloat4x4(&Globals::Get().renderer.GetShadowTransform(), shadowTransform);

    perObject perObject;
    perObject.gWorld             = XMMatrixTranspose(objectWorld);
    perObject.gWorldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(objectWorld),objectWorld));
    perObject.gWorldViewProj     = XMMatrixTranspose( objectWorld * GetCamera().GetView() * GetCamera().GetProj());
    perObject.gTexTransform      = XMMatrixTranspose(XMMatrixScaling(2.0f, 1.0f, 1.0f));
    perObject.gMaterial          = material;
    perObject.gShadowTransform   = XMMatrixTranspose(shadowTransform);

    Globals::Get().device.m_context->UpdateSubresource(m_constBuffer, 0, NULL, &perObject, 0, 0);*/
}

void CParallaxEffect::Bind()
{
  //  Globals::Get().device.m_context->PSSetConstantBuffers(1, 1, &m_constBuffer);
  //  Globals::Get().device.m_context->VSSetConstantBuffers(1, 1, &m_constBuffer);
}

///////////////////////////////////////////////////////

void CTexturedEffect::Update()
{
    perObject perObject;
    perObject.world  = XMMatrixTranspose(m_model->m_transformation.GetWorld());

    GetDX11Context()->UpdateSubresource(m_constBuffer, 0, NULL, &perObject, 0, 0);
}

void CTexturedEffect::Bind()
{
    GetDX11Context()->VSSetConstantBuffers(1, 1, &m_constBuffer);
}

///////////////////////////////////////////////////////
