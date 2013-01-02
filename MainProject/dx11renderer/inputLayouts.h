#pragma once

enum EInputLayoutType
{
    Position,
    PositionTexture,
    PositionTextureNormal, 
    PositionTextureNormalTangentBinormal,
    PositionTextureNormalTangentSkinned,
    BasicParticle,
    SpriteLayout,
    TerrainLayout,
    ELayoutTypesCount
};

///////////////////////////////////////////////////////////////////////////

struct SVertexPos
{
    XMFLOAT3 pos;
};

static D3D11_INPUT_ELEMENT_DESC layoutPos[] =
{
    { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

///////////////////////////////////////////////////////////////////////////

struct SVertexPosTex
{
    XMFLOAT3    pos;
    XMFLOAT2    tex;
};

static D3D11_INPUT_ELEMENT_DESC layoutPosTex[] =
{
    { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

///////////////////////////////////////////////////////////////////////////

struct SVertexPosTexNorm
{
    XMFLOAT3    pos;
    XMFLOAT2    tex;
    XMFLOAT3    normal;
};

static D3D11_INPUT_ELEMENT_DESC layoutPosTexNorm[] =
{
    { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

///////////////////////////////////////////////////////////////////////////

struct SVertexPosTexNormalBitangentTangent
{
    SVertexPosTexNormalBitangentTangent(){}

    XMFLOAT3    pos;
    XMFLOAT2    tex;
    XMFLOAT3    normal;
    XMFLOAT3    bitangent;
    XMFLOAT3    tangent;
};

static D3D11_INPUT_ELEMENT_DESC layoutPosTexNormalBitangentTangent[] =
{
    { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

///////////////////////////////////////////////////////////////////////////

struct SVertexPosNormalTexTanSkinned
{
        XMFLOAT3    Pos;
        XMFLOAT3    Normal;
        XMFLOAT2    Tex;
        XMFLOAT4    TangentU;
        XMFLOAT3    Weights;
        BYTE        BoneIndices[4];
};

const D3D11_INPUT_ELEMENT_DESC layoutPosTexNormalTangentSkinned[6] = 
{
    {"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"WEIGHTS",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"BONEINDICES",  0, DXGI_FORMAT_R8G8B8A8_UINT,      0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

///////////////////////////////////////////////////////////////////////////

struct SVertexBasicParticle
{
    XMFLOAT3      Pos;
    XMFLOAT3      Vel;
    float         Life;
    float         Size;
};

const D3D11_INPUT_ELEMENT_DESC basicParticlelayout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "LIFE",     0, DXGI_FORMAT_R32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "SIZE",     0, DXGI_FORMAT_R32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

////////////////////////////////////////////////////////////////////////////

struct SVertexSprite
{
    XMFLOAT3     Pos;
    XMFLOAT2     Tex;
    XMCOLOR      Color;
};

D3D11_INPUT_ELEMENT_DESC layoutSprite[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_B8G8R8A8_UNORM,  0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

////////////////////////////////////////////////////////////////////////////

struct SVertexTerrain
{
    XMFLOAT3 Pos;
    XMFLOAT2 Tex;
    XMFLOAT2 BoundsY;
};

const D3D11_INPUT_ELEMENT_DESC  layoutTerrain[3]= 
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
};


////////////////////////////////////////////////////////////////////////////


union VertexData
{
    SVertexPos*                          vertexPos;
    SVertexPosTex*                       vertexPosTex; 
    SVertexPosTexNorm*                   vertexPosTexNorm;
    SVertexPosTexNormalBitangentTangent* vertexPosTexNormBitangentTangent;
    SVertexPosNormalTexTanSkinned*       vertexPosTexNormTangentSkinned;
    SVertexBasicParticle*                vertexBasicParticle;
    SVertexSprite*                       vertexSprite;
    SVertexTerrain*                      vertexTerrain;
};