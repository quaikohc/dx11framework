
#define textureSize (1024)

#define warpUpLimit (0)
#define warpDownLimit (1024)//textureSize)

Texture2D           g_importanceTex : register(t0);
RWTexture1D<float>  g_warpingTex    : register(u0);

// values after calculating importance (horiz/vert min max from importance map)
groupshared float   gs_importance[textureSize];
// values after blurring calculated importance
groupshared float   gs_blurred[textureSize];


void BlurTexel(uint texelNo);


// TODO: test different values of warpDownLimit and warpUpLimit, it should be enough to analyse only some fixed neighbourhood of the texel
void ComputeWarpingValue(uint texelNo)
{
    float upWeight    = 0.0;   // weight of the 'upper' neighbours of the pixel
    float totalWeight = 0.0;   // total texture weight

    for (int i = warpUpLimit; i < texelNo; ++i)
    {
        upWeight    += gs_blurred[i];
        totalWeight += gs_blurred[i];
    }

    for (int i = texelNo; i < warpDownLimit; ++i)
    {
        totalWeight += gs_blurred[i];
    }

    g_warpingTex[texelNo] =  (upWeight / totalWeight) - ((float)(texelNo + 1) / (float)warpDownLimit);
}

 /*
1. importance for each cell is found by calculating the maximum importance within the respective row or column that each of the super-cells represent 
2. the values are blended using a Gaussian blur
3. the warping map is built by shifting texel positions based upon weight relative to the total weight, super-cell centers are moved based upon the ratio of
neighbour importance to the total importance. Cells with zero importance are culled.
*/
[numthreads(textureSize, 1, 1)]
void DispatchMaxBlurWarpX(uint3 groupThreadId : SV_GroupThreadID)
{
    uint texelNo = groupThreadId.x;

    uint2 uv = {texelNo, 0};
    float maxValue = g_importanceTex[uv].r;

    for (uint i = 1; i < textureSize; ++i)
    {
        uv.y = i;
        maxValue = max(maxValue, g_importanceTex[uv].r);
    }

    gs_importance[texelNo] = maxValue;

    GroupMemoryBarrierWithGroupSync();

    BlurTexel(texelNo);

    GroupMemoryBarrierWithGroupSync();

    ComputeWarpingValue(texelNo);
}

/*
//example
[numthreads(10, 1, 1)]
void CS_Main( uint3 dispatchThreadID : SV_DispatchThreadID )
{
     Particle p = srcParticleBuffer[dispatchThreadID.x];
}
*/
/*

#define kernelhalf 7
#define groupthreads 128
groupshared float4 temp[groupthreads];

[numthreads( groupthreads, 1, 1 )]
void CSMain( uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex )
{
    int2 coord = int2( GI - kernelhalf + (groupthreads - kernelhalf * 2) * Gid.x, Gid.y );
    coord = coord.xy * 8 + int2(4, 3);
    coord = clamp( coord, int2(0, 0), int2(g_inputsize.x-1, g_inputsize.y-1) );
    float4 vColor = Input.Load( int3(coord, 0) );
    }
*/
/*
[numthreads( groupthreads, 1, 1 )]
void CSHorizFilter( uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex )
{
    int2 coord = int2( GI - kernelhalf + (groupthreads - kernelhalf * 2) * Gid.x, Gid.y );
    coord = clamp( coord, int2(0, 0), int2(g_inputsize.x-1, g_inputsize.y-1) );
    temp[GI] = InputTex.Load( int3(coord, 0) );        

    GroupMemoryBarrierWithGroupSync();

    // Horizontal blur
    if ( GI >= kernelhalf && 
         GI < (groupthreads - kernelhalf) && 
         ( (Gid.x * (groupthreads - 2 * kernelhalf) + GI - kernelhalf) < g_outputsize.x) )
    {
        float4 vOut = 0;
        
        [unroll]
        for ( int i = -kernelhalf; i <= kernelhalf; ++i )
            vOut += temp[GI + i] * g_avSampleWeights[i + kernelhalf];

        Result[GI - kernelhalf + (groupthreads - kernelhalf * 2) * Gid.x + Gid.y * g_outputsize.x] = float4(vOut.rgb, 1.0f);
    }
}

*/
[numthreads(textureSize, 1, 1)]
void DispatchMaxBlurWarpY(uint3 groupThreadId : SV_GroupThreadID)
{
    uint texelNo = groupThreadId.x; // we are working on 1d buffers

    uint2 uv = {0, texelNo};
    float maxValue = g_importanceTex[uv].r;

    for (uint i = 1; i < textureSize; ++i)
    {
        uv.x = i;
        maxValue = max(maxValue, g_importanceTex[uv].r);
    }
    gs_importance[texelNo] = maxValue;

    GroupMemoryBarrierWithGroupSync();

    BlurTexel(texelNo);

    GroupMemoryBarrierWithGroupSync();

    ComputeWarpingValue(texelNo);
}

/* NOTES:

uint3 groupID : SV_GroupID 
 - Index of the group within the dispatch for each dimension / index for each dimension inside a ThreadGroupCount (The variable I am looking for)

uint3 groupThreadID : SV_GroupThreadID 
 - Index of the thread within the group for each dimension  / indices for each dimension inside a group of the current thread

uint groupIndex : SV_GroupIndex 
 - A sequential index within the group that starts from 0 top left back and goes on to bottom right front /  unique index of a thread inside a group

uint3 dispatchThreadID : SV_DispatchThreadID 
 - Global thread index within the whole dispatch / uint3 DispatchThreadID  = dot(GroupID, numthreads) + GroupThreadId;
*/





void BlurTexel(uint texelNo)
{
    // TODO: test blurring using more samples and other weights values
    const float weights[3] = {0.22254893, 0.21052226, 0.17820324}; // gaussian kernel, other variant: ( 0.2270270270, 0.3162162162, 0.0702702703 );

    float val = gs_importance[texelNo] * weights[0];

    if (texelNo > 1) // check for min and max value
    {
        val += gs_importance[texelNo - 1] * weights[1] + gs_importance[texelNo - 2] * weights[2];
    }
    else if (texelNo > 0)
    {
        val += gs_importance[texelNo - 1] * weights[1] + gs_importance[texelNo - 1] * weights[2];
    }
    else
    {
        val += gs_importance[texelNo] * weights[1] + gs_importance[texelNo] * weights[2];
    }

    if (texelNo < textureSize - 3)
    {
        val += gs_importance[texelNo + 1] * weights[1] + gs_importance[texelNo + 2] * weights[2];
    }
    else if (texelNo < textureSize - 2)
    {
        val += gs_importance[texelNo + 1] * weights[1] + gs_importance[texelNo + 1] * weights[2];
    }
    else
    {
        val += gs_importance[texelNo] * weights[1] + gs_importance[texelNo] * weights[2];
    }

    gs_blurred[texelNo] = val;
}