

cbuffer cbPerObject
{
    float4x4    view;
    float4x4    viewProj;
    float4x4    invView;
    float4x4    lightView;
    float4x4    invLightView;
    float4x4    invLightViewProj;
}; 


struct VertexOut
{
    float4 PosH     : SV_POSITION;
    float2 Tex      : TEXCOORD0;
    float  Depth    : TEXCOORD1;
    float  DepthVS  : TEXCOORD2;
};

//    transforming the texel from thelight space (Ml) to the desired view space (Md)
//      D(u,v) = 1 - (Md * Ml(-1)) * depth (u,v)' )z

float4 main(VertexOut input) : SV_TARGET
{
    float4 depth = float4( input.Depth, input.Depth, input.Depth, 1);
   // float4 depth = float4( input.Depth, 1.0f, 1.0f, 1.0f);

    float4 outVal;
     
    outVal = mul(depth, mul(view, invLightViewProj));

   // outVal = mul(input.PosH, mul(invLightView, view));

  //  outVal.x = outVal.y = outVal.z = outVal.w = input.DepthVS;

    outVal.x = 1 - outVal.x;
    return outVal;

   // return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
