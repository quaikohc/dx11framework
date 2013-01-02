#include "stdafx.h"
#include "debugDrawer.h"
#include "../utils/geometry_generator.h"
#include "inputLayouts.h"
#include "statesAndSamplers.h"


CDebugDrawer::CDebugDrawer() :
m_shader(nullptr),
m_shader1D(nullptr),
m_vb(nullptr),
m_ib(nullptr),
m_cBuffer(nullptr)
{

}

CDebugDrawer::~CDebugDrawer()
{
}

void CDebugDrawer::Initialize()
{
    BuildGeometry();

    m_shader = new CShader();
    m_shader1D= new CShader();

    m_shader->CreateFromCompiledFile(EInputLayoutType::PositionTexture, L"../assets/textureDebugPS.cso", L"../assets/textureDebugVS.cso");
    m_shader1D->CreateFromCompiledFile(EInputLayoutType::PositionTexture, L"../assets/textureDebug1DPS.cso", L"../assets/textureDebugVS.cso");
    
    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage          = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth      = sizeof(SDebugCBuffer);
    cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;

    GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_cBuffer);

}

void CDebugDrawer::DrawTexture(ID3D11ShaderResourceView* texture, uint slot, float scale)
{
    D3DPERF_BeginEvent(D3DCOLOR_XRGB( 255, 0, 0  ),  L"DRAW_DEBUG_TEXTURE_2D");


    UINT stride = sizeof(SVertexPosTex);
    UINT offset = 0;

    SDebugCBuffer cBuffer;
    XMMATRIX VP = GetCamera().GetView() * GetCamera().GetProj();

    m_shader->Bind();

    GetDX11Context()->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);
    GetDX11Context()->IASetIndexBuffer(m_ib, DXGI_FORMAT_R32_UINT, 0);

    /*XMMATRIX world(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, scale);
    */
    // Scale and shift quad to lower-right corner.
    XMMATRIX world(
    0.5f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    -3.0f + slot * 1.2f, -1.5f, 0.0f, scale);
    

    SDebugCBuffer cbuffer;
    cbuffer.worldViewProj = XMMatrixTranspose(world);

    GetDX11Context()->UpdateSubresource(m_cBuffer, 0, NULL, &cbuffer, 0, 0);

    GetDX11Context()->VSSetConstantBuffers(0, 1, &m_cBuffer);

    GetDX11Context()->PSSetShaderResources(0, 1, &texture);
    GetDX11Context()->PSSetSamplers(0, 1,        &SamplerStates::DefaultSamplerClamp);

    GetDX11Context()->DrawIndexed(6, 0, 0);


    D3DPERF_EndEvent();
}



void CDebugDrawer::DrawTexture1D(ID3D11ShaderResourceView* texture, int slot)
{
    D3DPERF_BeginEvent(D3DCOLOR_XRGB( 255, 0, 0  ),  L"DRAW_DEBUG_TEXTURE_1D");

    UINT stride = sizeof(SVertexPosTex);
    UINT offset = 0;

    SDebugCBuffer cBuffer;
    XMMATRIX VP = GetCamera().GetView() * GetCamera().GetProj();

    m_shader1D->Bind();

    GetDX11Context()->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);
    GetDX11Context()->IASetIndexBuffer(m_ib, DXGI_FORMAT_R32_UINT, 0);

    
    SDebugCBuffer cbuffer;
    cbuffer.worldViewProj = XMMatrixTranspose(XMMatrixScaling(0.9f, 0.05f, 0.5f) * XMMatrixTranslation(0.0f, 0.9f - 0.12f * float(slot), 0.0f));

    GetDX11Context()->UpdateSubresource(m_cBuffer, 0, NULL, &cbuffer, 0, 0);

    GetDX11Context()->VSSetConstantBuffers(0, 1, &m_cBuffer);

    GetDX11Context()->PSSetShaderResources(0, 1, &texture);
    GetDX11Context()->PSSetSamplers(0, 1,        &SamplerStates::DefaultSamplerClamp);

    GetDX11Context()->DrawIndexed(6, 0, 0);

    D3DPERF_EndEvent();
}



void CDebugDrawer::BuildGeometry()
{
    HRESULT hr = S_OK;

    MeshData quad;

    GeometryGenerator geoGen;
    geoGen.CreateFullscreenQuad(quad);

    std::vector<SVertexPosTex> vertices(quad.Vertices.size());

    for(UINT i = 0; i < quad.Vertices.size(); ++i)
    {
        vertices[i].pos    = quad.Vertices[i].Position;
        vertices[i].tex    = quad.Vertices[i].TexC;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage           = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth       = sizeof(SVertexPosTex) * quad.Vertices.size();
    vbd.BindFlags       = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags  = 0;
    vbd.MiscFlags       = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    hr = GetDX11Device()->CreateBuffer(&vbd, &vinitData, &m_vb);

    D3D11_BUFFER_DESC ibd;
    ibd.Usage           = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth       = sizeof(UINT) * quad.Indices.size();
    ibd.BindFlags       = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags  = 0;
    ibd.MiscFlags       = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &quad.Indices[0];
    hr = GetDX11Device()->CreateBuffer(&ibd, &iinitData, &m_ib);
}

