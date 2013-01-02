#pragma once

// work in progress



class CTransform
{
    bool        m_needUpdate;

    XMFLOAT3    m_pos;
    XMFLOAT3    m_rot;
    XMFLOAT3    m_scale;

    XMMATRIX    m_world;

public:
    CTransform(): m_needUpdate(true), m_pos(0.0f, 0.0f, 0.0f), m_scale(1.0f, 1.0f, 1.0f), m_rot(0.0f, 0.0f, 0.0f), m_world(XMMatrixIdentity())
    {

    }
    CTransform(XMFLOAT3 pPos, XMFLOAT3 pRot, XMFLOAT3 pScale = XMFLOAT3(1.0f, 1.0f, 1.0f)): m_needUpdate(true), m_pos(pPos), m_scale(pScale), m_rot(pRot), m_world(XMMatrixIdentity())//, XMFLOAT3 pScale = XMFLOAT3(1.0f, 1.0f, 1.0f)) : m_needUpdate(true), m_pos(pPos), m_rot(pRot), m_world(XMMatrixIdentity())
    {
    }
    CTransform(const CTransform& t)
    {
    }
    CTransform(const CTransform&& t)
    {
    }

      /*
    CTransform(const CTransform& p): ptr(p.ptr ? p.ptr->clone() : 0)
    {
    }

    CTransform& operator=(const CTransform& p)
    {
        if (this != &p)
        {
            delete ptr;
            ptr = p.ptr ? p.ptr->clone() : 0;
        }
        return *this;
    }

    // move semantics
    CTransform(CTransform&& p) : ptr(p.ptr) {p.ptr = 0;}

    CTransform& operator=(CTransform&& p)
    {
        std::swap(ptr, p.ptr);
        return *this;
    }
    */
    //CTransform& operator=(const CTransform& t);
   //CTransform& operator*=(const CTransform& t);

    XMMATRIX GetWorld()
    {
        if(m_needUpdate)
            Update();
        return m_world;
    }

    const XMFLOAT3& GetPos() const
    {
        return m_pos;
    }

    const XMFLOAT3& GetRot() const
    {
        return m_rot;
    }

    const XMFLOAT3& GetScale() const
    {
        return m_scale;
    }

    XMVECTOR GetPosVec() const
    {
        return XMLoadFloat3(&m_pos);
    }

    XMVECTOR GetRotVec() const
    {
        return XMLoadFloat3(&m_rot);
    }

    XMVECTOR GetScaleVec() const
    {
        return XMLoadFloat3(&m_scale);
    }

    void SetPos(XMFLOAT3 pPos)
    {
        m_pos = pPos;
        m_world.r[3].m128_f32[0] = pPos.x;
        m_world.r[3].m128_f32[1] = pPos.y;
        m_world.r[3].m128_f32[2] = pPos.z;
    }

    void SetRot(XMFLOAT3 pRot, bool inRadians = true)
    {
        if(inRadians)
            m_rot = pRot;
        else
            m_rot = XMFLOAT3(DegToRad(pRot.x), DegToRad(pRot.y), DegToRad(pRot.z));

        m_needUpdate = true;
    }

    void SetScale(XMFLOAT3 pScale)
    {
        m_scale = pScale;
        m_needUpdate = true;
    }

    void Update()
    {
        m_world =  XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z);// * XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
   
        m_world.r[3].m128_f32[0] = m_pos.x;
        m_world.r[3].m128_f32[1] = m_pos.y;
        m_world.r[3].m128_f32[2] = m_pos.z;

        m_needUpdate = false;
    }

};





