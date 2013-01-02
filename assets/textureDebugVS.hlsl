
cbuffer buff
{
    float4x4  gWorldViewProj;
};

struct VertexIn
{
    float3 PosL    : POSITION;
    float2 Tex     : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex  : TEXCOORD;
};
 
VertexOut main(VertexIn vin)
{
    VertexOut vout;

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    vout.Tex  = vin.Tex;
    
    return vout;
}
