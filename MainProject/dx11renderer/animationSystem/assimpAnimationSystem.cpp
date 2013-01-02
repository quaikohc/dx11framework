#include "stdafx.h"
#include "assimpAnimationSystem.h"
#include <iostream>


inline XMFLOAT4X4 TOXMFLOAT4X4(aiMatrix4x4 transform)
{
    return XMFLOAT4X4(transform.a1,transform.a2,transform.a3,transform.a4,
        transform.b1,transform.b2,transform.b3,transform.b4,
        transform.c1,transform.c2,transform.c3,transform.c4,
        transform.d1,transform.d2,transform.d3,transform.d4);
}

CClip::CClip(): m_lastTime(0.0f), m_ticksPerSecond(0.0f), m_duration(0.0f)
{

}

CClip::CClip(const aiAnimation* pAnim) 
{
    m_lastTime        = 0.0;
    m_ticksPerSecond  = static_cast<float>(pAnim->mTicksPerSecond != 0.0f ? pAnim->mTicksPerSecond : 100.0f);
    m_duration        = static_cast<float>(pAnim->mDuration);
    m_name            = pAnim->mName.data;

    m_channels.resize(pAnim->mNumChannels);

    for(uint a = 0; a < pAnim->mNumChannels; a++)
    {
        m_channels[a].m_name = pAnim->mChannels[a]->mNodeName.data;

        for(uint i(0); i< pAnim->mChannels[a]->mNumPositionKeys; i++) 
            m_channels[a].m_posKeys.push_back(pAnim->mChannels[a]->mPositionKeys[i]);

        for(uint i(0); i< pAnim->mChannels[a]->mNumRotationKeys; i++) 
            m_channels[a].m_rotKeys.push_back(pAnim->mChannels[a]->mRotationKeys[i]);

        for(uint i(0); i< pAnim->mChannels[a]->mNumScalingKeys; i++) 
        {
            m_channels[a].m_scaleKeys.push_back(pAnim->mChannels[a]->mScalingKeys[i]);
        }
    }
    m_lastPositions.resize(pAnim->mNumChannels, std::make_tuple( 0, 0, 0));

}

std::vector<XMFLOAT4X4>& CClip::GetTransforms(float dt)
{ 
    return m_transforms[GetFrameIndexAt(dt)]; 
}

uint CClip::GetFrameIndexAt(float ptime)
{
    ptime *= m_ticksPerSecond;
    
    float time = 0.0f;

    if(m_duration > 0.0)
        time = fmod(ptime, m_duration);

    float percent = time / m_duration;
    
    return static_cast<uint>((static_cast<float>(m_transforms.size()) * percent));
}

