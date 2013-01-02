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


cbuffer perFrameGlobalBuffer : register(cb1)
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




struct VS_OUT
{
    float4 PosH       : SV_POSITION;
    float3 PosW       : POSITION;
    float3 NormalW    : NORMAL;
    float2 Tex        : TEXCOORD0;
    float4 ShadowPosH : TEXCOORD1;
};

SamplerState basicSampler     : register(s0);
SamplerComparisonState shadowMapSampler : register(s1);

Texture2D diffuseTex          : register(t0);
Texture2D shadowMapTex        : register(t1);


float4 main(VS_OUT pin) : SV_Target
{
    pin.NormalW = normalize(pin.NormalW);
    
    float3 toEye = eyePos - pin.PosW;
    
    float distToEye = length(toEye);    // Cache the distance to the eye from this surface point.
    
    toEye /= distToEye;
    
    float4 texColor = float4(1, 1, 1, 1);

    texColor = diffuseTex.Sample( basicSampler, pin.Tex );

    float4 litColor = texColor;

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float shadow = 1.0f;
    shadow = CalcShadowFactor(shadowMapSampler, shadowMapTex, pin.ShadowPosH);

    float4 A, D, S;
    ComputeDirectionalLight(gMaterial, sunLight, pin.NormalW, toEye, A, D, S);

    ambient += A;    
    diffuse += shadow * D;
    spec    += shadow * S;

    litColor = texColor * (ambient + diffuse) + spec;
    
 
    litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
}