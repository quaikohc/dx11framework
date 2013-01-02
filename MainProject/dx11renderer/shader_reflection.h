#pragma once





class CShaderReflection
{


public:
    CShaderReflection();
    ~CShaderReflection();

    void    Initialize(ID3DBlob* shaderBlob);
    void    ReflectLayouts(ID3D11ShaderReflection* reflection, D3D11_SHADER_DESC& shaderDesc);
    void    ReflectBuffers(ID3D11ShaderReflection* reflection, D3D11_SHADER_DESC& shaderDesc);
    void    ReflectResources(ID3D11ShaderReflection* reflection, D3D11_SHADER_DESC& shaderDesc);


};