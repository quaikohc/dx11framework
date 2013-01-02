#pragma once





class CMaterial
{
    XMFLOAT4    Ambient;
    XMFLOAT4    Diffuse;
    XMFLOAT4    Specular; // w = SpecPower
    XMFLOAT4    Reflect;

public:
    CMaterial();
    ~CMaterial();

    bool        Initialize(const wchar_t* diffusePath, const wchar_t* normalPath, const wchar_t* specularPath, const wchar_t* heightPath);
    void        Bind();

    ID3D11ShaderResourceView*       m_diffuseRV;
    ID3D11ShaderResourceView*       m_specularRV;
    ID3D11ShaderResourceView*       m_normalRV;
    ID3D11ShaderResourceView*       m_heightRV;

    EMaterialFlag                   m_flags;
};