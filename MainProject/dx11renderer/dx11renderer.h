#pragma once

#include "dx11device.h"
#include "../camera/base_camera.h"
#include "inputLayouts.h"
#include "../globals.h"
#include "debugDrawer.h"
#include "animationSystem/skinnedModel.h"
#include "particleSystem/particleSystem.h"
#include "animationSystem/assimpAnimationLoader.h"
#include "shadowMap.h"




using namespace DirectX;

class DxFont;
class CMesh;


enum EMaterialFlag
{
    HAS_DIFFUSE  = (1 << 0), 
    HAS_NORMAL   = (1 << 1), 
    HAS_SPECULAR = (1 << 2),
    HAS_HEIGHT   = (1 << 3)
};

enum EEffectType
{
    EBasic,
    EBasicTextured,
    EParallaxMapping
};

// For aligning to float4 boundaries
//#define Float4Align __declspec(align(16))
//
//struct CBPerMesh
//{
//        Float4Align D3DXVECTOR3 ObjectPosition;
//        Float4Align D3DXVECTOR3 CameraPosition;
//        Float4Align D3DXVECTOR2 CameraZoom;
//};
 // __declspec(align(16)) XMFLOAT3            eyePos;

struct SGlobalPerFrameCBuffer
{
    XMFLOAT4            startGuard;
    XMMATRIX            view;
    XMMATRIX            proj;
    XMMATRIX            lightViewProj;
    DirectionalLight    sunLight;
    XMFLOAT3            eyePos;
    float               pad;
    XMFLOAT4            endGuard;
};


struct SPerObjectCBuffer
{
    XMMATRIX      gWorld;
    XMMATRIX      gWorldInvTranspose;
    XMMATRIX      gWorldViewProj;
    XMMATRIX      gTexTransform;
    XMMATRIX      gShadowTransform; 
    Material      gMaterial;
};



struct SMaterialData
{
    std::wstring        name;

    std::wstring        diffusePath;
    std::wstring        specularPath;
    std::wstring        normalPath;
    std::wstring        heightPath;
};

struct SMeshData
{
    std::wstring        fileName;

    EInputLayoutType    type;
    VertexData          vertexData;
    size_t              size;

    uint*               indices;
    uint                verticesCount;
    uint                indicesCount;
};

class CModel;
class CMaterial;

class CDX11Renderer
{
    ID3D11VertexShader*     m_depthVS;
    ID3D11InputLayout*      m_depthInputLayout;

    XMFLOAT3                mOriginalLightDir;

    XMFLOAT4X4              mShadowTransform;

    ID3D11Buffer*           m_depthConstBuffer;

public:
    CDX11Renderer();
    ~CDX11Renderer();

    XMFLOAT4X4                      GetShadowTransform(){ return mShadowTransform; }
    void                            Draw();
    bool                            InitScene(CDx11Device* device);
    void                            InitGeometry();

    void                            InitSkinned();
    void                            DrawSkinned();
    void                            InitParticles();
    void                            DrawParticles();
    void                            BuildShadowTransform();

    CCamera                         m_camera;
    CDebugDrawer                    m_debugDrawer;

    D3D11_VIEWPORT                  viewport;
    ShadowMap*                      m_shadowMap;
    XMFLOAT4X4                      mLightView;
    XMFLOAT4X4                      mLightProj;

private:
    CDx11Device*                    m_device;

    void                            DrawModelsToShadowMap(CModel* model);
    void                            DrawModels(CModel* model);

    void                            CreateViewport();
    void                            InitShadowMap();


    ID3D11Buffer*                   m_perFrameGlobalCBuffer;
    ID3D11Buffer*                   m_perObjectBuffer;

    SkinnedModel*                   m_skinnedModel;
    SkinnedModelInstance            m_skinnedModelInstance;

    ParticleSystem                  m_testParticleSystem2;

};