

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};

cbuffer cbPerFrame: register(cb0)
{
    float4              startTestValuePerFrame;
    DirectionalLight    gDirLight;
    float3              gEyePosW;
    float               pad;
    float4              endTestValuePerFrame;
};

cbuffer cbPerObject: register(cb1)
{
    float4              startTestValuePerObject;
    float4x4            gWorld;
    float4x4            gWorldInvTranspose;
    float4x4            gWorldViewProj;
    float4x4            gWorldViewProjTex;
    float4x4            gTexTransform;
    float4x4            gBoneTransforms[96];
    Material            gMaterial;
    float4              endTestValuePerObject;
}; 

//cbuffer cbSkinned : register(cb2)
//{
//    
//};


struct SkinnedVertexIn
{
    float3 PosL       : POSITION;
    float3 NormalL    : NORMAL;
    float2 Tex        : TEXCOORD;
    float4 TangentL   : TANGENT;
    float3 Weights    : WEIGHTS;
    uint4 BoneIndices : BONEINDICES;
};

struct VertexOut
{
    float4 PosH       : SV_POSITION;
    float3 PosW       : POSITION;
    float3 NormalW    : NORMAL;
    float4 TangentW   : TANGENT;
    float2 Tex        : TEXCOORD0;
};


VertexOut main(SkinnedVertexIn vin)
{
    VertexOut vout;

    float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    weights[0] = vin.Weights.x;
    weights[1] = vin.Weights.y;
    weights[2] = vin.Weights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 posL     = float3(0.0f, 0.0f, 0.0f);
    float3 normalL  = float3(0.0f, 0.0f, 0.0f);
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);

    for(int i = 0; i < 4; ++i)
    {
        posL     += weights[i] * mul(float4(vin.PosL, 1.0f),            gBoneTransforms[vin.BoneIndices[i]]).xyz;
        normalL  += weights[i] * mul(vin.NormalL,            (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
        tangentL += weights[i] * mul(vin.TangentL.xyz,       (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
    }
 
    vout.PosW     = mul(float4(posL, 1.0f), gWorld).xyz;
    vout.NormalW  = mul(normalL, (float3x3)gWorldInvTranspose);
    vout.TangentW = float4(mul(tangentL, (float3x3)gWorld), vin.TangentL.w);

    vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);
    
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

    return vout;
}
 