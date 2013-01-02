
struct GSParticleIn
{
    float3 Pos    : POSITION;
    float Life    : LIFE;
    float Size    : SIZE;
};


cbuffer cbPerObject: register(cb0)
{
    float4x4      g_worldViewProj;
    float3        g_eyePosW;
};

Texture2D       g_txColorGradient;
SamplerState    g_samLinearClamp;


struct GeometryShaderOut
{
    float4 PosH           : SV_Position;
    float3 particleColor  : TEXCOORD0;
    float3 Tex            : TEXCOORD1;
};

[maxvertexcount(4)]
void main(point GSParticleIn input[1], inout TriangleStream<GeometryShaderOut> SpriteStream)
{
    if( input[0].Life > -1 )
    {
        float3 look  = normalize(g_eyePosW.xyz - input[0].Pos);
        float3 right = normalize(cross(float3(0, 1, 0), look));
        float3 up    = cross(look, right);
    
        float halfWidth  = input[0].Size * 0.5f;
        float halfHeight = input[0].Size * 0.5f;

        float4 v[4];
        v[0] = float4(input[0].Pos + halfWidth * right - halfHeight*up, 1.0f);
        v[1] = float4(input[0].Pos + halfWidth * right + halfHeight*up, 1.0f);
        v[2] = float4(input[0].Pos - halfWidth * right - halfHeight*up, 1.0f);
        v[3] = float4(input[0].Pos - halfWidth * right + halfHeight*up, 1.0f);

        GeometryShaderOut output;
        float3 particleColor = g_txColorGradient.SampleLevel( g_samLinearClamp, float2(input[0].Life, 0), 0 );
        output.particleColor = particleColor;

        //////////////////////////////////////
        //////////////////////////////////////

        output.PosH  = mul(v[0], g_worldViewProj);
        output.Tex = float3(float2(0, 0), input[0].Life);

        SpriteStream.Append(output);

        output.PosH  = mul(v[1], g_worldViewProj);
        output.Tex = float3(float2(1, 0), input[0].Life);

        SpriteStream.Append(output);

        output.PosH  = mul(v[2], g_worldViewProj);
        output.Tex = float3(float2(0, 1), input[0].Life);

        SpriteStream.Append(output);

        output.PosH  = mul(v[3], g_worldViewProj);
        output.Tex = float3(float2(1, 1), input[0].Life);

        SpriteStream.Append(output);
    }
}