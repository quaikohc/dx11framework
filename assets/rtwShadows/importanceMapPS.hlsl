
//Texture2D       depthNormalTex  : register(t0);
 //Texture2D       shadowTex       : register(t0); 

//SamplerState    samPoint        : register(s0);


struct OutputVS
{
    float4  pos         : SV_POSITION;
    float4  shadow_pos  : SHADOW_POS;
    float4  posWVP      : POSWVP;
    float3  view_dir    : VIEW_DIR;
    float3  normal      : NORMAL;
    float   depth       : DEPTH;
};

cbuffer cbImportanceParam : register( b0 )
{
    float   g_minEdgeValue;
    float   g_edgeThreshold;
    float   g_normalMultiValue;
    float   g_depthOffset;
};




////////////////////////////////////////////

/*
float ShadowEdgeFunction(float2 uv)
{
    float center = shadowTex.SampleLevel(samPoint, uv, 0).x;

    int2 neighbor[8] = {
        int2(-1, -1),
        int2( 0, -1),
        int2( 1, -1),
        int2(-1,  0),
        int2( 1,  0),
        int2(-1,  1),
        int2( 0,  1),
        int2( 1,  1),
    };

    float maxVal = 0.0;
    for (int i = 0; i < 8; ++i)
    {
        float val = shadowTex.SampleLevel( samPoint, uv, 0, neighbor[i] ).x;
        if (abs(val - center) >= g_edgeThreshold)
        {
            return 1.0;
        }
        maxVal = max( maxVal, val );
    }

    if (maxVal >= 1.0) { return g_minEdgeValue * 0.1; }

    return g_minEdgeValue;
}
*/

/*
Surface Normal Function
surface at some fixed distance away from the desired view has a maximal projected screen space size when pointing towards view direction
as the surface is rotated the projected screen space surface area becomes smaller until it reaches zero when the surface is perpendicular to they rays sampling it
function gives bonus importance (0..2) to surfaces pointing towards view direction
*/
float SurfaceNormalFunction( float3 normal, float3 view_dir )
{
    return 1.0 + g_normalMultiValue * 12.0f * saturate( dot( -normal, view_dir ) ); // TODO: try some huge value
}

/*
**** Distance to Eye Function
With a perspective projection for the desired view, shadow map regions farther from the eye of the desired view will have lower sampling needs since they 
are smaller on the output image plane. Given a sphere that is q distance from the eye along the view direction, the size of the projected sphere will be proportional to 1/q.

Since the depth value produce by the projection matrix is analogous to 1/q, that is exactly the value we use. 

For forward analysis, the importance value is found by transforming the texel from the light space (ML) to the desired view space (MD),
Id(u, v) = 1 - (Md * Ml(-1) * [u, v, depth(u, v)']z

for backward analysis importance can be found directly Id(u,v) = 1 - depth(u, v)

The resulting function will have importance values in the range [0, 2] for valid points
*/
float DistanceToEyeFunction( float depth )
{
    float ret =  1.0 - (-depth * 8.0f);
    return ret > 0.1f? ret : 0.1f;
}



// scene is rendered from light perspective, In.pos is in light space
// shadow_pos is also in light space without divide by w (homogenous clip space)
// IN.depth is in camera view space
float4 main(OutputVS IN) : SV_TARGET0
{
    float importance = 0.2;

    if (IN.depth >= 0.0 )
    {
        discard;
    }


    // position (clip space) to texture space
    float2 desired_uv = (IN.posWVP.xy / IN.posWVP.w) * float2(0.5, -0.5) + float2(0.5, 0.5);

    if ((desired_uv.x < 0.0) || (1.0 < desired_uv.x) || (desired_uv.y < 0.0) || (1.0 < desired_uv.y))
    {
        discard;
    }

   
    importance *= DistanceToEyeFunction( IN.depth ); // view space
    
    importance *= SurfaceNormalFunction( normalize(IN.normal), normalize(IN.view_dir) );



    return float4(importance, 0, 0, 0);
}









/*
// backward (only uses depthNormal texture)
float4 main(OutputVS IN) : SV_TARGET0
{
    float importance = 1.0f;

    if(IN.depth < 0.0f)
    {
      //  discard;
    }
    if(IN.depth >= 1.0f)
    {
        discard;
    }

    float2 desired_uv = (IN.posWVP.xy / IN.posWVP.w) * float2(0.5, -0.5) + float2(0.5, 0.5);

    if ((desired_uv.x < 0.0) || (1.0 < desired_uv.x) || (desired_uv.y < 0.0) || (1.0 < desired_uv.y))
    {
        discard;
    }

    float4 desired_nd = depthNormalTex.SampleLevel(samPoint, desired_uv, 0);

    if (IN.depth > desired_nd.w + g_depthOffset)
    {
         discard;
    }

    importance *= SurfaceNormalFunction(desired_nd.xyz, normalize(IN.view_dir));

    importance *= DistanceToEyeFunction(IN.depth);

    return float4(importance, 0, 0, 0);
}
*/
 ////// forward
 /*
float4 main( OutputVS IN ) : SV_TARGET0
{
    float importance = 1.0;

    if ((IN.depth < 0.0) || (1.0 <= IN.depth))
    {
       // discard;
    }

    float2 desired_uv = (IN.posWVP.xy / IN.posWVP.w) * float2(0.5, -0.5) + float2(0.5, 0.5);
    if ((desired_uv.x < 0.0) || (1.0 < desired_uv.x) || (desired_uv.y < 0.0) || (1.0 < desired_uv.y))
    {
        discard;
    }

    float4 desired_nd = depthNormalTex.SampleLevel( samPoint, desired_uv, 0 );
    if (IN.depth > desired_nd.w + g_depthOffset)
    {
        discard;
    }

    importance *= DistanceToEyeFunction( IN.depth );

    float2 shadow_uv = (IN.shadow_pos.xy / IN.shadow_pos.w) * float2(0.5, -0.5) + float2(0.5, 0.5);
    importance *= ShadowEdgeFunction( shadow_uv );

    return float4(importance, 0, 0, 0);
}

*/
// hybrid

