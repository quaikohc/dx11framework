#pragma once







class CMesh 
{

public:
    CMesh();
    ~CMesh();

    void               CreateFromMeshData(const SMeshData* meshData);

public:
    uint               m_indexCount;
    uint               m_vertexCount;

    ID3D11Buffer*      m_vertexBuffer;
    ID3D11Buffer*      m_indexBuffer;

    uint               m_stride;
};