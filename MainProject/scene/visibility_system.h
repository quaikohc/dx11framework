#pragma once


#define MAX_CULLABLE_OBJECTS 1000




class CVisibilitySystem
{

    BoundingBox m_boundingBoxes[MAX_CULLABLE_OBJECTS];

public:
    CVisibilitySystem();
    ~CVisibilitySystem();

    void Update();

    std::vector<XMFLOAT3> CreateAABB(std::vector<XMFLOAT3> &vertices);

};