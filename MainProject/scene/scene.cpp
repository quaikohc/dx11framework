#include "stdafx.h"
#include "scene.h"
#include "../dx11renderer/mesh.h"
#include "../dx11renderer/model.h"
#include "../dx11renderer/material.h"
#include "../utils/assimp_loader.h"

#include "../utils/filesystem/xmlFile.h"

CScene::CScene()
{

}

CScene::~CScene()
{
}

void CScene::Update()
{


}

void CScene::Draw()
{

}

bool CScene::CreateModel(const std::wstring& shaderFileName,  XMFLOAT3 pos, XMFLOAT3 rot, MeshData& geomData)
{
    CModel* m_model = new(16) CModel();
    m_model->Initialize(EEffectType::EBasicTextured);

    /// TEST
    m_model->m_shader = new CShader();
    m_model->m_shader->CreateFromCompiledFile(EInputLayoutType::PositionTextureNormal, L"../assets/basicTexturedPS.cso", L"../assets/basicTexturedVS.cso");

    m_model->m_material = new CMaterial();
    m_model->m_material->Initialize(L"c:\\__repository\\box_d.dds", L"c:\\__repository\\box_s.dds", L"c:\\__repository\\box_n.dds", L"c:\\__repository\\box_h.dds");

    /////////////////////////////////////////////
    SMeshData meshData;
    meshData.type = EInputLayoutType::PositionTextureNormal;

    meshData.indicesCount  = geomData.Indices.size();
    meshData.verticesCount = geomData.Vertices.size();

    meshData.vertexData.vertexPosTexNorm = new SVertexPosTexNorm[meshData.verticesCount];
    meshData.size = sizeof(SVertexPosTexNorm);

    uint k = 0;
    for(size_t i = 0; i < meshData.verticesCount; ++i, ++k)
    {
        meshData.vertexData.vertexPosTexNorm[i].pos    = geomData.Vertices[i].Position;
        meshData.vertexData.vertexPosTexNorm[i].tex    = geomData.Vertices[i].TexC;
        meshData.vertexData.vertexPosTexNorm[i].normal = geomData.Vertices[i].Normal;
    }

    meshData.indices = new uint[meshData.indicesCount];

    for(int i = 0; i < meshData.indicesCount; i++)
        meshData.indices[i] = geomData.Indices[i];

    m_model->m_mesh = new CMesh();
    m_model->m_mesh->CreateFromMeshData(&meshData);
  
    //////////////////////////////////////////////
   // m_model->m_world = XMMatrixTranslation(pos.x, pos.y, pos.z);

   // m_model->m_transformation.SetPos(XMVectorSet(pos.x, pos.y, pos.z, 1.0f));

    m_model->m_transformation = CTransform();//.SetPos(XMFLOAT3(pos.x, pos.y, pos.z));

   // m_model->m_transformation.m_world = XMMatrixTranslation(posX, posY, posZ);
    m_models.push_back(m_model);

    return false;
}

CModel*  CScene::CreateModelParallax(const std::wstring& shaderFileName,  XMFLOAT3 pos, XMFLOAT3 rot, MeshData& geomData)
{
    CModel* m_model = new(16) CModel();
    m_model->Initialize(EEffectType::EParallaxMapping);

    /// TEST
    m_model->m_shader = new CShader();
    m_model->m_shader->CreateFromCompiledFile(EInputLayoutType::PositionTextureNormalTangentBinormal, L"../assets/parallaxPS.cso", L"../assets/parallaxVS.cso");

    m_model->m_material = new CMaterial();
    m_model->m_material->Initialize(L"c:\\__repository\\textures\\grid_gray_d.dds", L"c:\\__repository\\textures\\grid_gray_n.dds", L"c:\\__repository\\textures\\grid_gray_s.dds", L"c:\\__repository\\textures\\grid_gray_h.dds");

   // m_model->m_material->Initialize(L"c:\\__repository\\box_d.dds", L"c:\\__repository\\box_d.dds", L"c:\\__repository\\box_d.dds", L"c:\\__repository\\box_d.dds");

    
    SMeshData meshData;
    meshData.type = EInputLayoutType::PositionTextureNormalTangentBinormal;

    meshData.indicesCount  = geomData.Indices.size();
    meshData.verticesCount = geomData.Vertices.size();

    meshData.vertexData.vertexPosTexNormBitangentTangent = new SVertexPosTexNormalBitangentTangent[meshData.verticesCount];
    meshData.size = sizeof(SVertexPosTexNormalBitangentTangent);

    uint k = 0;
    for(size_t i = 0; i < meshData.verticesCount; ++i, ++k)
    {
        meshData.vertexData.vertexPosTexNormBitangentTangent[i].pos       = geomData.Vertices[i].Position;
        meshData.vertexData.vertexPosTexNormBitangentTangent[i].tex       = geomData.Vertices[i].TexC;
        meshData.vertexData.vertexPosTexNormBitangentTangent[i].normal    = geomData.Vertices[i].Normal;
        meshData.vertexData.vertexPosTexNormBitangentTangent[i].bitangent = geomData.Vertices[i].TangentU;
        meshData.vertexData.vertexPosTexNormBitangentTangent[i].tangent   = geomData.Vertices[i].TangentU;
    }

    meshData.indices = new uint[meshData.indicesCount];

    for(int i = 0; i < meshData.indicesCount; i++)
        meshData.indices[i] = geomData.Indices[i];

    m_model->m_mesh = new CMesh();
    m_model->m_mesh->CreateFromMeshData(&meshData);

    //////////////////////////////////////////////
    // m_model->m_world = XMMatrixTranslation(pos.x, pos.y, pos.z);

    m_model->m_transformation = CTransform();//.SetPos(XMFLOAT3(pos.x, pos.y, pos.z));

    // m_model->m_transformation.m_world = XMMatrixTranslation(posX, posY, posZ);
    m_models.push_back(m_model);

    return m_model;
}

