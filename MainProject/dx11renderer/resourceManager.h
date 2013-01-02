#pragma once

#include "../utils/hashtable.h"


/*
//////// GOALS:

1. fast (O(1)) queries whether a resource is already loaded or not, and the ability to quickly retrieve and return the resource if it's already loaded.
2. thread-safe for adding/removing resources.
3. can load resources that are missing from the resource manager in a separate thread using a listener pattern to notify users when the resource becomes available.
4. the resource manager can store resources in an LRU queue and can use a configurable amount of RAM for all resources of a specific type, evicting the least-recently used resource when the resource manager's quota is exceeded.
5. resources are "ref-counted" so that they automatically go away when no other game sub-systems have references to them.
6. resources can be loaded either from files on disk or files in a compressed archive.
7. the resource manager manages search paths, in either a relative or absolute fashion, depending on user taste.
*/

class CAssimpLoader;
class CShader;

class CResourceManager
{
    typedef cbhashtableentry_hkd<const wchar*, CMesh*,     cbhashtable_ops_wcharptr> meshHashTableEntry;
    typedef cbhashtableentry_hkd<const wchar*, CMaterial*, cbhashtable_ops_wcharptr> materialHashTableEntry;
    typedef cbhashtableentry_hkd<const wchar*, CShader*,   cbhashtable_ops_wcharptr> shaderHashTableEntry;

    hashtable<meshHashTableEntry>       m_meshes;
    hashtable<materialHashTableEntry>   m_materials;
    hashtable<shaderHashTableEntry>     m_shaders;

    CAssimpLoader*       m_assimpLoader;

public:
    CResourceManager();
    ~CResourceManager();

    void                Initialize();

    void                InsertShader(const std::wstring& name, CShader* shader);
    void                InsertMesh(const std::wstring& name, CMesh* mesh);
    void                InsertMaterial(const std::wstring& name, CMaterial* material);

    CShader*            GetShader(std::wstring name);
    CMesh*              GetMesh(std::wstring name);
    CMaterial*          GetMaterial(std::wstring name);

};