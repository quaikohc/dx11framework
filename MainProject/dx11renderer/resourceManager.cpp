#include "stdafx.h"
#include "resourceManager.h"
#include "../utils/assimp_loader.h"


CResourceManager::CResourceManager()
{
    m_assimpLoader = new CAssimpLoader();

}

CResourceManager::~CResourceManager()
{
    if(m_assimpLoader)
        delete m_assimpLoader;
}

void CResourceManager::Initialize()
{

}

void CResourceManager::InsertMaterial(const std::wstring& name, CMaterial* material)
{
    m_materials.insert(name.c_str(), material);
}

void CResourceManager::InsertMesh(const std::wstring& name, CMesh* mesh)
{
    m_meshes.insert(name.c_str(), mesh);
}

void CResourceManager::InsertShader(const std::wstring& name, CShader* shader)
{
    m_shaders.insert(name.c_str(), shader);
}

CMesh* CResourceManager::GetMesh(std::wstring name)
{
    meshHashTableEntry const* entry = m_meshes.find(name.c_str());

    if(entry)
        return entry->m_data;
    else
        return nullptr;
}

CMaterial* CResourceManager::GetMaterial(std::wstring name)
{
    materialHashTableEntry const* entry = m_materials.find(name.c_str());

    if(entry)
        return entry->m_data;
    else
        return nullptr;
}


CShader* CResourceManager::GetShader(std::wstring name)
{
    shaderHashTableEntry const* entry = m_shaders.find(name.c_str());

    if(entry)
        return entry->m_data;
    else
        return nullptr;
}