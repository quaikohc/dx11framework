#include "stdafx.h"
#include "skinnedModel.h"

#include "../../utils/DDSTextureLoader.h"
#include "../material.h"
#include "assimpAnimationLoader.h"
#include "../statesAndSamplers.h"

SkinnedModel::SkinnedModel(const std::string& modelFilename) : m_vb(0), m_ib(0), m_stride(0)
{
    std::vector<M3dMaterial> mats;

    Importer m_assimpImporter;

    const aiScene*  aiScene = m_assimpImporter.ReadFile(modelFilename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    CAssimpAnimationLoader m_assimpAnimLoader;
    m_assimpAnimLoader.Load(aiScene, m_verticesData, m_indicesData, m_subsetsData, mats);

    m_testAssimpLoader.Initialize(aiScene);

    SetVertices(&m_verticesData[0], m_verticesData.size());
    SetIndices(&m_indicesData[0], m_indicesData.size());
    SetSubsetTable(m_subsetsData);
    

    /// TEST 

    HRESULT hr = S_OK;
    ID3D11ShaderResourceView* diffuseMapSRV;
    std::wstring diffusePath = L"..\\assets\\textures\\leo_d.dds";
    hr = CreateDDSTextureFromFile(GetDX11Device(), diffusePath.c_str(),  NULL, &diffuseMapSRV);
    if(FAILED(hr)) assert(false);
    m_diffuseTextures.push_back(diffuseMapSRV);

    ID3D11ShaderResourceView* normalMapSRV;
    std::wstring normalPath = L"..\\assets\\textures\\leo_n.dds";
    hr = CreateDDSTextureFromFile(GetDX11Device(), normalPath.c_str(),  NULL, &normalMapSRV);
    if(FAILED(hr)) assert(false);
    m_normalTextures.push_back(normalMapSRV);

    InitShaders();
}

SkinnedModel::~SkinnedModel()
{
    if(m_vb)
        m_vb->Release();
    
    if(m_shader)
        delete m_shader;

    m_skinnedPerFrameBuffer->Release();
    m_skinnedPerObjectBuffer->Release();
    m_ib->Release();
}

void SkinnedModelInstance::Draw()
{
    Model->m_shader->Bind();

    SSkinnedPerFrameBuffer tmpPerFrameBuffer;

    DirectionalLight light;
    light.Ambient   = XMFLOAT4(1.3f,    0.3f,   0.3f, 1.0f);
    light.Diffuse   = XMFLOAT4(1.0f,    1.0f,   1.0f, 1.0f);
    light.Specular  = XMFLOAT4(0.8f,    0.8f,   0.7f, 1.0f);
    light.Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);
//    light.Pad       = 5;

    tmpPerFrameBuffer.startValuePerFrame = XMFLOAT4(3.3f, 3.3f,3.3f, 3.3f);
    tmpPerFrameBuffer.gEyePosW           = XMFLOAT3(0.3f, 0.4f, 0.5f);
    tmpPerFrameBuffer.gDirLight          = light;
    tmpPerFrameBuffer.pad                = 77;
    tmpPerFrameBuffer.endValuePerFrame   = XMFLOAT4(4.4f, 4.4f, 4.4f, 4.4f);

    GetDX11Context()->UpdateSubresource(Model->m_skinnedPerFrameBuffer, 0, NULL, &tmpPerFrameBuffer, 0, 0);

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    XMMATRIX toTexSpace(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    Material mat;
    mat.Ambient  = XMFLOAT4(6.0f, 6.0f, 6.0f, 6.0f);
    mat.Diffuse  = XMFLOAT4(7.0f, 7.0f, 7.0f, 7.0f);
    mat.Specular = XMFLOAT4(8.0f, 8.0f, 8.0f, 8.0f);
    mat.Reflect  = XMFLOAT4(9.0f, 9.0f, 9.0f, 9.0f);

    SSkinnedPerObjectBuffer tmpPerObject;
    tmpPerObject.startValuePerObject = XMFLOAT4(9.0f, 9.0f, 9.0f, 9.0f);
    tmpPerObject.world               = XMMatrixTranspose(XMMatrixIdentity());
    tmpPerObject.worldInvTranspose   = XMMatrixTranspose(XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(tmpPerObject.world), tmpPerObject.world)));     //tmpPerObject.gWorldInvTranspose = InverseTranspose(tmpPerObject.gWorld);
    tmpPerObject.worldViewProj       = XMMatrixTranspose(tmpPerObject.world * Globals::Get().renderer.m_camera.GetView() * Globals::Get().renderer.m_camera.GetProj());
    tmpPerObject.worldViewProjTex    = XMMatrixTranspose(tmpPerObject.world * Globals::Get().renderer.m_camera.GetView() * Globals::Get().renderer.m_camera.GetProj() * toTexSpace);
    tmpPerObject.texTransform        = XMMatrixScaling(1.0f, 1.0f, 1.0f);

    for(int i = 0; i < 40; i++) //96
    {
        XMMATRIX tmp = XMLoadFloat4x4(&FinalTransforms[i]);
        tmp = XMMatrixTranspose(tmp);
        XMStoreFloat4x4(&FinalTransforms[i], tmp);

        tmpPerObject.boneTransforms[i] = FinalTransforms[i];
    }

    tmpPerObject.material            = mat;

    tmpPerObject.endValuePerObject   = XMFLOAT4(9.0f, 9.0f, 9.0f, 9.0f);

    GetDX11Context()->UpdateSubresource(Model->m_skinnedPerObjectBuffer, 0, NULL, &tmpPerObject, 0, 0);

    GetDX11Context()->VSSetConstantBuffers(0, 1, &Model->m_skinnedPerObjectBuffer);
    GetDX11Context()->PSSetConstantBuffers(0, 1, &Model->m_skinnedPerObjectBuffer);

    GetDX11Context()->VSSetConstantBuffers(1, 1, &Model->m_skinnedPerFrameBuffer);
    GetDX11Context()->PSSetConstantBuffers(1, 1, &Model->m_skinnedPerFrameBuffer);

    //for(UINT subset = 0; subset < Model->SubsetCount; ++subset)
    {
        GetDX11Context()->PSSetSamplers(0, 1, &SamplerStates::DefaultSamplerClamp);
        GetDX11Context()->PSSetShaderResources(0, 1, &Model->m_diffuseTextures[0]);
        GetDX11Context()->PSSetShaderResources(1, 1, &Model->m_normalTextures[0]);

        Model->Draw(0);
    }
}

