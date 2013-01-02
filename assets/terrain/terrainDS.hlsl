#include "terrainCommon.hlsl"

SamplerState samHeightmap;

Texture2D gHeightMap;


[domain("quad")]
DomainOut main(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
    DomainOut dout;
    
    dout.PosW = lerp(lerp(quad[0].PosW, quad[1].PosW, uv.x), lerp(quad[2].PosW, quad[3].PosW, uv.x),uv.y); 
    
    dout.Tex = lerp(lerp(quad[0].Tex, quad[1].Tex, uv.x), lerp(quad[2].Tex, quad[3].Tex, uv.x),uv.y); 

  //  dout.TiledTex = dout.Tex; 
    dout.TiledTex = dout.Tex * 50.0f; 

    dout.PosW.y = gHeightMap.SampleLevel( samHeightmap, dout.Tex, 0 ).r;
    
    dout.PosH    = mul(float4(dout.PosW, 1.0f), gViewProj);
    
    return dout;
}
