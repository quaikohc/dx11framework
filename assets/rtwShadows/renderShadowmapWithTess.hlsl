

Texture1D    texWarpX        : register( t2 );
Texture1D    texWarpY        : register( t3 );

SamplerState samPoint        : register( s0 );
SamplerState samLinear       : register( s1 );


struct InputVS
{
    float4	pos			: IN_POSITION;
    float3	normal		: IN_NORMAL;
};

struct InputTexVS
{
    float4	pos			: IN_POSITION;
    float3	normal		: IN_NORMAL;
    float2	uv			: IN_UV;
};

struct OutputShadowVS
{
    float4	pos			: SV_POSITION;
    float2	depth		: DEPTH;
};

cbuffer cbTessParam : register( b0 )
{
    float	g_TessFactor;
};

struct HullConstantValue
{
    float	tessFactor[3]	: SV_TessFactor;
    float	insideFactor	: SV_InsideTessFactor;
};

cbuffer cbShadowParam : register( b3 )
{
    float4x4	mtxShadowVP;
};





float4 WarpRtwVertex( float4 pos )
{
    float4 ret = pos;

    float2 cpos = (pos.xy / pos.w) * float2(0.5, -0.5) + 0.5;

    cpos += float2(
        texWarpX.SampleLevel( samLinear, cpos.x, 0 ).r,
        texWarpY.SampleLevel( samLinear, cpos.y, 0 ).r );

    ret.xy = (cpos.xy * float2(2.0, -2.0) - float2(1.0, -1.0)) * pos.w;

    return ret;
}

float CalcAddaptiveTessFactor( float2 dir )
{
    dir = abs( dir );
    float maxValue = max( dir.x, dir.y );
    float tess = floor( maxValue );

    if (tess < maxValue) 
        tess += 1.0;

    tess = max( 2, min( tess, 16 ) );

    return tess;
}


HullConstantValue RenderHSConst( InputPatch<InputVS, 3> inPatch )
{
    HullConstantValue hsCV;

    hsCV.tessFactor[0] = hsCV.tessFactor[1] = hsCV.tessFactor[2] = g_TessFactor;
    hsCV.insideFactor = g_TessFactor;

    return hsCV;
}

HullConstantValue RenderAddaptiveHSConst( InputPatch<InputTexVS, 3> inPatch )
{
    HullConstantValue hsCV;

    hsCV.tessFactor[0] = CalcAddaptiveTessFactor( inPatch[1].uv - inPatch[0].uv );
    hsCV.tessFactor[1] = CalcAddaptiveTessFactor( inPatch[2].uv - inPatch[1].uv );
    hsCV.tessFactor[2] = CalcAddaptiveTessFactor( inPatch[0].uv - inPatch[2].uv );

    hsCV.insideFactor = max( hsCV.tessFactor[0], max( hsCV.tessFactor[1], hsCV.tessFactor[2] ) );

    return hsCV;
}


[domain("tri")]					
[partitioning("integer")]		
[outputtopology("triangle_cw")]	
[outputcontrolpoints(3)]
[patchconstantfunc("RenderHSConst")]
InputVS RenderHS( InputPatch<InputVS, 3> inPatch, uint pointID : SV_OutputControlPointID )
{
    return inPatch[pointID];
}

[domain("tri")]					
[partitioning("integer")]		
[outputtopology("triangle_cw")]	
[outputcontrolpoints(3)]
[patchconstantfunc("RenderAddaptiveHSConst")]
InputVS RenderAddaptiveHS(InputPatch<InputTexVS, 3> inPatch, uint pointID : SV_OutputControlPointID)
{
    InputVS outVert;

    outVert.pos = inPatch[pointID].pos;
    outVert.normal = inPatch[pointID].normal;

    return outVert;
}

// The domain shader is called for every vertex created by the tessellator.  
[domain("tri")]
OutputShadowVS RenderRtwShadowDS(HullConstantValue inConst, float3 domainLocation : SV_DomainLocation, const OutputPatch<InputVS, 3> inPatch)
{
    OutputShadowVS outVert;

    float4 posW   = inPatch[0].pos * domainLocation.z + inPatch[1].pos * domainLocation.x + inPatch[2].pos * domainLocation.y;
    float4 posWVP = mul( posW, mtxShadowVP );

    outVert.pos = WarpRtwVertex(posWVP);

    outVert.depth.xy = outVert.pos.zw;

    return outVert;
}




