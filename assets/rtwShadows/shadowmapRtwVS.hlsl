#define g_normalOffset   0.001f

Texture1D     texWarpX   : register(t0);
Texture1D     texWarpY   : register(t1);

SamplerState  samLinear  : register(s0);

struct InputVS
{
    float3 pos       : POSITION;
    float2 tex       : TEXCOORD0;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
};

struct OutputVS
{
    float4 pos         : SV_POSITION;
    float2 depth       : DEPTH;
};

cbuffer cbShadowParam : register(b0)
{
    matrix      g_lightVP;
    matrix      g_world;
};

float4 WarpVertex(float4 pos)
{
    float4 ret = pos;

    float2 cpos = (pos.xy / pos.w) * float2(0.5, -0.5) + 0.5;

    cpos += float2(texWarpX.SampleLevel(samLinear, cpos.x, 0).r, texWarpY.SampleLevel(samLinear, cpos.y, 0 ).r);

    ret.xy = (cpos.xy * float2(2.0, -2.0) - float2(1.0, -1.0)) * pos.w;

    return ret;
}

OutputVS main(InputVS IN)
{
    OutputVS outVert;

    float4 pos       = float4(IN.pos.xyz - IN.normal.xyz * g_normalOffset, 1.0);
   // float4 pos       = float4(IN.pos.xyz, 1.0);

    float4 posW      = mul(pos, g_world);
    float4 posWVP    = mul(posW, g_lightVP);

    outVert.pos      = WarpVertex(posWVP);
    outVert.depth.xy = outVert.pos.zw;

    return outVert;
}

InputVS main2(InputVS IN)
{
    InputVS	OUT;

    OUT = IN;

    return OUT;
}


///////////////////////////////////////////////

struct HS_OUT      // output control point
{
    float3 vPosition : WORLDPOS; 
};


struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3]			: SV_TessFactor;
    float InsideTessFactor			: SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants( InputPatch<InputVS, 3> ip, uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

    Output.EdgeTessFactor[0] = Output.EdgeTessFactor[1] = Output.EdgeTessFactor[2] = Output.InsideTessFactor = 15; 

    return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_OUT FinalHS(InputPatch<InputVS, 3> ip,  uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
    HS_OUT Output;

    Output.vPosition = ip[i].pos;

    return Output;
}



[domain("tri")]
OutputVS FinalDS(HS_CONSTANT_DATA_OUTPUT input, float3 domain : SV_DomainLocation, const OutputPatch<HS_OUT, 3> patch)
{
    OutputVS Output;

    float4 outPos =  float4(patch[0].vPosition * domain.x + patch[1].vPosition * domain.y + patch[2].vPosition * domain.z, 1);
    outPos = mul(outPos, g_world);
    outPos = mul(outPos, g_lightVP);

    Output.pos = outPos;
    Output.depth.xy = Output.pos.zw;

    return Output;
}
