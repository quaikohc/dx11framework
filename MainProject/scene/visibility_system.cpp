
#include "stdafx.h"
#include "visibility_system.h"


CVisibilitySystem::CVisibilitySystem()
{

}

CVisibilitySystem::~CVisibilitySystem()
{

}

void CVisibilitySystem::Update()
{


}

std::vector<XMFLOAT3> CreateAABB(std::vector<XMFLOAT3> &vertPosArray)
{
    XMFLOAT3 minVertex = XMFLOAT3( FLT_MAX,  FLT_MAX,  FLT_MAX);
    XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for(UINT i = 0; i < vertPosArray.size(); i++)
    {
        minVertex.x = min(minVertex.x, vertPosArray[i].x);
        minVertex.y = min(minVertex.y, vertPosArray[i].y);
        minVertex.z = min(minVertex.z, vertPosArray[i].z);

        maxVertex.x = max(maxVertex.x, vertPosArray[i].x);
        maxVertex.y = max(maxVertex.y, vertPosArray[i].y);
        maxVertex.z = max(maxVertex.z, vertPosArray[i].z);
    }

    std::vector<XMFLOAT3> box;

    box.push_back(minVertex);
    box.push_back(maxVertex);

    return box;
}