void CClip::Update(float pTime, std::map<std::string, CBone*>& bones) 
{
    pTime *= m_ticksPerSecond;
    
    float time = 0.0f;

    if(m_duration > 0.0)
        time = fmod(pTime, m_duration);

    for(uint a = 0; a < m_channels.size(); a++)
    {
        const CChannel* channel = &m_channels[a];
        auto bonenode = bones.find(channel->m_name);

        if(bonenode == bones.end())
            continue;

        // ******** Position *****
        aiVector3D presentPosition( 0, 0, 0);
        if(channel->m_posKeys.size() > 0)
        {
            // Look for present frame number. Search from last position if time is after the last time, else from beginning
            // Should be much quicker than always looking from start for the average use case.
            uint frame = (time >= m_lastTime) ? std::get<0>(m_lastPositions[a]): 0;

            while(frame < channel->m_posKeys.size() - 1)
            {
                if(time < channel->m_posKeys[frame+1].mTime)
                {
                    break;
                }
                frame++;
            }

            // interpolate between this frame's value and next frame's value
            uint nextFrame = (frame + 1) % channel->m_posKeys.size();
    
            const aiVectorKey& key = channel->m_posKeys[frame];
            const aiVectorKey& nextKey = channel->m_posKeys[nextFrame];

            double diffTime = nextKey.mTime - key.mTime;

            if( diffTime < 0.0)
                diffTime += m_duration;

            if( diffTime > 0) 
            {
                float factor = float((time - key.mTime) / diffTime);
                presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
            } 
            else 
            {
                presentPosition = key.mValue;
            }
            std::get<0>(m_lastPositions[a]) = frame;
        }
        // ******** Rotation *********
        aiQuaternion presentRotation(1, 0, 0, 0);
        if( channel->m_rotKeys.size() > 0)
        {
            uint frame = (time >= m_lastTime) ? std::get<1>(m_lastPositions[a]) : 0;

            while(frame < channel->m_rotKeys.size()  - 1)
            {
                if(time < channel->m_rotKeys[frame+1].mTime)
                    break;
                frame++;
            }

            // interpolate between this frame's value and next frame's value
            uint nextFrame = (frame + 1) % channel->m_rotKeys.size() ;

            const aiQuatKey& key = channel->m_rotKeys[frame];
            const aiQuatKey& nextKey = channel->m_rotKeys[nextFrame];
            double diffTime = nextKey.mTime - key.mTime;

            if(diffTime < 0.0) 
                diffTime += m_duration;

            if( diffTime > 0) 
            {
                float factor = float( (time - key.mTime) / diffTime);
                aiQuaternion::Interpolate( presentRotation, key.mValue, nextKey.mValue, factor);
            } 
            else 
                presentRotation = key.mValue;

            std::get<1>(m_lastPositions[a]) = frame;
        }

        // ******** Scaling **********
        aiVector3D presentScaling(1, 1, 1);
        if( channel->m_scaleKeys.size() > 0) 
        {
            uint frame = (time >= m_lastTime) ? std::get<2>(m_lastPositions[a]) : 0;

            while(frame < channel->m_scaleKeys.size() - 1)
            {
                if(time < channel->m_scaleKeys[frame+1].mTime)
                    break;
                frame++;
            }

            presentScaling = channel->m_scaleKeys[frame].mValue;
            std::get<2>(m_lastPositions[a]) = frame;
        }

        aiMatrix4x4 mat = aiMatrix4x4(presentRotation.GetMatrix());

        mat.a1 *= presentScaling.x; mat.b1 *= presentScaling.x; mat.c1 *= presentScaling.x;
        mat.a2 *= presentScaling.y; mat.b2 *= presentScaling.y; mat.c2 *= presentScaling.y;
        mat.a3 *= presentScaling.z; mat.b3 *= presentScaling.z; mat.c3 *= presentScaling.z;
        mat.a4 = presentPosition.x; mat.b4 = presentPosition.y; mat.c4 = presentPosition.z;

        mat.Transpose();
        
        bonenode->second->m_localTransform = TOXMFLOAT4X4(mat);
    }
    m_lastTime = time;
}

CAnimationSystem::CAnimationSystem(): m_skeletonRoot(0), m_currentClip(-1) 
{
}

CAnimationSystem::~CAnimationSystem()
{ 
    m_currentClip = -1;
    m_clips.clear();

    delete m_skeletonRoot;
    m_skeletonRoot = nullptr;
}

std::vector<XMFLOAT4X4>& CAnimationSystem::GetTransforms(float dt)
{ 
    return m_clips[m_currentClip].GetTransforms(dt); 
}

