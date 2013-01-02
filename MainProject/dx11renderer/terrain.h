#pragma once

#include "lights/light.h"



struct TerrainMaterial
{
    TerrainMaterial() { ZeroMemory(this, sizeof(this)); }
    
    XMFLOAT4 Ambient;
    XMFLOAT4 Diffuse;
    XMFLOAT4 Specular; // w = SpecPower
    XMFLOAT4 Reflect;
};


struct STerrainPerFrameBuffer
{
    DirectionalLight    dirLight;
    XMFLOAT3            eyePosWorld;
    float               pad;
    XMFLOAT4            fogColor;

    float               texelCellSpaceU;
    float               texelCellSpaceV;
    float               worldCellSpace;
    float               pad2;

    XMMATRIX            viewProj;

    XMFLOAT4            worldFrustumPlanes[6];

    TerrainMaterial     material;
};


// unfinished terrain with tesselation based on Frank's Luna implementation from his book
class CTerrain 
{

public:
    struct InitInfo
    {
        std::wstring    HeightMapFilename;
        std::wstring    TextureArrayFileName;
        std::wstring    BlendMapFilename;

        float           HeightScale;
        UINT            HeightmapWidth;
        UINT            HeightmapHeight;
        float           CellSpacing;
    };

public:
    CTerrain();
    ~CTerrain();
    
    void                        Initialize(const InitInfo& initInfo);
    void                        Draw();

    float                       GetWidth() const;
    float                       GetDepth() const;
    float                       GetHeight(float x, float z) const;


private:
    void                        LoadHeightmap();
    void                        Smooth();
    bool                        InBounds(int i, int j);
    float                       Average(int i, int j);

    void                        CalcAllPatchBoundsY();
    void                        CalcPatchBoundsY(UINT i, UINT j);

    void                        BuildQuadPatchVB();
    void                        BuildQuadPatchIB();
    void                        BuildHeightmapSRV();


    void                        SetConstantBuffer();
    void                        SetShadersAndTextures();

private:
    static const int            CellsPerPatch = 64;
    
    ID3D11Buffer*               m_quadPatchVB;
    ID3D11Buffer*               m_quadPatchIB;
    
    ID3D11ShaderResourceView*   m_layerMapArraySRV;
    ID3D11ShaderResourceView*   m_blendMapSRV;
    ID3D11ShaderResourceView*   m_heightMapSRV;
    
    ////////////// test to run without texture array

    ID3D11ShaderResourceView*   m_grass;
    ID3D11ShaderResourceView*   m_ground;
    ID3D11ShaderResourceView*   m_snow;
    ID3D11ShaderResourceView*   m_desert;
    ID3D11ShaderResourceView*   m_dirt;

    //////////////

    InitInfo                    m_info;
    
    UINT                        m_patchVerticesCount;
    UINT                        m_patchQuadFacesCount;
    
    UINT                        m_patchVertRowsCount;
    UINT                        m_patchVertColsCount;
    
    
    TerrainMaterial             m_material;
    
    std::vector<XMFLOAT2>       m_patchBoundsY;
    std::vector<float>          m_heightmap;

    STerrainPerFrameBuffer      m_terrainCB;

    ID3D11Buffer*               m_terrainConstBuffer;

    ID3D11SamplerState*         m_heightSampler;

    CShader*                    m_shader;
};