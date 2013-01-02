#include "stdafx.h"
#include "terrain.h"

#include "../utils/DDSTextureLoader.h"
#include "statesAndSamplers.h"


CTerrain::CTerrain() : 
    m_quadPatchVB(nullptr), 
    m_quadPatchIB(nullptr), 
    m_layerMapArraySRV(nullptr), 
    m_blendMapSRV(nullptr), 
    m_heightMapSRV(nullptr),
    m_patchVerticesCount(0),
    m_patchQuadFacesCount(0),
    m_patchVertRowsCount(0),
    m_patchVertColsCount(0),
    m_grass(nullptr),
    m_ground(nullptr),
    m_snow(nullptr),
    m_desert(nullptr),
    m_dirt(nullptr)
{

    m_material.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
    m_material.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

CTerrain::~CTerrain()
{
    if(m_quadPatchVB)
        m_quadPatchVB->Release();
    if(m_quadPatchIB)
        m_quadPatchIB->Release();
    if(m_layerMapArraySRV)
        m_layerMapArraySRV->Release();
    if(m_blendMapSRV)
        m_blendMapSRV->Release();
    if(m_heightMapSRV)
        m_heightMapSRV->Release();
}


void CTerrain::Initialize(const InitInfo& initInfo)
{
    m_info = initInfo;

    // Divide heightmap into patches such that each patch has CellsPerPatch.
    m_patchVertRowsCount = ((m_info.HeightmapHeight - 1) / CellsPerPatch) + 1;
    m_patchVertColsCount = ((m_info.HeightmapWidth - 1)  / CellsPerPatch) + 1;

    m_patchVerticesCount  = m_patchVertRowsCount * m_patchVertColsCount;
    m_patchQuadFacesCount = (m_patchVertRowsCount - 1) * (m_patchVertColsCount - 1);

    LoadHeightmap();
    Smooth();
    CalcAllPatchBoundsY();

    BuildQuadPatchVB();
    BuildQuadPatchIB();
    BuildHeightmapSRV();

    HRESULT hr = S_OK;

    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, initInfo.TextureArrayFileName.c_str(),  NULL, &m_layerMapArraySRV);

    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, m_info.BlendMapFilename.c_str(),  NULL, &m_blendMapSRV);

    // just for test until tool for creating texture array with mipmaps will be fixed

    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, L"../assets/textures/terrain/0.dds",  NULL, &m_grass);
    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, L"../assets/textures/terrain/1.dds",  NULL, &m_ground);
    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, L"../assets/textures/terrain/2.dds",  NULL, &m_snow);
    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, L"../assets/textures/terrain/3.dds",  NULL, &m_desert);
    hr = CreateDDSTextureFromFile(Globals::Get().device.m_device, L"../assets/textures/terrain/4.dds",  NULL, &m_dirt);


    m_shader = new CShader();
    m_shader->CreateFromCompiledFile(EInputLayoutType::TerrainLayout, L"../assets/terrain/terrainPS.cso", L"../assets/terrain/terrainVS.cso", nullptr, L"../assets/terrain/terrainDS.cso", L"../assets/terrain/terrainHS.cso");

    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage          = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth      = sizeof(STerrainPerFrameBuffer);
    cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;

    hr = Globals::Get().device.m_device->CreateBuffer(&cbbd, NULL, &m_terrainConstBuffer);
    if(FAILED(hr)) assert(false);
}


