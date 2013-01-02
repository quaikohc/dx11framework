#pragma once

#include <PxPhysicsAPI.h>



class CPhysics
{
    physx::PxPhysics*                   m_sdk;
    physx::PxFoundation*                m_foundation;

    physx::PxDefaultAllocator           m_allocatorCallback;
    physx::PxDefaultErrorCallback       m_errorCallback;

    physx::PxDefaultCpuDispatcher*      m_cpuDispatcher;
    physx::PxScene*                     m_scene;


public:
    CPhysics();
    ~CPhysics();

    bool                                Initialize();
    void                                Update();

};