bool SkinnedModel::InitShaders()
{
    HRESULT hr = S_OK;

    m_shader = new CShader();

    m_shader->CreateFromCompiledFile(EInputLayoutType::PositionTextureNormalTangentSkinned, L"../assets/skinnedPS.cso", L"../assets/skinnedVS.cso");

    D3D11_BUFFER_DESC cbbd;

    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage          = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth      = sizeof(SSkinnedPerFrameBuffer);
    cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;

    hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_skinnedPerFrameBuffer);
    if(FAILED(hr)) assert(false);


    D3D11_BUFFER_DESC cbbd2;
    ZeroMemory(&cbbd2, sizeof(D3D11_BUFFER_DESC));

    cbbd2.Usage          = D3D11_USAGE_DEFAULT;
    cbbd2.ByteWidth      = sizeof(SSkinnedPerObjectBuffer);
    cbbd2.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd2.CPUAccessFlags = 0;

     hr = GetDX11Device()->CreateBuffer(&cbbd2, NULL, &m_skinnedPerObjectBuffer);
    if(FAILED(hr)) assert(false);

    return true;
}

void SkinnedModelInstance::Update(float dt)
{
    FinalTransforms = Model->m_testAssimpLoader.GetTransforms(TimePos);
    TimePos += dt;

    if(TimePos >22.0f)
        TimePos = 0.0f;
}

void SkinnedModel::SetVertices(const SVertexPosNormalTexTanSkinned* vertices, UINT count)
{
    m_stride = sizeof(SVertexPosNormalTexTanSkinned);

    D3D11_BUFFER_DESC vbd;
    vbd.Usage               = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth           = sizeof(SVertexPosNormalTexTanSkinned) * count;
    vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags      = 0;
    vbd.MiscFlags           = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

    GetDX11Device()->CreateBuffer(&vbd, &vinitData, &m_vb);
}

void SkinnedModel::SetIndices(const USHORT* indices, UINT count)
{
    D3D11_BUFFER_DESC ibd;
    ibd.Usage               = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth           = sizeof(USHORT) * count;
    ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags      = 0;
    ibd.MiscFlags           = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    GetDX11Device()->CreateBuffer(&ibd, &iinitData, &m_ib);
}

void SkinnedModel::SetSubsetTable(std::vector<Subset>& subsetTable)
{
    m_subsetsData = subsetTable;
}

void SkinnedModel::Draw(UINT subsetId)
{
    UINT offset = 0;

    GetDX11Context()->IASetVertexBuffers(0, 1, &m_vb, &m_stride, &offset);
    GetDX11Context()->IASetIndexBuffer(m_ib, DXGI_FORMAT_R16_UINT, 0);

    GetDX11Context()->DrawIndexed(m_subsetsData[subsetId].FaceCount * 3,  m_subsetsData[subsetId].FaceStart * 3, 0);
}