void CTerrain::SetConstantBuffer()
{
    DirectionalLight light;
    light.Ambient   = XMFLOAT4(0.3f,    0.3f,   0.3f, 1.0f);
    light.Diffuse   = XMFLOAT4(1.0f,    1.0f,   1.0f, 1.0f);
    light.Specular  = XMFLOAT4(0.8f,    0.8f,   0.7f, 1.0f);

    light.Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);
    
    m_terrainCB.dirLight     = light;
    m_terrainCB.eyePosWorld  = Globals::Get().renderer.m_camera.GetPosition();
    m_terrainCB.pad          = 7777;
    m_terrainCB.fogColor     = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    
    m_terrainCB.texelCellSpaceU = 1.0f / m_info.HeightmapWidth;
    m_terrainCB.texelCellSpaceV = 1.0f / m_info.HeightmapHeight;
    m_terrainCB.worldCellSpace  = m_info.CellSpacing;
    m_terrainCB.pad2            = 8888;
    

    m_material.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
    m_material.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    m_terrainCB.material = m_material;

    m_terrainCB.viewProj = XMMatrixTranspose(Globals::Get().renderer.m_camera.GetView() * Globals::Get().renderer.m_camera.GetProj());


    XMFLOAT4 worldPlanes[6];
    // TODO:

    GetDX11Context()->UpdateSubresource(m_terrainConstBuffer, 0, NULL, &m_terrainCB, 0, 0);

    GetDX11Context()->VSSetConstantBuffers(0, 1, &m_terrainConstBuffer);
    GetDX11Context()->PSSetConstantBuffers(0, 1, &m_terrainConstBuffer);
    GetDX11Context()->HSSetConstantBuffers(0, 1, &m_terrainConstBuffer);
    GetDX11Context()->DSSetConstantBuffers(0, 1, &m_terrainConstBuffer);
}

void CTerrain::SetShadersAndTextures()
{
    m_shader->Bind();

    GetDX11Context()->GSSetShader(NULL, 0, 0);

    ///////
    GetDX11Context()->VSSetSamplers(0, 1,        &SamplerStates::TerrainHeightSampler);
    GetDX11Context()->VSSetShaderResources(0, 1, &m_heightMapSRV);

    GetDX11Context()->PSSetSamplers(0, 1, &SamplerStates::DefaultSamplerWrap);
    GetDX11Context()->PSSetSamplers(1, 1, &SamplerStates::TerrainHeightSampler);
    

    //Globals::Get().device.m_context->PSSetShaderResources(0, 1, &m_layerMapArraySRV);

    GetDX11Context()->PSSetShaderResources(0, 1, &m_blendMapSRV);
    GetDX11Context()->PSSetShaderResources(1, 1, &m_heightMapSRV);

    GetDX11Context()->PSSetShaderResources(2, 1, &m_grass);
    GetDX11Context()->PSSetShaderResources(3, 1, &m_ground);
    GetDX11Context()->PSSetShaderResources(4, 1, &m_snow);
    GetDX11Context()->PSSetShaderResources(5, 1, &m_desert);
    GetDX11Context()->PSSetShaderResources(6, 1, &m_dirt);

    /////////
    GetDX11Context()->DSSetSamplers(0, 1,        &SamplerStates::TerrainHeightSampler);
    GetDX11Context()->DSSetShaderResources(0, 1, &m_heightMapSRV);

}

void CTerrain::Draw()
{
    GetDX11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    SetShadersAndTextures();
    SetConstantBuffer();

    uint stride = sizeof(SVertexTerrain);
    uint offset = 0;

    GetDX11Context()->IASetVertexBuffers(0, 1, &m_quadPatchVB, &stride, &offset);
    GetDX11Context()->IASetIndexBuffer(m_quadPatchIB, DXGI_FORMAT_R16_UINT, 0);

    GetDX11Context()->DrawIndexed(m_patchQuadFacesCount * 4, 0, 0);

    GetDX11Context()->HSSetShader(NULL, 0, 0);
    GetDX11Context()->DSSetShader(NULL, 0, 0);

}

void CTerrain::LoadHeightmap()
{
    std::vector<unsigned char> in(m_info.HeightmapWidth * m_info.HeightmapHeight);
    
    std::ifstream inFile;
    inFile.open(m_info.HeightMapFilename.c_str(), std::ios_base::binary);
    
    if(inFile)
    {
        inFile.read((char*)&in[0], (std::streamsize)in.size());
        inFile.close();
    }
    
    m_heightmap.resize(m_info.HeightmapHeight * m_info.HeightmapWidth, 0);
    for(uint i = 0; i < m_info.HeightmapHeight * m_info.HeightmapWidth; ++i)
    {
        m_heightmap[i] = (in[i] / 255.0f) * m_info.HeightScale;
    }
}