/*    

XMVECTOR scale = {0,0,0,0};

      //  XMMatrixRotationQuaternion    Builds a rotation matrix from a quaternion.

typedef
ALIGNED(16)
struct entity_info_t {
    entity_number_t entity_number;
    uint16_t        type;

    uint32_t        flags;
    uint32_t        time;

    void            (*update)(struct entity_s* ent, float frame_change);
    void            (*post_physics_update)(struct entity_s* ent, float frame_change);

    uint32_t        _padding[3];
} entity_info_t; // 16B


typedef
ALIGNED(16)
struct entity_position_state_s {
    XMFLOAT4A   position;
    XMFLOAT4A   velocity;
    XMFLOAT4A   orientation;

    // Coords of room
    room_id_t room_id;
} entity_position_state_t; // 48B


typedef
ALIGNED(16)
struct entity_collision_info_s {
    aabb_t      bounds;
    void        (*trace_response)(struct entity_s* ent, float frame_change, const trace_result_t* trace_result);

    uint32_t    _padding[1];
} entity_collision_info_t; // 8B
*/
/*

 XMFLOAT4X4A* curr_matrix = out_matrices;

    for(uint32_t index=0; index<E_MOBILE_ENTITY_STATE_POOL_SIZE; ++index, ++curr_matrix) {
        const XMVECTOR position = XMLoadFloat3(&g_mobile_entities.positions[ index ]);
        const XMVECTOR orientation = XMLoadFloat4(&g_mobile_entities.orientations[ index ]);
        const XMVECTOR scale = XMVectorReplicate(g_mobile_entities.scales[ index ]);
        const XMMATRIX out_matrix = XMMatrixAffineTransformation(scale, g_XMZero, orientation, position);
        XMStoreFloat4x4A(curr_matrix, out_matrix);
    }

    // build the final static entity matrices

    for(uint32_t index=0; index<E_STATIC_ENTITY_STATE_POOL_SIZE; ++index, ++curr_matrix) {
        const XMVECTOR position = XMLoadFloat3(&g_static_entities.positions[ index ]);
        const XMVECTOR orientation = XMLoadFloat4(&g_static_entities.orientations[ index ]);
        const XMVECTOR scale = XMVectorReplicate(g_static_entities.scales[ index ]);
        const XMMATRIX out_matrix = XMMatrixAffineTransformation(scale, g_XMZero, orientation, position);
        XMStoreFloat4x4A(curr_matrix, out_matrix);
    }







     const XMVECTOR position = XMLoadFloat3(&g_missile_entities.positions[ index ]);
        const XMVECTOR direction = XMLoadFloat3(&g_missile_entities.directions[ index ]);
        const XMVECTOR orientation = XMQuaternionRotationNormal( direction, 0.0f );
        const XMMATRIX out_matrix = XMMatrixAffineTransformation( g_XMOne, g_XMZero, orientation, position );
*/


//+void DPMatrix::transformation(python::object& tr, python::object& rot, python::object& scale, 
//+    python::object& rorigin, python::object& sorigin, python::object& sorientation)
//+{
//+    XMVECTOR trans = PyToXMV(tr);
//+    XMVECTOR rotQuat= XMQuaternionRotationRollPitchYawFromVector(PyToXMV(rot));
//+    XMVECTOR scaling = PyToXMV(scale);
//+
//+    XMVECTOR rotOrigin = PyToXMV(rorigin);
//+    XMVECTOR scalingOrigin = PyToXMV(sorigin);
//+    XMVECTOR scalingOrientationQuat = XMQuaternionRotationRollPitchYawFromVector(PyToXMV(sorientation));
//+
//+    matrix = XMMatrixTransformation(scalingOrigin, scalingOrientationQuat, 
//+        scaling, rotOrigin, rotQuat, trans);
//+}


//matrix44::transformation(float4 const &scalingCenter, const quaternion& scalingRotation, float4 const &scaling, float4 const &rotationCenter, const quaternion& rotation, float4 const &translation)
//{
//    return XMMatrixTransformation(scalingCenter.vec,
//                                  scalingRotation.vec,
//                                  scaling.vec,
//                                  rotationCenter.vec,
//                                  rotation.vec,
//                                  translation.vec);
//}
//
//matrix44
//matrix44::affinetransformation(scalar scaling, float4 const &rotationCenter, const quaternion& rotation, float4 const &translation)
//{
//    return XMMatrixAffineTransformation(XMVectorReplicate(scaling), rotationCenter.vec, rotation.vec, translation.vec);
//}


