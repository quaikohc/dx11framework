#include "stdafx.h"
#include "base_camera.h"



CCamera::CCamera() : m_pos(0.0f, 0.0f, 0.0f), m_right(1.0f, 0.0f, 0.0f), m_up(0.0f, 1.0f, 0.0f), m_look(0.0f, 0.0f, 1.0f)
{
    SetLens(0.25f * XM_PI, 1.0f, 1.0f, 1000.0f);
}

CCamera::~CCamera()
{
}

XMVECTOR CCamera::GetPositionXM() const
{
    return XMLoadFloat3(&m_pos);
}

XMFLOAT3 CCamera::GetPosition() const
{
    return m_pos;
}

void CCamera::SetPosition(float x, float y, float z)
{
    m_pos = XMFLOAT3(x, y, z);
}

void CCamera::SetPosition(const XMFLOAT3& v)
{
    m_pos = v;
}

XMVECTOR CCamera::GetRightXM() const
{
    return XMLoadFloat3(&m_right);
}

XMFLOAT3 CCamera::GetRight() const
{
    return m_right;
}

XMVECTOR CCamera::GetUpXM()const
{
    return XMLoadFloat3(&m_up);
}

XMFLOAT3 CCamera::GetUp() const
{
    return m_up;
}

XMVECTOR CCamera::GetLookXM() const
{
    return XMLoadFloat3(&m_look);
}

XMFLOAT3 CCamera::GetLook() const
{
    return m_look;
}

float CCamera::GetNearZ() const
{
    return m_nearPlane;
}

float CCamera::GetFarZ() const
{
    return m_farPlane;
}

float CCamera::GetAspect() const
{
    return m_aspectRatio;
}

float CCamera::GetFovY() const
{
    return m_fov;
}

float CCamera::GetFovX() const
{
    float halfWidth = 0.5f * GetNearWindowWidth();
    return 2.0f * atan(halfWidth / m_nearPlane);
}

float CCamera::GetNearWindowWidth()const
{
    return m_aspectRatio * m_nearHeight;
}

float CCamera::GetNearWindowHeight()const
{
    return m_nearHeight;
}

float CCamera::GetFarWindowWidth()const
{
    return m_aspectRatio * m_farHeight;
}

float CCamera::GetFarWindowHeight()const
{
    return m_farHeight;
}

void CCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
    m_fov         = fovY;
    m_aspectRatio = aspect;
    m_nearPlane   = zn;
    m_farPlane    = zf;

    m_nearHeight = 2.0f * m_nearPlane * tanf(0.5f * m_fov);
    m_farHeight  = 2.0f * m_farPlane  * tanf(0.5f * m_fov);

    XMMATRIX P = XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    XMStoreFloat4x4(&m_proj, P);
}

void CCamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
    XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
    XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
    XMVECTOR U = XMVector3Cross(L, R);

    XMStoreFloat3(&m_pos,   pos);
    XMStoreFloat3(&m_look,  L);
    XMStoreFloat3(&m_right, R);
    XMStoreFloat3(&m_up,    U);
}

void CCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
    XMVECTOR P = XMLoadFloat3(&pos);
    XMVECTOR T = XMLoadFloat3(&target);
    XMVECTOR U = XMLoadFloat3(&up);

    LookAt(P, T, U);
}

XMMATRIX CCamera::GetView()const
{
    return XMLoadFloat4x4(&m_view);
}

XMMATRIX CCamera::GetProj()const
{
    return XMLoadFloat4x4(&m_proj);
}

XMMATRIX CCamera::GetViewProj()const
{
    return XMMatrixMultiply(GetView(), GetProj());
}

void CCamera::Strafe(float d)
{
    XMVECTOR s = XMVectorReplicate(d);
    XMVECTOR r = XMLoadFloat3(&m_right);
    XMVECTOR p = XMLoadFloat3(&m_pos);
    XMStoreFloat3(&m_pos, XMVectorMultiplyAdd(s, r, p));
}

void CCamera::Walk(float d)
{
    XMVECTOR s = XMVectorReplicate(d);
    XMVECTOR l = XMLoadFloat3(&m_look);
    XMVECTOR p = XMLoadFloat3(&m_pos);
    XMStoreFloat3(&m_pos, XMVectorMultiplyAdd(s, l, p));
}