void CAnimationSystem::Initialize(const aiScene* scene)
{
    if(!scene->HasAnimations()) return;
    
    m_skeletonRoot = CreateHierarchy(scene->mRootNode, nullptr);

    for(uint i = 0; i< scene->mNumAnimations; i++)
    {
        m_clips.push_back(CClip(scene->mAnimations[i])); 
    }

    for(uint i = 0; i < m_clips.size(); i++)
    {
        m_clipsMapping.insert(std::map<std::string, uint>::value_type(m_clips[i].m_name, i));
    }

    m_currentClip = 0;
    SetCurrentClip("Idle");

    for (uint i = 0; i < scene->mNumMeshes;++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        
        for (uint n = 0; n < mesh->mNumBones;++n)
        {
            const aiBone* bone = mesh->mBones[n];
            std::map<std::string, CBone*>::iterator found = m_bonesMapping.find(bone->mName.data);
            
            if(found != m_bonesMapping.end())
            {
                bool skip = false;
                for(uint  j = 0; j< m_bones.size(); j++)
                {
                    std::string bname = bone->mName.data;
                    if(m_bones[j]->m_name == bname) 
                    {
                        skip = true;
                        break;
                    }
                }
                if(!skip)
                {
                    std::string tes = found->second->m_name;
                    found->second->m_offset =  TOXMFLOAT4X4(bone->mOffsetMatrix);

                    XMMATRIX tmp = XMLoadFloat4x4(&found->second->m_offset);
                    tmp = XMMatrixTranspose(tmp);
                    XMStoreFloat4x4(&found->second->m_offset, tmp);

                    m_bones.push_back(found->second);
                }
            } 
        }
    }
    m_bonesTransforms.resize(m_bones.size());

    float timestep = 1.0f / 30.0f;

    for(uint i = 0; i < m_clips.size(); i++) 
    {
        SetCurrentClip(i);
        float dt = 0;

        for(float ticks = 0; ticks < m_clips[i].m_duration; ticks += m_clips[i].m_ticksPerSecond / 30.0f)
        {
            dt += timestep;
            Calculate(dt);
            m_clips[i].m_transforms.push_back(std::vector<XMFLOAT4X4>());

            std::vector<XMFLOAT4X4>& trans = m_clips[i].m_transforms.back();

            for(uint a = 0; a < m_bonesTransforms.size(); ++a)
            {
                XMMATRIX tmp = XMLoadFloat4x4(&m_bones[a]->m_offset);
                XMMATRIX tmp2 = XMLoadFloat4x4(&m_bones[a]->m_globalTransform);
                tmp2 = XMMatrixMultiply(tmp, tmp2);
                XMFLOAT4X4 rotationmat;
                XMStoreFloat4x4(&rotationmat, tmp2);

                trans.push_back(rotationmat);
            }
        }
    }
}

bool CAnimationSystem::SetCurrentClip(const std::string& name)
{
    auto iter = m_clipsMapping.find(name);
    int oldindex = m_currentClip;

    if(iter != m_clipsMapping.end()) 
        m_currentClip = iter->second;

    return oldindex != m_currentClip;
}

bool CAnimationSystem::SetCurrentClip(int  pAnimIndex)
{
    if(pAnimIndex >= m_clips.size()) 
        return false;

    int oldindex = m_currentClip;
    m_currentClip = pAnimIndex;

    return oldindex != m_currentClip;
}

void CAnimationSystem::Calculate(float time)
{
    if( (m_currentClip < 0) | (m_currentClip >= m_clips.size()) ) 
        return;
    
    m_clips[m_currentClip].Update(time, m_bonesMapping);
    UpdateTransforms(m_skeletonRoot);
}


CBone* CAnimationSystem::CreateHierarchy(aiNode* node, CBone* parent)
{
    CBone* internalNode     = new CBone();
    internalNode->m_name    = node->mName.data;
    internalNode->m_parent  = parent;

    m_bonesMapping[internalNode->m_name] = internalNode;// use the name as a key
    internalNode->m_localTransform =  TOXMFLOAT4X4(node->mTransformation);


    XMMATRIX tmp = XMLoadFloat4x4(&internalNode->m_localTransform);
    tmp = XMMatrixTranspose(tmp);
    XMStoreFloat4x4(&internalNode->m_localTransform, tmp);


    internalNode->m_originalLocalTransform = internalNode->m_localTransform;
    CalculateBoneToWorldTransform(internalNode);

    // continue for all child nodes and assign the created internal nodes as our children
    for(uint a = 0; a < node->mNumChildren; a++)
    {
        internalNode->m_children.push_back(CreateHierarchy(node->mChildren[a], internalNode));
    }

    return internalNode;
}

void CAnimationSystem::UpdateTransforms(CBone* node) 
{
    CalculateBoneToWorldTransform(node);

    for(std::vector<CBone*>::iterator it = node->m_children.begin(); it != node->m_children.end(); ++it)
        UpdateTransforms(*it);
}


void CAnimationSystem::CalculateBoneToWorldTransform(CBone* child)
{
    child->m_globalTransform = child->m_localTransform;
    CBone* parent = child->m_parent;

    while(parent)  // this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
    {
        XMMATRIX tmp = XMLoadFloat4x4(&child->m_globalTransform);
        XMMATRIX tmp2 = XMLoadFloat4x4(&parent->m_localTransform);
        tmp2 = XMMatrixMultiply(tmp, tmp2);
        XMFLOAT4X4 rotationmat;
        XMStoreFloat4x4(&rotationmat, tmp2);

        child->m_globalTransform = rotationmat;//child->m_globalTransform * parent->LocalTransform;
        parent  = parent->m_parent;   // get the parent of the bone we are working on 
    }
}
