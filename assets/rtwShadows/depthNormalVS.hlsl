

struct VertexIn
{
    float3 position  : POSITION;
    float2 texCoord  : TEXCOORD0;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
};

struct OutputVS
{
    float4  pos         : SV_POSITION;
    float3  normal      : NORMAL;
    float   depth       : DEPTH;
};

cbuffer constBuffer 
{
    matrix  g_view;
    matrix  g_proj;
    matrix  g_world;
    matrix  g_worldIT;
    float4  screenParam;
};



OutputVS main( VertexIn IN )
{
    OutputVS OUT = (OutputVS)0;
    
    float4 posW;
    float4 posWV;

    posW       = mul(float4(IN.position, 1.0f), g_world);
    posWV      = mul(posW, g_view);

    OUT.pos    = mul(posWV, g_proj);

    // (-depthVS - nearPlane) / (farPlane - nearPlane)
    OUT.depth  = (-posWV.z - screenParam.x) / (screenParam.y - screenParam.x); // y - far plane, x - near plane

    OUT.normal = mul(IN.normal, (float3x3)g_world);
    OUT.normal = mul(OUT.normal, g_view);
    
    return OUT;
}