void CTerrain::Smooth()
{
    std::vector<float> dest( m_heightmap.size() );
    
    for(uint i = 0; i < m_info.HeightmapHeight; ++i)
    {
        for(uint j = 0; j < m_info.HeightmapWidth; ++j)
        {
            dest[i * m_info.HeightmapWidth+j] = Average(i,j);
        }
    }

    m_heightmap = dest;
}


bool CTerrain::InBounds(int i, int j)   // True if ij are valid indices; false otherwise.
{
    return i >= 0 && i < (int)m_info.HeightmapHeight &&  j >= 0 && j < (int)m_info.HeightmapWidth;
}

float CTerrain::Average(int i, int j)
{
    float avg = 0.0f;
    float num = 0.0f;
    
    for(int m = i-1; m <= i+1; ++m)
    {
        for(int n = j-1; n <= j+1; ++n)
        {
            if( InBounds(m,n) )
            {
                avg += m_heightmap[m * m_info.HeightmapWidth + n];
                num += 1.0f;
            }
        }
    }
    
    return avg / num;
}

void CTerrain::CalcAllPatchBoundsY()
{
    m_patchBoundsY.resize(m_patchQuadFacesCount);
      
    for(uint i = 0; i < m_patchVertRowsCount-1; ++i) // For each patch
    {
        for(uint j = 0; j < m_patchVertColsCount-1; ++j)
        {
            CalcPatchBoundsY(i, j);
        }
    }
}

void CTerrain::CalcPatchBoundsY(uint i, uint j)
{
    uint x0 = j * CellsPerPatch;
    uint x1 = (j + 1) * CellsPerPatch;
    
    uint y0 = i * CellsPerPatch;
    uint y1 = (i + 1) * CellsPerPatch;
    
    float minY = +Infinity;
    float maxY = -Infinity;

    for(uint y = y0; y <= y1; ++y)
    {
        for(uint x = x0; x <= x1; ++x)
        {
            uint k = y * m_info.HeightmapWidth + x;
            minY = min(minY, m_heightmap[k]);
            maxY = max(maxY, m_heightmap[k]);
        }
    }
    
    uint patchID = i * (m_patchVertColsCount - 1) + j;
    m_patchBoundsY[patchID] = XMFLOAT2(minY, maxY);
}

void CTerrain::BuildQuadPatchVB()
{
    std::vector<SVertexTerrain> patchVertices(m_patchVertRowsCount * m_patchVertColsCount);
    
    float halfWidth = 0.5f * GetWidth();
    float halfDepth = 0.5f * GetDepth();
    
    float patchWidth = GetWidth() / (m_patchVertColsCount - 1);
    float patchDepth = GetDepth() / (m_patchVertRowsCount - 1);
    float du = 1.0f / (m_patchVertColsCount - 1);
    float dv = 1.0f / (m_patchVertRowsCount - 1);
    
    for(uint i = 0; i < m_patchVertRowsCount; ++i)
    {
        float z = halfDepth - i * patchDepth;
        for(uint j = 0; j < m_patchVertColsCount; ++j)
        {
            float x = -halfWidth + j * patchWidth;
            
            patchVertices[i * m_patchVertColsCount+j].Pos = XMFLOAT3(x, 0.0f, z);
            
            // Stretch texture over grid.
            patchVertices[i * m_patchVertColsCount+j].Tex.x = j * du;
            patchVertices[i * m_patchVertColsCount+j].Tex.y = i * dv;
        }
    }
    
    // Store axis-aligned bounding box y-bounds in upper-left patch corner.
    for(uint i = 0; i < m_patchVertRowsCount-1; ++i)
    {
        for(uint j = 0; j < m_patchVertColsCount-1; ++j)
        {
            uint patchID = i * (m_patchVertColsCount - 1)+j;
            patchVertices[i * m_patchVertColsCount+j].BoundsY = m_patchBoundsY[patchID];
        }
    }
    
    D3D11_BUFFER_DESC vbd;
    vbd.Usage               = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth           = sizeof(SVertexTerrain) * patchVertices.size();
    vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags      = 0;
    vbd.MiscFlags           = 0;
    vbd.StructureByteStride = 0;
    
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &patchVertices[0];
    Globals::Get().device.m_device->CreateBuffer(&vbd, &vinitData, &m_quadPatchVB);
}

