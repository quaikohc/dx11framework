
#include "common.hlsl"

cbuffer cvPerObject : register(cb1)
{
    matrix world;
};


struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

cbuffer perFrameGlobalBuffer : register(cb0)
{
    float4              startGuard;
    matrix              view;
    matrix              proj;
    matrix              lightViewProj;
    DirectionalLight    sunLight;
    float3              eyePos;
    float pad;
    float4              endGuard;
};



cbuffer cbPerObjectParallax : register(cb1)
{
    matrix      gWorld;
    matrix      gWorldInvTranspose;
    matrix      gWorldViewProj;
    matrix      gTexTransform;
    matrix      gShadowTransform; 
    Material    gMaterial;
}; 

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    output.Pos = mul( input.Pos, world );
    output.Pos = mul( output.Pos, view );
    output.Pos = mul( output.Pos, proj );
    output.Tex = input.Tex;
    
    return output;
}
