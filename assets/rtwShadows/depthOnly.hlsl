#define kNormalOffset   0.001f

cbuffer cbPerObject
{
    float4x4 gWorldLightViewProj;
}; 

struct VertexIn
{
    float3 position  : POSITION;
    float2 texCoord  : TEXCOORD0;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VertexOut
{
    float4 PosH     : SV_POSITION;
};
 
VertexOut main(VertexIn vin)
{
    VertexOut vout;

    float4 pos = float4( vin.position.xyz - vin.normal.xyz * kNormalOffset, 1.0 );
    vout.PosH = mul(pos, gWorldLightViewProj);

    return vout;
}