bool CScene::LoadModel(const std::wstring& fileName, const std::wstring& shaderFileName, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale)
{
    CModel* m_model = new(16) CModel();
    m_model->Initialize(EEffectType::EParallaxMapping);
     

    CMesh* mesh         = Globals::Get().resourceManager.GetMesh(fileName);
    CMaterial* material = Globals::Get().resourceManager.GetMaterial(fileName);
    CShader* shader     = Globals::Get().resourceManager.GetShader(shaderFileName);

    if(shader == nullptr)
    {
        m_model->m_shader = new CShader();
        //m_model->m_shader->CompileFromFile(L"../assets/parallaxPS.fx", L"../assets/parallaxVS.fx");
        m_model->m_shader->CreateFromCompiledFile(EInputLayoutType::PositionTextureNormalTangentBinormal, L"../assets/parallaxPS.cso", L"../assets/parallaxVS.cso");
        Globals::Get().resourceManager.InsertShader(shaderFileName, m_model->m_shader);
    }
    else
    {
        m_model->m_shader = shader;
    }

    if(mesh == nullptr || material == nullptr)
    {
        SMeshData      meshData;
        SMaterialData  materialData;

        m_assimpLoader->Import(fileName,  EInputLayoutType::PositionTextureNormalTangentBinormal, &meshData, &materialData);

        m_model->m_material = new CMaterial();
        m_model->m_material->Initialize(materialData.diffusePath.c_str(), materialData.normalPath.c_str(), materialData.specularPath.c_str(), materialData.heightPath.c_str());

        m_model->m_mesh = new CMesh();
        m_model->m_mesh->CreateFromMeshData(&meshData);

        Globals::Get().resourceManager.InsertMesh(fileName, m_model->m_mesh);
        Globals::Get().resourceManager.InsertMaterial(fileName, m_model->m_material);
    }
    else
    {
        m_model->m_mesh = mesh;
        m_model->m_material = material;
    }
//        XMFLOAT3 scale(5.5f, 5.5f, 5.5f);
    m_model->m_transformation = CTransform(pos, rot, scale);
    
    
    //.SetPos(XMFLOAT3(pos.x, pos.y, pos.z));
    //m_model->m_transformation.SetRot(XMVectorSet(pos.x, pos.y, pos.z, 1.0f));

  //  m_model->m_world = XMMatrixTranslation(posX, posY, posZ);
    m_models.push_back(m_model);

    return true;
}


bool CScene::LoadScene()
{
    CXmlFile xmlFile;
    xmlFile.Open(L"..\\assets\\testScene0.xml");

    CXmlNode root = xmlFile.GetRootNode();
    uint entitiesCount = root.GetChildsCount();

    for(int i = 0; i < entitiesCount; i++)
    {
        CXmlNode tmpNode = root.GetChildByIndex(i);
        uint attrCount = tmpNode.GetAttributesCount();

        std::wstring modelPath  = tmpNode.GetStringAttr(L"modelPath");
        std::wstring shaderPath = tmpNode.GetStringAttr(L"shaderPath");

        float scale = tmpNode.GetFloatAttr(L"scale");
        XMFLOAT3 pos = tmpNode.GetVec3Attr(L"pos");
        XMFLOAT3 rot = tmpNode.GetVec3Attr(L"rot");

        LoadModel(std::wstring(L"c:\\__repository\\") + modelPath, std::wstring(L"..\\Output\\parallaxPS.cso"),  pos, rot);

    }

    return true;
}

