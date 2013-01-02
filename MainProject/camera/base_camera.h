#pragma once
#include "stdafx.h"



class CCamera
{
public:
    CCamera();
    ~CCamera();

    XMVECTOR                GetPositionXM() const;
    XMFLOAT3                GetPosition()   const;

    void                    SetPosition(float x, float y, float z);
    void                    SetPosition(const XMFLOAT3& v);

    XMVECTOR                GetRightXM() const;
    XMFLOAT3                GetRight()   const;
    XMVECTOR                GetUpXM()    const;
    XMFLOAT3                GetUp()      const;
    XMVECTOR                GetLookXM()  const;
    XMFLOAT3                GetLook()    const;

    float                   GetNearZ()   const;
    float                   GetFarZ()    const;
    float                   GetAspect()  const;
    float                   GetFovY()    const;
    float                   GetFovX()    const;

    float                   GetNearWindowWidth()  const;
    float                   GetNearWindowHeight() const;
    float                   GetFarWindowWidth()   const;
    float                   GetFarWindowHeight()  const;

    void                    SetLens(float fovY, float aspect, float zn, float zf);

    void                    LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
    void                    LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

    XMMATRIX                GetView()     const;
    XMMATRIX                GetProj()     const;
    XMMATRIX                GetViewProj() const;


    void                    Strafe(float d);
    void                    Walk(float d);

    void                    Pitch(float angle);
    void                    RotateY(float angle);

    void                    UpdateViewMatrix();
    void                    Update();

    std::vector<XMFLOAT4>   GetFrustumPlanes();

private:
    XMFLOAT3                m_pos;
    XMFLOAT3                m_right;
    XMFLOAT3                m_up;
    XMFLOAT3                m_look;

    float                   m_nearPlane;
    float                   m_farPlane;
    float                   m_aspectRatio;
    float                   m_fov;
    float                   m_nearHeight;
    float                   m_farHeight;

    XMFLOAT4X4              m_view;
    XMFLOAT4X4              m_proj;
};
