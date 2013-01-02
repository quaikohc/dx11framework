#include "stdafx.h"
#include "assimpAnimationLoader.h"
#include <map>


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
    return XMMatrixSet(transform.a1, transform.a2, transform.a3, transform.a4,
                       transform.b1, transform.b2, transform.b3, transform.b4,
                       transform.c1, transform.c2, transform.c3, transform.c4,
                       transform.d1, transform.d2, transform.d3, transform.d4);
}

void VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
    for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(indexes) ; i++) 
    {
        if (indexes[i] == 0.0) 
        {
            indexes[i]     = BoneID;
            weights[i] = Weight;
            return;
        }        
    }
}

CAssimpAnimationLoader::CAssimpAnimationLoader()  
{
    m_numBones = 0;
}

CAssimpAnimationLoader::~CAssimpAnimationLoader() 
{
}

bool CAssimpAnimationLoader::Load(const aiScene*  aiScene, std::vector<SVertexPosNormalTexTanSkinned>& vertices, std::vector<USHORT>& indices, std::vector<SkinnedModel::Subset>& subsets, std::vector<M3dMaterial>& mats)
{
    if(!aiScene)
        return false;

    DirectX::XMVECTOR det    = DirectX::XMMatrixDeterminant(TOXMMATRIX(aiScene->mRootNode->mTransformation));
    XMMATRIX m_globalInverseTransform = DirectX::XMMatrixTranspose( DirectX::XMMatrixInverse(&det, TOXMMATRIX(aiScene->mRootNode->mTransformation)));

    uint meshesCount     = aiScene->mNumMeshes;
    uint materialsCount  = aiScene->mNumMaterials;
    uint animationsCount = aiScene->mNumAnimations;
    uint texturesCount   = aiScene->mNumTextures;

    m_meshes.resize(meshesCount);
       
    uint verticesCount = 0;
    uint indicesCount  = 0;
    
    for (uint i = 0 ; i < meshesCount ; i++) 
    {
        m_meshes[i].m_materialIndex = aiScene->mMeshes[i]->mMaterialIndex;        
        m_meshes[i].m_indicesCount    = aiScene->mMeshes[i]->mNumFaces * 3;
        m_meshes[i].m_baseVertex    = verticesCount;
        m_meshes[i].m_baseIndex     = indicesCount;
        
        verticesCount += aiScene->mMeshes[i]->mNumVertices;
        indicesCount  += m_meshes[i].m_indicesCount;
    }

    vector<VertexBoneData> bones;
    bones.resize(verticesCount);
    indices.resize(indicesCount);

    subsets.resize(meshesCount);
    vertices.resize(verticesCount);

    for(UINT i = 0; i < meshesCount; ++i)
    {
        subsets[i].Id          = 0;
        subsets[i].VertexStart = 0;
        subsets[i].VertexCount = verticesCount;
        subsets[i].FaceStart   = 0;
        subsets[i].FaceCount   = aiScene->mMeshes[i]->mNumFaces * 3; 
    }

    for (uint i = 0 ; i < m_meshes.size() ; i++) 
    {
        const aiMesh* paiMesh = aiScene->mMeshes[i];

        // read vertices
        for (uint i = 0 ; i < paiMesh->mNumVertices ; i++) 
        {
            const aiVector3D* pPos    = &(paiMesh->mVertices[i]);
            const aiVector3D* pNormal = &(paiMesh->mNormals[i]);

            aiVector2D texCoord(paiMesh->mTextureCoords[0][i].x, paiMesh->mTextureCoords[0][i].y);

            vertices[i].Pos     = AssimpVec3ToXMFloat3(paiMesh->mVertices[i]);  
            vertices[i].Normal  = AssimpVec3ToXMFloat3(paiMesh->mNormals[i]);
            vertices[i].Tex     = AssimpVec2ToXMFloat2(texCoord);

            vertices[i].TangentU.x = vertices[i].TangentU.y = vertices[i].TangentU.z = vertices[i].TangentU.w = 0.0f;
        }

        // read indices
        for (uint i = 0 ; i < paiMesh->mNumFaces ; i++) 
        {
            const aiFace& Face = paiMesh->mFaces[i];
            assert(Face.mNumIndices == 3);
            indices.push_back(Face.mIndices[0]);
            indices.push_back(Face.mIndices[1]);
            indices.push_back(Face.mIndices[2]);
        }

        LoadBones(i, paiMesh, bones);
    }

    int boneIndices[4];
    float weights[4];

    for(uint i = 0; i < verticesCount; ++i)
    {
        weights[0]     = bones[i].weights[0];
        weights[1]     = bones[i].weights[1];
        weights[2]     = bones[i].weights[2];
        weights[3]     = bones[i].weights[3];
        boneIndices[0] = bones[i].indexes[0];
        boneIndices[1] = bones[i].indexes[1];
        boneIndices[2] = bones[i].indexes[2];
        boneIndices[3] = bones[i].indexes[3];

        vertices[i].Weights.x  = weights[0];
        vertices[i].Weights.y  = weights[1];
        vertices[i].Weights.z  = weights[2];

        vertices[i].BoneIndices[0] = (BYTE) boneIndices[0]; 
        vertices[i].BoneIndices[1] = (BYTE) boneIndices[1]; 
        vertices[i].BoneIndices[2] = (BYTE) boneIndices[2]; 
        vertices[i].BoneIndices[3] = (BYTE) boneIndices[3]; 
    }

    return true;
}

void CAssimpAnimationLoader::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
    for (uint i = 0 ; i < pMesh->mNumBones ; i++)
    {                
        uint BoneIndex = 0;        
        string BoneName(pMesh->mBones[i]->mName.data);
        
        if (m_boneMapping.find(BoneName) == m_boneMapping.end())
        {
            BoneIndex = m_numBones;
            m_numBones++;

            m_boneMapping[BoneName] = BoneIndex;
        }
        else
        {
            BoneIndex = m_boneMapping[BoneName];
        }                      
        
        for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) 
        {
            uint VertexID = m_meshes[MeshIndex].m_baseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }    
}

bool CAssimpAnimationLoader::InitMaterials(const aiScene* pScene)
{
    for (uint i = 0; i < pScene->mNumMaterials; i++) 
    {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
        {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
            {
                string p(Path.data);
                ////// ... 
                // do sth with it
            }
        }
    }

    return true;
}