void CScene::InitializeScene()
{
    CMaterial material;
  //  LoadScene();
  //  InitializeTerrain();


    GeometryGenerator geoGen;

    MeshData box;
    MeshData grid;


    //    MeshData plane;
    //    geoGen.CreatePlane(100.0f, plane);
    //    CreateModelParallax(std::wstring(L""), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), plane);


    geoGen.CreateGrid(200.0f, 200.0f, 32, 32, grid);
    CModel* m =  CreateModelParallax(std::wstring(L""), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), grid);
    m->texScale = 22.0f;


    XMFLOAT3 pos(0.0f, 0.0f, 0.0f);
    XMFLOAT3 rot(0.0f, 0.0f, 0.0f);

    XMFLOAT3 scale(1.0f, 1.0f, 1.0f);
    XMFLOAT3 rot2(0.0f, 0.0f, 0.0f);

    LoadModel(std::wstring(L"c:\\__repository\\h1.dae"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos, rot);

  
    XMFLOAT3 pos2(-25.0f, 0.0f, 7);
    LoadModel(std::wstring(L"c:\\__repository\\h2.dae"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos2, rot2);

    XMFLOAT3 pos3(-10.0f, 0.0f, -4);
    LoadModel(std::wstring(L"c:\\__repository\\h3.dae"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos3, rot2);

    XMFLOAT3 pos4(9.0f, 0.0f, 0);
    LoadModel(std::wstring(L"c:\\__repository\\h4.dae"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos4, rot2);

    XMFLOAT3 pos5(19.0f, 0.0f, -8);
    LoadModel(std::wstring(L"c:\\__repository\\h5.dae"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos5, rot2);
    
    XMFLOAT3 pos6(35.0f, 0.0f, 18);
    LoadModel(std::wstring(L"c:\\__repository\\haywagon.dae"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos6, rot2);


    XMFLOAT3 pos7(21.0f, 0.0f, -38);
    XMFLOAT3 scale2(0.04f, 0.04f, 0.04f);
    LoadModel(std::wstring(L"c:\\__repository\\electricPole.obj"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos7, rot2, scale2);


 /*
    XMFLOAT3 pos7(-8.0f, 0.0f, -38);
    LoadModel(std::wstring(L"c:\\__repository\\electricPole.obj"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos7, rot2, scale2);


    XMFLOAT3 pos8(-39.0f, 0.0f, -38);
    LoadModel(std::wstring(L"c:\\__repository\\electricPole.obj"), std::wstring(L"..\\Output\\parallaxPS.cso"), pos8, rot2, scale2);
    */
    
  /*  
    for(int i = 0; i < 256; i++)
    {
        float zOffset = i * 15;

        LoadModel(std::wstring(L"c:\\__repository\\h1.dae"),   std::wstring(L"..\\Output\\parallaxPS.cso"),   20.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h2.dae"),   std::wstring(L"..\\Output\\parallaxPS.cso"),   10.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h3.dae"),   std::wstring(L"..\\Output\\parallaxPS.cso"),  -30.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h4.dae"),   std::wstring(L"..\\Output\\parallaxPS.cso"),  -20.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h5.dae"),   std::wstring(L"..\\Output\\parallaxPS.cso"),  -10.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h7.dae"),   std::wstring(L"..\\Output\\parallaxPS.cso"),   0.0f,  0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h8.dae"),   std::wstring(L"..\\Output\\parallaxPS.cso"),  -40.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h9.dae"),   std::wstring(L"..\\Output\\parallaxPS.cso"),   30.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h10.dae"),  std::wstring(L"..\\Output\\parallaxPS.cso"),  -55.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\h11.dae"),  std::wstring(L"..\\Output\\parallaxPS.cso"),   45.0f, 0.0f, zOffset);
        LoadModel(std::wstring(L"c:\\__repository\\tower.dae"),std::wstring(L"..\\Output\\parallaxPS.cso"),   65.0f, 0.0f, zOffset);
    }
    */



    /* 
    SMeshData meshData2;
    m_assimpLoader->Import("c:\\__repository\\box.dae", &meshData2, EInputLayoutType::PositionTextureNormalTangentBinormal);

    CModel* m_model2 = new(16) CModel();
    m_model2->Initialize(&meshData2,  EEffectType::EParallaxMapping);
    XMMATRIX scalingMat = XMMatrixScaling(0.03f, 0.03f, 0.03f);
    m_model2->m_world = XMMatrixTranslation(12.0f, 0.0f, -12.0f);
    m_model2->m_world = XMMatrixMultiply(scalingMat, m_model2->m_world);
    m_models.push_back(m_model2);
    */


    // XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
    // XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
    // XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

}


void CScene::InitializeTerrain()
{
    m_terrain = new CTerrain();
    
    CTerrain::InitInfo terrInfo;
    terrInfo.HeightMapFilename    = L"../assets/Textures/terrain.raw";
    terrInfo.TextureArrayFileName = L"../assets/Textures/terrainTextureArray.dds";
    terrInfo.BlendMapFilename     = L"../assets/Textures/blend.dds";
    terrInfo.HeightScale          = 50.0f;
    terrInfo.HeightmapWidth       = 2049;
    terrInfo.HeightmapHeight      = 2049;
    terrInfo.CellSpacing          = 0.5f;
    
    m_terrain->Initialize(terrInfo);
}

CTerrain* CScene::GetTerrain()
{
    return m_terrain;
}