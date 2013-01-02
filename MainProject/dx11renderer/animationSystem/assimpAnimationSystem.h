#pragma once

#include <map>
#include <vector>
#include <tuple>
#include <string>

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>          
#include <assimp/postprocess.h>  


// Very basic skinned animation support

// TODO: 
// - clips management & loading clips from xml
// - clips blending
// - calculate tangent space
// - OPTIMIZATION: perform all calculation using xna math, add multithreading, add support for instancing, use custom fixed allocator

// TODO later:
// - dual quaternion skinning
// - IK
// - morphing
// - loading bvh

class CBone 
{
public:
    std::string             m_name;
    XMFLOAT4X4              m_localTransform;
    XMFLOAT4X4              m_globalTransform;
    XMFLOAT4X4              m_originalLocalTransform;
    XMFLOAT4X4              m_offset;

    CBone*                  m_parent;
    std::vector<CBone*>     m_children;

    CBone() : m_parent(0)
    {
    }

    ~CBone()
    { 
        for(size_t i(0); i < m_children.size(); i++) 
        {
            if(m_children[i])
            delete m_children[i]; 
        }
    }
};

class CChannel
{
public:
    std::string                 m_name;
    std::vector<aiVectorKey>    m_posKeys;
    std::vector<aiQuatKey>      m_rotKeys;
    std::vector<aiVectorKey>    m_scaleKeys;
};

class CClip
{
public:
    CClip();
    CClip(const aiAnimation* anim);

    void                        Update(float time, std::map<std::string, CBone*>& bones);
    std::vector<XMFLOAT4X4>&    GetTransforms(float dt);
    uint                        GetFrameIndexAt(float time);

    std::string                 m_name;
    std::vector<CChannel>       m_channels;

    float                       m_lastTime;
    float                       m_ticksPerSecond;
    float                       m_duration;

    std::vector<std::tuple<uint, uint, uint> >  m_lastPositions;
    std::vector<std::vector<XMFLOAT4X4>>        m_transforms;
};


class CAnimationSystem
{
public:
    CAnimationSystem();
    ~CAnimationSystem();

    void                            Initialize(const aiScene* scene);
                                                     
    bool                            SetCurrentClip(int index);
    bool                            SetCurrentClip(const std::string& name);
                                    
    std::vector<XMFLOAT4X4>&        GetTransforms(float dt);

    std::vector<CClip>              m_clips;
    int                             m_currentClip;

protected:
    void                            UpdateTransforms(CBone* node);
    void                            CalculateBoneToWorldTransform(CBone* node);

    void                            Calculate(float time);

    CBone*                          CreateHierarchy(aiNode* node, CBone* parent);

    CBone*                          m_skeletonRoot;
    std::map<std::string, CBone*>   m_bonesMapping;
    std::map<std::string, uint>     m_clipsMapping;
    std::vector<CBone*>             m_bones;
    std::vector<XMFLOAT4X4>         m_bonesTransforms;
};