#pragma once

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>          
#include <assimp/postprocess.h>  

#include <map>
#include <vector>
#include "skinnedModel.h"

using namespace Assimp;
using namespace std;


struct VertexBoneData
{        
    // there is a maximum of 4 bones influencing a vertex
    uint  indexes[4];
    float weights[4];

    VertexBoneData()
    {
        Reset();
    };
        
    void Reset()
    {
        ZERO_MEM(indexes);
        ZERO_MEM(weights);        
    }
        
    void AddBoneData(uint boneIndex, float weight);
};


class CAssimpAnimationLoader
{
    struct MeshEntry 
    {
        MeshEntry()
        {
            m_indicesCount   = 0;
            m_baseVertex    = 0;
            m_baseIndex     = 0;
            m_materialIndex = 0;
        }
        
        uint m_indicesCount;
        uint m_baseVertex;
        uint m_baseIndex;
        uint m_materialIndex;
    };
    
    vector<MeshEntry>    m_meshes;
    map<string, uint>    m_boneMapping; // maps a bone name to its index
    uint                 m_numBones;

public:
    CAssimpAnimationLoader();
    ~CAssimpAnimationLoader();

    bool Load(const aiScene*  m_aiScene, std::vector<SVertexPosNormalTexTanSkinned>& vertices, std::vector<USHORT>& indices, std::vector<SkinnedModel::Subset>& subsets, std::vector<M3dMaterial>& mats);

    void LoadBones(uint MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
    bool InitMaterials(const aiScene* pScene);
};
