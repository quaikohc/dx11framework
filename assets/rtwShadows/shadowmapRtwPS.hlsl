
struct OutputVS
{
    float4  pos   : SV_POSITION;
    float2  depth : DEPTH;
};


float4 main( OutputVS IN ) : SV_TARGET
{
    float depth = IN.depth.x / IN.depth.y;

    return float4(depth, depth, depth, 1.0);
    //return float4(1.0f, 1.0f, 0.0f, 1.0);
}