void CCamera::Pitch(float angle)
{
    // Rotate up and look vector about the right vector.
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

    XMStoreFloat3(&m_up,   XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
    XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
}

void CCamera::RotateY(float angle)
{
    // Rotate the basis vectors about the world y-axis.

    XMMATRIX R = XMMatrixRotationY(angle);

    XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
    XMStoreFloat3(&m_up,    XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
    XMStoreFloat3(&m_look,  XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
}

void CCamera::Update()
{
    UpdateViewMatrix();
}

void CCamera::UpdateViewMatrix()
{
    XMVECTOR R = XMLoadFloat3(&m_right);
    XMVECTOR U = XMLoadFloat3(&m_up);
    XMVECTOR L = XMLoadFloat3(&m_look);
    XMVECTOR P = XMLoadFloat3(&m_pos);

    // Keep camera's axes orthogonal to each other and of unit length.
    L = XMVector3Normalize(L);
    U = XMVector3Normalize(XMVector3Cross(L, R));

    // U, L already ortho-normal, so no need to normalize cross product.
    R = XMVector3Cross(U, L); 

    float x = -XMVectorGetX(XMVector3Dot(P, R));
    float y = -XMVectorGetX(XMVector3Dot(P, U));
    float z = -XMVectorGetX(XMVector3Dot(P, L));

    XMStoreFloat3(&m_right, R);
    XMStoreFloat3(&m_up,    U);
    XMStoreFloat3(&m_look,  L);

    m_view(0,0) = m_right.x; 
    m_view(1,0) = m_right.y; 
    m_view(2,0) = m_right.z; 
    m_view(3,0) = x;   

    m_view(0,1) = m_up.x;
    m_view(1,1) = m_up.y;
    m_view(2,1) = m_up.z;
    m_view(3,1) = y;  

    m_view(0,2) = m_look.x; 
    m_view(1,2) = m_look.y; 
    m_view(2,2) = m_look.z; 
    m_view(3,2) = z;   

    m_view(0,3) = 0.0f;
    m_view(1,3) = 0.0f;
    m_view(2,3) = 0.0f;
    m_view(3,3) = 1.0f;
}


std::vector<XMFLOAT4> CCamera::GetFrustumPlanes()
{
    std::vector<XMFLOAT4> planes(6);

    CXMMATRIX viewProjMatrix = GetViewProj();

    XMVECTOR row0 = viewProjMatrix.r[0];
    XMVECTOR row1 = viewProjMatrix.r[1];
    XMVECTOR row2 = viewProjMatrix.r[2];
    XMVECTOR row3 = viewProjMatrix.r[3];

    // Left
    planes[0].x = row0.m128_f32[3] + row0.m128_f32[0];
    planes[0].y = row1.m128_f32[3] + row1.m128_f32[0];
    planes[0].z = row2.m128_f32[3] + row2.m128_f32[0];
    planes[0].w = row3.m128_f32[3] + row3.m128_f32[0];

    // Right
    planes[1].x =row0.m128_f32[3] - row0.m128_f32[0];
    planes[1].y =row1.m128_f32[3] - row1.m128_f32[0];
    planes[1].z =row2.m128_f32[3] - row2.m128_f32[0];
    planes[1].w =row3.m128_f32[3] - row3.m128_f32[0];

    // Bottom
    planes[2].x = row0.m128_f32[3] + row0.m128_f32[1];
    planes[2].y = row1.m128_f32[3] + row1.m128_f32[1];
    planes[2].z = row2.m128_f32[3] + row2.m128_f32[1];
    planes[2].w = row3.m128_f32[3] + row3.m128_f32[1];

    // Top
    planes[3].x = row0.m128_f32[3] - row0.m128_f32[1];
    planes[3].y = row1.m128_f32[3] - row1.m128_f32[1];
    planes[3].z = row2.m128_f32[3] - row2.m128_f32[1];
    planes[3].w = row3.m128_f32[3] - row3.m128_f32[1];

    // Near
    planes[4].x = row0.m128_f32[2];
    planes[4].y = row1.m128_f32[2];
    planes[4].z = row2.m128_f32[2];
    planes[4].w = row3.m128_f32[2];

    // Far
    planes[5].x = row0.m128_f32[3] - row0.m128_f32[2];
    planes[5].y = row1.m128_f32[3] - row1.m128_f32[2];
    planes[5].z = row2.m128_f32[3] - row2.m128_f32[2];
    planes[5].w = row3.m128_f32[3] - row3.m128_f32[2];

    for(int i = 0; i < 6; ++i)
    {
        XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
        XMStoreFloat4(&planes[i], v);
    }

    return planes;
}