////////////////////////////////////////
/*

class CTransform
{
    bool                m_needUpdate;

    XMFLOAT3 position;

    XMVECTOR            m_pos;
    XMVECTOR            m_scale;
    XMVECTOR            m_rotation;
    XMMATRIX            m_world;

public:
    CTransform() : m_needUpdate(true), m_pos(XMVectorZero()), m_scale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)), m_rotation(XMVectorZero()), m_world(XMMatrixIdentity())
    {
    }
    CTransform(XMVECTOR pos, XMVECTOR rot) : m_needUpdate(true), m_pos(pos), m_scale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)), m_rotation(rot), m_world(XMMatrixIdentity())
    {
    }

    void SetPos(XMFLOAT3 pos)
    {
        position = pos;
        m_needUpdate = true;
    }

    //void SetPos(XMVECTOR pos)
    //{
    //    m_pos = pos;
    //    m_needUpdate = true;
    //}
    void SetRot(XMVECTOR rot)
    {
        m_rotation = rot;
        m_needUpdate = true;
    }

    void SetScale(XMVECTOR scale) 
    {
        m_scale = scale;
        m_needUpdate = true;
    }
    
    void SetFromEuler(float yaw, float pitch, float roll)
    {
       m_world = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
    }

    XMMATRIX GetWorld()
    {
        if(m_needUpdate)
            Update();

        return m_world;
    }

    void Update()
    {
       // XMMATRIX tmpTranslation = XMMatrixTranslation(m_pos.m128_f32[0], m_pos.m128_f32[1], m_pos.m128_f32[2]);
       /// XMMATRIX tmpScaling     = XMMatrixScaling(m_scale.m128_f32[0], m_scale.m128_f32[1], m_scale.m128_f32[2]);
       // XMMATRIX tmpRotation    = XMMatrixRotationRollPitchYaw(m_rotation.m128_f32[0], m_rotation.m128_f32[1], m_rotation.m128_f32[2]);

        //XMMATRIX tmpRotation    = XMMatrixRotationQuaternion(m_rotation);

      //  m_world = tmpScaling * tmpRotation * tmpTranslation;
        m_world = XMMatrixTranslation(position.x, position.y, position.z);
        m_needUpdate = false;
    }

};



*/




/*
void SampleRenderer::convertToD3D11(XMFLOAT3& dxvec, const PxVec3& vec)
{
    dxvec.x = vec.x;
    dxvec.y = vec.y;
    dxvec.z = vec.z;
}

void SampleRenderer::convertToD3D11(XMFLOAT4X4& dxmat, const physx::PxMat44 &mat)
{
    PxMat44 mat44 = mat.getTranspose();
    memcpy(&dxmat._11, mat44.front(), 4 * 4 * sizeof (float));
}

void SampleRenderer::convertToD3D11(XMMATRIX& dxmat, const physx::PxMat44 &mat)
{
    PxMat44 mat44 = mat.getTranspose();
    memcpy(&dxmat._11, mat44.front(), 4 * 4 * sizeof (float));
}
*/


/*
inline XMMATRIX TOXMMATRIX(aiMatrix4x4 transform)
{
    return XMMatrixSet(transform.a1,transform.a2,transform.a3,transform.a4,
        transform.b1,transform.b2,transform.b3,transform.b4,
        transform.c1,transform.c2,transform.c3,transform.c4,
        transform.d1,transform.d2,transform.d3,transform.d4);				
}

inline XMFLOAT4X4 TOXMFLOAT4X4(aiMatrix4x4 transform)
{
    return XMFLOAT4X4(transform.a1,transform.a2,transform.a3,transform.a4,
        transform.b1,transform.b2,transform.b3,transform.b4,
        transform.c1,transform.c2,transform.c3,transform.c4,
        transform.d1,transform.d2,transform.d3,transform.d4);			
}
*/