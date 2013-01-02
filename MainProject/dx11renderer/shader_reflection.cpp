#include "stdafx.h"
#include "shader_reflection.h"


CShaderReflection::CShaderReflection()
{

}

CShaderReflection::~CShaderReflection()
{

}

struct SGeneralShaderDesc
{
    uint cbuffersCount;
    uint resourcesCount;
    uint inputParamsCount;
    uint outputParamsCount;
};
SGeneralShaderDesc generalShaderDesc;

struct SCBufferDesc
{
    char    name[32];
    uint    size;
    uint    variablesCount;

};
SCBufferDesc cBufferDesc;

struct SVariableDesc
{
    char    name[32];
    char    typeName[32];
    uint    startOffset;
    uint    size; 

};

SVariableDesc var;

void CShaderReflection::ReflectLayouts(ID3D11ShaderReflection* reflection, D3D11_SHADER_DESC& shaderDesc)
{
    // input parameters (layout) and output parameters
    for (uint i = 0; i < shaderDesc.InputParameters; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC inputParamDesc;      
        reflection->GetInputParameterDesc(i, &inputParamDesc);
    }

    for (uint i = 0; i < shaderDesc.OutputParameters; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC outputParamDesc;
        reflection->GetOutputParameterDesc(i, &outputParamDesc);
    }
}

void CShaderReflection::ReflectBuffers(ID3D11ShaderReflection* reflection, D3D11_SHADER_DESC& shaderDesc)
{

    for (uint i = 0; i < shaderDesc.ConstantBuffers; i++)   // cbuffers
    {
        ID3D11ShaderReflectionConstantBuffer* constBuffer = reflection->GetConstantBufferByIndex(i);

        D3D11_SHADER_BUFFER_DESC bufferDesc;
        constBuffer->GetDesc(&bufferDesc);

        strcpy(cBufferDesc.name, bufferDesc.Name);
        cBufferDesc.size           = bufferDesc.Size;
        cBufferDesc.variablesCount = bufferDesc.Variables;

        if(bufferDesc.Type == D3D_CT_CBUFFER || bufferDesc.Type == D3D_CT_TBUFFER) // const buffer or texture buffer
        {

            for(UINT j = 0; j < bufferDesc.Variables; j++)
            {
                ID3D11ShaderReflectionVariable* variable = constBuffer->GetVariableByIndex(j);
                D3D11_SHADER_VARIABLE_DESC varDesc;
                variable->GetDesc(&varDesc);

                strcpy(var.name, varDesc.Name);
                var.size = varDesc.Size;
                var.startOffset = varDesc.StartOffset;

                ID3D11ShaderReflectionType* type = variable->GetType();
                D3D11_SHADER_TYPE_DESC typeDesc;
                type->GetDesc(&typeDesc);


            }
        }
    }
}

void CShaderReflection::ReflectResources(ID3D11ShaderReflection* reflection, D3D11_SHADER_DESC& shaderDesc)
{
    // resources
    for (uint i = 0; i < shaderDesc.BoundResources; i++)
    {
        D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
        reflection->GetResourceBindingDesc(i, &resourceDesc);

        int result = 0;
        if ( resourceDesc.Type == D3D_SIT_CBUFFER || resourceDesc.Type == D3D_SIT_TBUFFER )
        {
            result+= 12;
        }
        else if ( resourceDesc.Type == D3D_SIT_TEXTURE || resourceDesc.Type == D3D_SIT_STRUCTURED )
        {
              result+= 34;
        }
        else if ( resourceDesc.Type == D3D_SIT_SAMPLER )
        {
              result+= 65;
        }
        else if (  resourceDesc.Type == D3D_SIT_UAV_RWTYPED || resourceDesc.Type == D3D_SIT_UAV_RWSTRUCTURED
            || resourceDesc.Type == D3D_SIT_BYTEADDRESS || resourceDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS
            || resourceDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED || resourceDesc.Type == D3D_SIT_UAV_CONSUME_STRUCTURED
            || resourceDesc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER )
        {
              result+= 56;
        }
        else
        {
              result+= 16544564;
        }
    }
}

void  CShaderReflection::Initialize(ID3DBlob* shaderBlob)
{
    ID3D11ShaderReflection* reflection = NULL;
    D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);

     // general shader info
    D3D11_SHADER_DESC shaderDesc;  
    reflection->GetDesc(&shaderDesc);

    generalShaderDesc.cbuffersCount      = shaderDesc.ConstantBuffers;
    generalShaderDesc.resourcesCount     = shaderDesc.BoundResources;
    generalShaderDesc.inputParamsCount   = shaderDesc.InputParameters;
    generalShaderDesc.outputParamsCount  = shaderDesc.OutputParameters;

    ReflectLayouts(reflection, shaderDesc);
    
    ReflectBuffers(reflection, shaderDesc);

    ReflectResources(reflection, shaderDesc);



    reflection->Release();
}

