#pragma once

#include "../dx11renderer/dx11device.h"
#include "../dx11renderer/dx11renderer.h"
#include "../dx11renderer/terrain.h"
#include "../utils/geometry_generator.h"
#include "visibility_system.h"


class CAssimpLoader;


class CScene
{
    CAssimpLoader*          m_assimpLoader ;
    CTerrain*               m_terrain;

public:
    CScene();
    ~CScene();

    bool                    LoadScene();
    void                    InitializeScene();
    void                    InitializeTerrain();

    ///////// omfg: big fake - rewrite it asap
    bool                    CreateModel(const std::wstring& shaderFileName, XMFLOAT3 pos, XMFLOAT3 rot, MeshData& meshData);
    CModel*                     CreateModelParallax(const std::wstring& shaderFileName,  XMFLOAT3 pos, XMFLOAT3 rot, MeshData& geomData);

    bool                    LoadModel(const std::wstring& fileName, const std::wstring& shaderFileName, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f));

    void                    Update();
    void                    Draw();

    CTerrain*               GetTerrain();

    CVisibilitySystem       m_visibilitySystem;

    std::vector<CModel*>    m_models;
};