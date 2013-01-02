
SamplerState samPoint        : register( s0 );
SamplerState samLinear       : register( s1 );
SamplerState samLinearClamp  : register( s2 );

Texture2D    texAlbedo       : register( t0 );
Texture2D    texShadowBuffer : register( t1 );
Texture1D    texWarpX        : register( t2 );
Texture1D    texWarpY        : register( t3 );


struct OutputTexVS
{
    float4  pos         : SV_POSITION;
    float4  shadow_pos  : SHADOW_POSITION;
    float3  normal      : NORMAL;
    float2  uv          : UV;
};

cbuffer cbDLightParam : register(b0)
{
    float4  lightDir;
};


float3 CalcRtwShadow(float4 pos)
{
    float2 shadowUV = (pos.xy / pos.w) * float2(0.5, -0.5) + 0.5;

    shadowUV += float2( texWarpX.SampleLevel(samLinearClamp, shadowUV.x, 0).r, texWarpY.SampleLevel(samLinearClamp, shadowUV.y, 0).r);

    float mapDepth   = texShadowBuffer.Sample(samPoint, shadowUV).r;
    float modelDepth = pos.z / pos.w;

    if(modelDepth < mapDepth + 0.003f)       // 0.015 is good but self shadows are fucked
    {
        return float3(1, 1, 1); // pixel is not in shadow
    }
    return float3(0, 0, 0);
}


[earlydepthstencil]
float4 main(OutputTexVS IN) : SV_TARGET
{
  //  float l = saturate(dot(IN.normal, lightDir.xyz));

    float4  albedo = texAlbedo.Sample(samLinear, IN.uv);
   // albedo.xyz *= l;

    float3 color = albedo.xyz * CalcRtwShadow(IN.shadow_pos);

    return float4(color, 1.0);
}
