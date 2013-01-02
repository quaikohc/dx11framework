#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../dx11renderer/dx11renderer.h"

using namespace Assimp;

//extern Assimp::Importer* g_assimpImporter;

static unsigned int g_ppsteps = aiProcess_CalcTangentSpace         | // calculate tangents and bitangents if possible
                                aiProcess_ConvertToLeftHanded      |
                                aiProcess_Triangulate              |
                                aiProcess_SortByPType              |
                                aiProcess_JoinIdenticalVertices    | // join identical vertices/ optimize indexing
                                aiProcess_ValidateDataStructure    |// perform a full validation of the loader's output
                                aiProcess_ImproveCacheLocality     | // improve the cache locality of the output vertices
                                aiProcess_RemoveRedundantMaterials | // remove redundant materials
                                aiProcess_FindDegenerates          | // remove degenerated polygons from the import
                                aiProcess_FindInvalidData          | // detect invalid model data, such as invalid normal vectors
                                aiProcess_GenUVCoords              | // convert spherical, cylindrical, box and planar mapping to proper UVs
                                aiProcess_TransformUVCoords        | // preprocess UV transformations (scaling, translation ...)
                                aiProcess_FindInstances            | // search for instanced meshes and remove them by references to one master
                                aiProcess_LimitBoneWeights         | // limit bone weights to 4 per vertex
                                aiProcess_OptimizeMeshes           | // join small meshes, if possible;
                                aiProcess_OptimizeGraph
                                ;

class CAssimpLoader
{

public:
    CAssimpLoader(){};
    ~CAssimpLoader(){};

    bool Import(const std::wstring& pFile, EInputLayoutType type, SMeshData* meshData,  SMaterialData* materialData);
    void ImportNode(aiNode* node, const aiScene* scene, SMeshData* meshData, SMaterialData* materialData);

};


static XMFLOAT3 AssimpVec3ToXMFloat3(const aiVector3D & source)
{
    XMFLOAT3 vec;
    vec.x = source.x;
    vec.y = source.y;
    vec.z = source.z;
    return vec;
}

static XMFLOAT2 AssimpVec2ToXMFloat2(const aiVector2D & source)
{
    XMFLOAT2 vec;
    vec.x = source.x;
    vec.y = source.y;
    return vec;
}

static inline XMMATRIX TOXMMATRIX(aiMatrix4x4 transform)
{
            return XMMatrixSet(transform.a1,transform.a2,transform.a3,transform.a4,
            transform.b1,transform.b2,transform.b3,transform.b4,
            transform.c1,transform.c2,transform.c3,transform.c4,
            transform.d1,transform.d2,transform.d3,transform.d4);				
}

static inline XMFLOAT4X4 TOXMFLOAT4X4(aiMatrix4x4 transform)
{
    return XMFLOAT4X4(transform.a1,transform.a2,transform.a3,transform.a4,
            transform.b1,transform.b2,transform.b3,transform.b4,
            transform.c1,transform.c2,transform.c3,transform.c4,
            transform.d1,transform.d2,transform.d3,transform.d4);			
}