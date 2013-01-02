#include "common.hlsl"


cbuffer cbPerObjectParallax : register(cb0)
{
    matrix      gWorld;
    matrix      gWorldInvTranspose;
    matrix      gWorldViewProj;
    matrix      gTexTransform;
    matrix      gShadowTransform; 
    Material    gMaterial;
}; 



struct VS_IN
{
    float3 pos       : POSITION;
    float2 tex       : TEXCOORD0;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_OUT
{
    float4 PosH       : SV_POSITION;
    float3 PosW       : POSITION;
    float3 NormalW    : NORMAL;
    float2 Tex        : TEXCOORD0;
    float4 ShadowPosH : TEXCOORD1;
};


VS_OUT main(VS_IN IN)
{
    VS_OUT output;

    output.PosW    = mul(float4(IN.pos, 1.0f), gWorld).xyz;
    output.NormalW = mul(IN.normal, (float3x3)gWorldInvTranspose);
    	
    output.PosH = mul(float4(IN.pos, 1.0f), gWorldViewProj);    // to CS
    
    output.Tex = mul(float4(IN.tex, 0.0f, 1.0f), gTexTransform).xy;
    
    output.ShadowPosH = mul(float4(IN.pos, 1.0f), gShadowTransform);

    return output;
}
