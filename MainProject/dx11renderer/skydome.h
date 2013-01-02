#pragma once


class CSky
{
    ID3D11VertexShader*         m_skyVS;
    ID3D11PixelShader*          m_skyPS;
    ID3D11InputLayout*          m_inputLayout;
    ID3D11Buffer*               m_skyPSConstBuffer;
    ID3D11Buffer*               m_skyVSConstBuffer;

    uint                        m_indexCount;
    uint                        m_vertexCount;

    ID3D11Buffer*               m_vertexBuffer;
    ID3D11Buffer*               m_indexBuffer;

    int                         NumSphereVertices;
    int                         NumSphereFaces;

    XMMATRIX Rotationx;
    XMMATRIX Rotationy;
    XMMATRIX Rotationz;

public:
    CSky();
    ~CSky();

    bool                        Initialize();

    HRESULT                     CreateSphere(int LatLines, int LongLines);


};