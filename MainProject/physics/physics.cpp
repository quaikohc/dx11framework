#include "stdafx.h"
#include "physics.h"


CPhysics::CPhysics()
{

}

CPhysics::~CPhysics()
{

}    

bool CPhysics::Initialize()
{
 /*   m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocatorCallback, m_errorCallback);
    if(!m_foundation)
        assert(false);

    m_sdk = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), false, nullptr);
    if(!m_sdk)
        assert(false);

    if(!PxInitExtensions(*m_sdk))
        assert(false);

    PxSceneDesc sceneDesc(m_sdk->getTolerancesScale());

    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.flags  |= PxSceneFlag::eENABLE_SWEPT_INTEGRATION;

    m_cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.cpuDispatcher	= m_cpuDispatcher;

   // sceneDesc.filterShader = CustomSceneFilterShader;
   // sceneDesc.simulationEventCallback = this;

    m_scene = m_sdk->createScene(sceneDesc);
    */
    return false;
}

void CPhysics::Update()
{


}