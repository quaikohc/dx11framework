#include "terrainCommon.hlsl"


SamplerState samHeightmap;

Texture2D gHeightMap : register(t0);

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    vout.PosW = vin.PosL;
    
    vout.PosW.y = gHeightMap.SampleLevel( samHeightmap, vin.Tex, 0 ).r;
    
    vout.Tex      = vin.Tex;
    vout.BoundsY  = vin.BoundsY;
    
    return vout;
}
 