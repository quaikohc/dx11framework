

struct InputVS
{
    float3      pos       : POSITION;
    float2      tex       : TEXCOORD0;
    float3      normal    : NORMAL;
    float3      tangent   : TANGENT;
    float3      bitangent : BITANGENT;
};

cbuffer cbMeshParam : register(b0)
{
    matrix      g_world    ;  // : packoffset( c0 );
    matrix      g_worldIT  ;  // : packoffset( c4 );
    matrix      g_lightVP  ;  // : packoffset( c8 );
    matrix      g_view     ;  // : packoffset( c12 );
    matrix      g_proj     ;  // : packoffset( c16 );
    float       g_texScale ;  // : packoffset( c20 );
};

struct OutputTexVS
{
    float4 pos        : SV_POSITION;
    float4 shadow_pos : SHADOW_POSITION;
    float3 normal     : NORMAL;
    float2 uv         : UV;
};

//OutputTexVS main(InputVS IN)
//{
//    OutputTexVS	OUT;
//
//    float4 posW      = mul(float4(IN.pos.xyz, 1.0), g_world);
//
//    float4 posWVV    = mul(posW, g_view);
//
//    OUT.pos         = mul(posWVV, g_proj);
//    OUT.shadow_pos  = mul(posW, g_lightVP);
//    OUT.normal      = mul(IN.normal, (float3x3)g_worldIT);
//    OUT.uv          = IN.tex * g_texScale;
//
//    return OUT;
//}

InputVS main(InputVS IN)
{
    InputVS	OUT;

    OUT = IN;

    return OUT;
}

////////////////////////////////////////////////////////////////////////////



struct HS_OUT      // output control point
{
    float3 vPosition : WORLDPOS; 
    float3 NormalW  : NORMAL;
    float2 Tex      : TEXCOORD;
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
    Output.NormalW   = ip[i].normal;
    Output.Tex       = ip[i].tex;
    return Output;
}


/////////////////////////////////////////////////////////////////////////////////

// The domain shader is called for every vertex created by the tessellator.  

[domain("tri")]
OutputTexVS FinalDS(HS_CONSTANT_DATA_OUTPUT input, float3 domain : SV_DomainLocation, const OutputPatch<HS_OUT, 3> patch)
{
    OutputTexVS Output;

    Output.pos      = float4(patch[0].vPosition * domain.x + patch[1].vPosition * domain.y + patch[2].vPosition * domain.z, 1);
    Output.normal   = domain.x*patch[0].NormalW  + domain.y*patch[1].NormalW  + domain.z*patch[2].NormalW;
    Output.uv       = domain.x*patch[0].Tex      + domain.y*patch[1].Tex      + domain.z*patch[2].Tex;

    Output.pos  = mul(Output.pos, g_world);

    Output.shadow_pos  = mul(Output.pos, g_lightVP);

    Output.pos  = mul(Output.pos, g_view);
    
    Output.pos  = mul(Output.pos, g_proj);



    Output.normal      = normalize(mul(Output.normal, (float3x3)g_worldIT));
    Output.uv          = Output.uv * g_texScale;

    return Output;
}