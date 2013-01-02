#include "stdafx.h"
#include "mesh.h"



CMesh::CMesh()
{

}

CMesh::~CMesh()
{
}

void CMesh::CreateFromMeshData(const SMeshData* meshData)
{
    HRESULT hr = S_OK;

    m_indexCount  = meshData->indicesCount;
    m_vertexCount = meshData->verticesCount;
    m_stride      = meshData->size;

    /////////////// create index buffer //////
    D3D11_BUFFER_DESC ibDesc;
    ZeroMemory(&ibDesc, sizeof(ibDesc));

    ibDesc.Usage          = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth      = sizeof(unsigned int) * m_indexCount;
    ibDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags      = 0;

    D3D11_SUBRESOURCE_DATA ibData;
    ibData.pSysMem = meshData->indices;

    hr = Globals::Get().device.m_device->CreateBuffer(&ibDesc, &ibData, &m_indexBuffer);
    if(FAILED(hr)) assert(false);

    //////////// create vertex buffer ////////
    D3D11_BUFFER_DESC vbDesc;
    ZeroMemory(&vbDesc, sizeof(vbDesc));

    vbDesc.Usage          = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth      = meshData->size * m_vertexCount;
    vbDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.MiscFlags      = 0;
   
    D3D11_SUBRESOURCE_DATA vbData; 

    ZeroMemory(&vbData, sizeof(vbData));

    switch(meshData->type)
    {
        case EInputLayoutType::Position:
            vbData.pSysMem = meshData->vertexData.vertexPos;
        break;
        case EInputLayoutType::PositionTexture:
            vbData.pSysMem = meshData->vertexData.vertexPosTex;
        break;
        case EInputLayoutType::PositionTextureNormal:
            vbData.pSysMem = meshData->vertexData.vertexPosTexNorm;
        break;
        case EInputLayoutType::PositionTextureNormalTangentBinormal:
            vbData.pSysMem = meshData->vertexData.vertexPosTexNormBitangentTangent;
        break;
        default:
            assert(false);
    };

    hr = Globals::Get().device.m_device->CreateBuffer(&vbDesc, &vbData, &m_vertexBuffer);
    if(FAILED(hr)) assert(false);

}