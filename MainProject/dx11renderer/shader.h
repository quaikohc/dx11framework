#pragma once

// TODO: make CShader template class and implement CShaderGroup to support handling multiple shaders together

class CShaderGroup
{

public:
    CShaderGroup(){};
    ~CShaderGroup(){};

};

class CShader
{
    ID3DBlob*               m_vsBlob;
    ID3DBlob*               m_psBlob;

    ID3D11VertexShader*     m_vs;
    ID3D11PixelShader*      m_ps;
    ID3D11GeometryShader*   m_gs;
    ID3D11HullShader*       m_hs;
    ID3D11DomainShader*     m_ds;

    ID3D11InputLayout*      m_inputLayout;

public:
    CShader();
    ~CShader();

    bool CompileFromFile(EInputLayoutType type, const wchar* psPath, const wchar* vsPath, const wchar* gsPath = nullptr);
    bool CreateFromCompiledFile(EInputLayoutType type, const wchar* psPath, const wchar* vsPath, const wchar* gsPath = nullptr, const wchar* dsPath = nullptr, const wchar* hsPath = nullptr);

    void Bind();

    void PerformVertexShaderReflection();

};