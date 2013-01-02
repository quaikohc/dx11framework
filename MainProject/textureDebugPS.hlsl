
Texture2D gTexture;

SamplerState samLinear;

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex  : TEXCOORD;
};
 

float4 main(VertexOut IN) : SV_Target
{
    float4 c = gTexture.Sample(samLinear, IN.Tex).r;

    return float4(c.rrr, 1);

   // return gTexture.Sample(samLinear, pin.Tex);

   // float4 c2 = gTexture.Sample(samLinear, IN.Tex);
   // c2.a = 1.0f;
  //  return c2;
}
