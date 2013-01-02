#include "terrainCommon.hlsl"


[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut main(InputPatch<VertexOut, 4> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HullOut hout;
    
    hout.PosW     = p[i].PosW;
    hout.Tex      = p[i].Tex;
    
    return hout;
}


PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess pt;
    
    // Compute midpoint on edges, and patch center
    float3 e0 = 0.5f  * (patch[0].PosW + patch[2].PosW);
    float3 e1 = 0.5f  * (patch[0].PosW + patch[1].PosW);
    float3 e2 = 0.5f  * (patch[1].PosW + patch[3].PosW);
    float3 e3 = 0.5f  * (patch[2].PosW + patch[3].PosW);
    float3  c = 0.25f * (patch[0].PosW + patch[1].PosW + patch[2].PosW + patch[3].PosW);
        
    pt.EdgeTess[0] = CalcTessFactor(e0);
    pt.EdgeTess[1] = CalcTessFactor(e1);
    pt.EdgeTess[2] = CalcTessFactor(e2);
    pt.EdgeTess[3] = CalcTessFactor(e3);
        
    pt.InsideTess[0] = CalcTessFactor(c);
    pt.InsideTess[1] = pt.InsideTess[0];
    
    return pt;
}