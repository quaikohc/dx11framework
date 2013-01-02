#include "stdafx.h"
#include "assimp_loader.h"
#include "misc.h"


bool CAssimpLoader::Import(const std::wstring& fileName, EInputLayoutType type, SMeshData* meshData,  SMaterialData* materialData)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(WStringToString(fileName), g_ppsteps);

    meshData->type = type;

    if(!scene) 
        return false;

  //  int numMeshes    = scene->mNumMeshes;
 //   int numMaterials = scene->mNumMaterials;
  //  int numTextures  = scene->mNumTextures;

    ImportNode(scene->mRootNode, scene, meshData, materialData);

    return true;
}

void CAssimpLoader::ImportNode(aiNode* node, const aiScene* scene, SMeshData* meshData, SMaterialData* materialData)
{
    for (uint i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];

        unsigned int numVertices = mesh->mNumVertices;
        unsigned int numFaces    = mesh->mNumFaces;

        switch(meshData->type)
        {
            case EInputLayoutType::Position:
                meshData->vertexData.vertexPos = new SVertexPos[numVertices];
                meshData->size = sizeof(SVertexPos);
            break;
            case EInputLayoutType::PositionTexture:
                meshData->vertexData.vertexPosTex = new SVertexPosTex[numVertices];
                meshData->size = sizeof(SVertexPosTex);
            break;
            case EInputLayoutType::PositionTextureNormal:
                meshData->vertexData.vertexPosTexNorm = new SVertexPosTexNorm[numVertices];
                meshData->size = sizeof(SVertexPosTexNorm);
            break;
            case EInputLayoutType::PositionTextureNormalTangentBinormal:
                meshData->vertexData.vertexPosTexNormBitangentTangent = new SVertexPosTexNormalBitangentTangent[numVertices];
                meshData->size = sizeof(SVertexPosTexNormalBitangentTangent);
            break;
            default:
                assert(false);
        };

        //////////////////////////////////////
        /// TODO: REFACTOR THIS///////////////
        //////////////////////////////////////

        switch(meshData->type)
        {
            case EInputLayoutType::Position:
                for(uint j = 0; j < numVertices; j++)
                {
                    meshData->vertexData.vertexPos[j].pos = AssimpVec3ToXMFloat3(mesh->mVertices[j]);
                }
            break;
            case EInputLayoutType::PositionTexture:
                for(uint j = 0; j < numVertices; j++)
                {
                    meshData->vertexData.vertexPosTex[j].pos = AssimpVec3ToXMFloat3(mesh->mVertices[j]);
                    aiVector2D texCoord(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
                    meshData->vertexData.vertexPosTex[j].tex  = AssimpVec2ToXMFloat2(texCoord);
                }
            break;
            case EInputLayoutType::PositionTextureNormal:
                for(uint j = 0; j < numVertices; j++)
                {
                    meshData->vertexData.vertexPosTexNorm[j].pos = AssimpVec3ToXMFloat3(mesh->mVertices[j]);
                    aiVector2D texCoord(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
                    meshData->vertexData.vertexPosTexNorm[j].tex    = AssimpVec2ToXMFloat2(texCoord);
                    meshData->vertexData.vertexPosTexNorm[j].normal = AssimpVec3ToXMFloat3(mesh->mNormals[j]);
                }
            break;
            case EInputLayoutType::PositionTextureNormalTangentBinormal:    
                for(uint j = 0; j < numVertices; j++)
                {
                    meshData->vertexData.vertexPosTexNormBitangentTangent[j].pos = AssimpVec3ToXMFloat3(mesh->mVertices[j]);
                    aiVector2D texCoord(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
                    meshData->vertexData.vertexPosTexNormBitangentTangent[j].tex       = AssimpVec2ToXMFloat2(texCoord);
                    meshData->vertexData.vertexPosTexNormBitangentTangent[j].normal    = AssimpVec3ToXMFloat3(mesh->mNormals[j]);
                    meshData->vertexData.vertexPosTexNormBitangentTangent[j].bitangent = AssimpVec3ToXMFloat3(mesh->mBitangents[j]);
                    meshData->vertexData.vertexPosTexNormBitangentTangent[j].tangent   = AssimpVec3ToXMFloat3(mesh->mTangents[j]);
                }
            break;
            default:
                assert(false);
        };

        /*

        for(uint j = 0; j < numVertices; j++)
        {
            vertices[j].pos       = AssimpVec3ToXMFloat3(mesh->mVertices[j]);
            vertices[j].normal    = AssimpVec3ToXMFloat3(mesh->mNormals[j]);
            vertices[j].bitangent = AssimpVec3ToXMFloat3(mesh->mBitangents[j]);
            vertices[j].tangent   = AssimpVec3ToXMFloat3(mesh->mTangents[j]);

            aiVector2D texCoord(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
            vertices[j].tex       = AssimpVec2ToXMFloat2(texCoord);
        }*/

        //////////////////////////////////////
        //////////////////////////////////////
        //////////////////////////////////////

        uint nidx = 0;
        switch (mesh->mPrimitiveTypes) 
        {
            case aiPrimitiveType_POINT:
                nidx = 1; break;
            case aiPrimitiveType_LINE:
                nidx = 2; break;
            case aiPrimitiveType_TRIANGLE:
                nidx = 3; break;
            default: 
                assert(false);
        };

        int numIndices = numFaces * nidx;

        uint* indices = new uint[numFaces * nidx];
        uint* originalIndices = indices;

        for (uint x = 0; x < numFaces; ++x)
        {
            for (uint a = 0; a < nidx; ++a)
            {
                *indices++ = mesh->mFaces[x].mIndices[a];
            }
        }

        const aiMaterial* material = scene->mMaterials[0];

        aiString szPath;
        aiGetMaterialString(material, AI_MATKEY_TEXTURE_DIFFUSE(0),  &szPath);
        //aiGetMaterialString(material, AI_MATKEY_TEXTURE_SPECULAR(0), &szPath);
        //aiGetMaterialString(material, AI_MATKEY_TEXTURE_NORMALS(0),  &szPath);

        meshData->verticesCount = numVertices;
        meshData->indicesCount  = numIndices;
        meshData->indices       = originalIndices;
        //meshData->vertices      = vertices;

        if(szPath.length == 0)
        {
            materialData->diffusePath  = L"c:\\__repository\\null_d.dds";
            materialData->normalPath   = L"c:\\__repository\\null_n.dds";
            materialData->specularPath = L"c:\\__repository\\null_s.dds";
            materialData->heightPath   = L"c:\\__repository\\null_h.dds";
        }
        else
        {
            materialData->diffusePath  = L"c:\\__repository\\" + StringToWString(std::string(szPath.C_Str())) + L"_d.dds" ;
            materialData->normalPath   = L"c:\\__repository\\" + StringToWString(std::string(szPath.C_Str())) + L"_n.dds" ;
            materialData->specularPath = L"c:\\__repository\\" + StringToWString(std::string(szPath.C_Str())) + L"_s.dds" ;
            materialData->heightPath   = L"c:\\__repository\\" + StringToWString(std::string(szPath.C_Str())) + L"_h.dds" ;
        }
    }

    for(uint i = 0; i < node->mNumChildren;++i)
    {
        ImportNode(node->mChildren[i], scene, meshData, materialData);
    }
}