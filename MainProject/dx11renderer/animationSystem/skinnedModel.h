#pragma once

#include "../../globals.h"
#include "../../utils/math/math.h"
#include "lights/light.h"

#include "assimpAnimationSystem.h"
#include "../shader.h"



struct Material
{
    Material() { ZeroMemory(this, sizeof(this)); }

    XMFLOAT4 Ambient;
    XMFLOAT4 Diffuse;
    XMFLOAT4 Specular; // w = SpecPower
    XMFLOAT4 Reflect;
};

struct M3dMaterial
{
    Material        Mat;
    bool            AlphaClip;
    std::string     EffectTypeName;
    std::wstring    DiffuseMapName;
    std::wstring    NormalMapName;
};

struct SSkinnedPerFrameBuffer
{
    XMFLOAT4            startValuePerFrame;
    DirectionalLight    gDirLight;
    XMFLOAT3            gEyePosW;
    float               pad;
    XMFLOAT4            endValuePerFrame;
};

struct SSkinnedPerObjectBuffer
{
    XMFLOAT4            startValuePerObject;
    XMMATRIX            world;
    XMMATRIX            worldInvTranspose;
    XMMATRIX            worldViewProj;
    XMMATRIX            worldViewProjTex;
    XMMATRIX            texTransform;
    XMFLOAT4X4          boneTransforms[96];
    Material            material;
    XMFLOAT4            endValuePerObject;
};


class SkinnedModel
{
    bool InitShaders();

public:
    SkinnedModel(const std::string& modelFilename);
    ~SkinnedModel();

    struct Subset
    {
        Subset() : Id(-1),  VertexStart(0), VertexCount(0), FaceStart(0), FaceCount(0)
        {
        }

        uint Id;
        uint VertexStart;
        uint VertexCount;
        uint FaceStart;
        uint FaceCount;
    };

    void                                         SetVertices(const SVertexPosNormalTexTanSkinned* vertices, uint count);
    void                                         SetIndices(const USHORT* indices, uint count);
    void                                         SetSubsetTable(std::vector<Subset>& subsetTable);
    void                                         Draw(uint subsetId);
                                                 
    CShader*                                     m_shader;
    CAnimationSystem                             m_testAssimpLoader;

    uint                                         SubsetCount;
    
    std::vector<ID3D11ShaderResourceView*>       m_diffuseTextures;
    std::vector<ID3D11ShaderResourceView*>       m_normalTextures;
    
    // prbly there is no need to keep this after initialization
    std::vector<SVertexPosNormalTexTanSkinned>   m_verticesData;
    std::vector<USHORT>                          m_indicesData;
    std::vector<Subset>                          m_subsetsData;
    
    ID3D11VertexShader*                          m_vs;
    ID3D11PixelShader*                           m_ps;
    ID3D11Buffer*                                m_skinnedPerFrameBuffer;
    ID3D11Buffer*                                m_skinnedPerObjectBuffer;
                                                 
    ID3D11InputLayout*                           m_inputLayout;

    ID3D11Buffer*                                m_vb;
    ID3D11Buffer*                                m_ib;

    uint                                         m_stride;

};

struct SkinnedModelInstance
{
    SkinnedModel*               Model;
    float                       TimePos;
    std::string                 ClipName;

    std::vector<XMFLOAT4X4>     FinalTransforms;
    
    void                        Update(float dt);
    void                        Draw();
};
