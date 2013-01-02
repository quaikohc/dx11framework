
// for adjusting shadow bias
#define g_normalOffset   0.001f

struct VertexIn
{
    float3 pos       : POSITION;
    float2 tex       : TEXCOORD0;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
};


struct OutputVS
{
    float4  pos         : SV_POSITION;
    float4  shadow_pos  : SHADOW_POS;
    float4  posWVP      : POSWVP;
    float3  view_dir    : VIEW_DIR;
    float3  normal      : NORMAL;
    float   depth       : DEPTH;
};

cbuffer cbSceneParam : register(b0)
{
    matrix  g_view;
    matrix  g_proj;
    matrix  g_world;
    matrix  g_lightVP;
    float4  screenParam;
};

OutputVS main(VertexIn IN)
{
    OutputVS  OUT = (OutputVS)0;

    float4 posW;
    float4 posWV;

    posW    = mul(float4(IN.pos /*- IN.normal.xyz * g_normalOffset*/, 1.0f), g_world);
    posWV   = mul(posW, g_view);

    OUT.posWVP    = mul(posWV, g_proj);
    OUT.view_dir  = normalize(posWV.xyz);
    OUT.depth     = (-posWV.z - screenParam.x) / (screenParam.y - screenParam.x);
   //OUT.depth     = -posWV.z  / screenParam.y;

    OUT.pos = mul(posW, g_lightVP);

    OUT.shadow_pos = OUT.pos;


    OUT.normal = mul(IN.normal, (float3x3)g_world);
    OUT.normal = mul(OUT.normal, g_view);
    
    return OUT;
}
