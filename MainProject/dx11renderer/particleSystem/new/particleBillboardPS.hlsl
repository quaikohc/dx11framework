
Texture3D       g_txVolumeDiff;
SamplerState    g_samVolume;


struct GeometryShaderOut
{
    float4 PosH           : SV_Position;
    float3 particleColor  : TEXCOORD0;
    float3 Tex            : TEXCOORD1;

};

float4 main(GeometryShaderOut input ) : SV_TARGET
{
   float4 particleSample =  g_txVolumeDiff.Sample( g_samVolume, input.Tex);

    float4 Light = float4( 0.992, 1.0, 0.880, 0.0 ) + float4(0.525, 0.474, 0.474, 0.0);// directional and ambient light color, TODO: read this from uniform
    particleSample.rgb *= Light.xyz * input.particleColor;

    return particleSample;
}