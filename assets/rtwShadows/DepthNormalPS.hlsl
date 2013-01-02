
struct OutputVS
{
    float4  pos         : SV_POSITION;
    float3  normal      : NORMAL;
    float   depth       : DEPTH;
};


float4 main(OutputVS IN) : SV_TARGET0
{
    return float4(normalize(IN.normal), IN.depth);
}