void CTerrain::BuildQuadPatchIB()
{
    std::vector<USHORT> indices(m_patchQuadFacesCount * 4); // 4 indices per quad face
    
    int k = 0;
    for(uint i = 0; i < m_patchVertRowsCount - 1; ++i)
    {
        for(uint j = 0; j < m_patchVertColsCount - 1; ++j)
        {
            // Top row of 2x2 quad patch
            indices[k]   = i * m_patchVertColsCount + j;
            indices[k+1] = i * m_patchVertColsCount + j + 1;
            
            // Bottom row of 2x2 quad patch
            indices[k+2] = (i + 1) * m_patchVertColsCount + j;
            indices[k+3] = (i + 1) * m_patchVertColsCount + j + 1;
            
            k += 4; // next quad
        }
    }
    
    D3D11_BUFFER_DESC ibd;
    ibd.Usage               = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth           = sizeof(USHORT) * indices.size();
    ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags      = 0;
    ibd.MiscFlags           = 0;
    ibd.StructureByteStride = 0;
    
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    Globals::Get().device.m_device->CreateBuffer(&ibd, &iinitData, &m_quadPatchIB);
}

void CTerrain::BuildHeightmapSRV()
{
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width               = m_info.HeightmapWidth;
    texDesc.Height              = m_info.HeightmapHeight;
    texDesc.MipLevels           = 1;
    texDesc.ArraySize           = 1;
    texDesc.Format              = DXGI_FORMAT_R16_FLOAT;
    texDesc.SampleDesc.Count    = 1;
    texDesc.SampleDesc.Quality  = 0;
    texDesc.Usage               = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags      = 0;
    texDesc.MiscFlags           = 0;
    
    std::vector<HALF> hmap(m_heightmap.size());
    std::transform(m_heightmap.begin(), m_heightmap.end(), hmap.begin(), XMConvertFloatToHalf);
    
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem            = &hmap[0];
    data.SysMemPitch        = m_info.HeightmapWidth * sizeof(HALF);
    data.SysMemSlicePitch   = 0;
    
    ID3D11Texture2D* hmapTex = 0;
    HRESULT hr = Globals::Get().device.m_device->CreateTexture2D(&texDesc, &data, &hmapTex);
    ASSERT_ON_FAIL(hr);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format                    = texDesc.Format;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels       = -1;

    hr = Globals::Get().device.m_device->CreateShaderResourceView(hmapTex, &srvDesc, &m_heightMapSRV);
    ASSERT_ON_FAIL(hr);

    hmapTex->Release();
}


float CTerrain::GetWidth() const
{
    return (m_info.HeightmapWidth - 1) * m_info.CellSpacing;
}

float CTerrain::GetDepth() const
{
    return (m_info.HeightmapHeight - 1) * m_info.CellSpacing;
}

float CTerrain::GetHeight(float x, float z) const
{
    // Transform from terrain local space to "cell" space.
    float c = (x + 0.5f * GetWidth()) /  m_info.CellSpacing;
    float d = (z - 0.5f * GetDepth()) / -m_info.CellSpacing;

    // Get the row and column we are in.
    int row = (int)floorf(d);
    int col = (int)floorf(c);

    // Grab the heights of the cell we are in.
    // A*--*B
    //  | /|
    //  |/ |
    // C*--*D
    float A = m_heightmap[row * m_info.HeightmapWidth + col];
    float B = m_heightmap[row * m_info.HeightmapWidth + col + 1];
    float C = m_heightmap[(row+1) * m_info.HeightmapWidth + col];
    float D = m_heightmap[(row+1) * m_info.HeightmapWidth + col + 1];

    // Where we are relative to the cell.
    float s = c - (float)col;
    float t = d - (float)row;

    // If upper triangle ABC.
    if( s + t <= 1.0f)
    {
        float uy = B - A;
        float vy = C - A;
        return A + s*uy + t*vy;
    }
    else // lower triangle DCB.
    {
        float uy = C - D;
        float vy = B - D;
        return D + (1.0f - s)*uy + (1.0f - t) * vy;
    }
}