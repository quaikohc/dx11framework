#define kNormalOffset   0.001f

struct InputVS
{
    float4  pos         : POSITION;
    float3  normal      : NORMAL;
};

struct OutputVS
{
    float4  pos         : SV_POSITION;
    float2  depth       : DEPTH;
};

cbuffer cbShadowParam : register( b0 )
{
    float4 mtxShadowVP[4]  : packoffset( c0 );
};

cbuffer cbMeshParam : register( b1 )
{
    float4 mtxWorld[3]     : packoffset( c0 );
    float4 mtxWorldIt[3]   : packoffset( c3 );
};


OutputVS main( InputVS inVert )
{
    OutputVS    outVert;

    float4 pos = float4( inVert.pos.xyz - inVert.normal.xyz * kNormalOffset, 1.0 );

    float4 posW = float4( dot( mtxWorld[0], pos ),
                          dot( mtxWorld[1], pos ),
                          dot( mtxWorld[2], pos ), 1.0 );

    outVert.pos.x = dot( mtxShadowVP[0], posW );
    outVert.pos.y = dot( mtxShadowVP[1], posW );
    outVert.pos.z = dot( mtxShadowVP[2], posW );
    outVert.pos.w = dot( mtxShadowVP[3], posW );

    outVert.depth.xy = outVert.pos.zw;

    return outVert;
}

float4 RenderShadowPS( OutputVS inPixel ) : SV_TARGET
{
    float depth = inPixel.depth.x / inPixel.depth.y;
    return float4( depth, depth, depth, 1.0 );
}