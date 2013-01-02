#include "terrainCommon.hlsl"

// TODO: use texture array
//Texture2DArray          gLayerMapArray;

Texture2D               gBlendMap  : register(t0);
Texture2D               gHeightMap : register(t1);

////////////////
Texture2D               gTextureGrass  : register(t2);
Texture2D               gTextureGround : register(t3);
Texture2D               gTextureSnow   : register(t4);
Texture2D               gTextureDesert : register(t5);
Texture2D               gTextureDirt   : register(t6);
////////////////

SamplerState            samLinear    : register(s0);
SamplerState            samHeightmap : register(s1);


float4 main(DomainOut pin) : SV_Target
{
    float2 leftTex   = pin.Tex + float2(-gTexelCellSpaceU, 0.0f);
    float2 rightTex  = pin.Tex + float2(gTexelCellSpaceU, 0.0f);
    float2 bottomTex = pin.Tex + float2(0.0f, gTexelCellSpaceV);
    float2 topTex    = pin.Tex + float2(0.0f, -gTexelCellSpaceV);
    

    float leftY   = gHeightMap.SampleLevel( samHeightmap, leftTex, 0 ).r;
    float rightY  = gHeightMap.SampleLevel( samHeightmap, rightTex, 0 ).r;
    float bottomY = gHeightMap.SampleLevel( samHeightmap, bottomTex, 0 ).r;
    float topY    = gHeightMap.SampleLevel( samHeightmap, topTex, 0 ).r;
    
    float3 tangent = normalize(float3(2.0f * gWorldCellSpace, rightY - leftY, 0.0f));
    float3 bitan   = normalize(float3(0.0f, bottomY - topY, -2.0f * gWorldCellSpace)); 
    float3 normalW = cross(tangent, bitan);
    
    float3 toEye = gEyePosW - pin.PosW;
    
    float distToEye = length(toEye);
    
    toEye /= distToEye;
    
    // float4 c0 = gLayerMapArray.Sample( samLinear, float3(pin.TiledTex, 0.0f) );
    // float4 c1 = gLayerMapArray.Sample( samLinear, float3(pin.TiledTex, 1.0f) );
    // float4 c2 = gLayerMapArray.Sample( samLinear, float3(pin.TiledTex, 2.0f) );
    // float4 c3 = gLayerMapArray.Sample( samLinear, float3(pin.TiledTex, 3.0f) );
    // float4 c4 = gLayerMapArray.Sample( samLinear, float3(pin.TiledTex, 4.0f) ); 

     float4 c0 = gTextureGrass.Sample(  samLinear, float2(pin.TiledTex) );
     float4 c1 = gTextureGround.Sample( samLinear, float2(pin.TiledTex) );
     float4 c2 = gTextureSnow.Sample(   samLinear, float2(pin.TiledTex) );
     float4 c3 = gTextureDesert.Sample( samLinear, float2(pin.TiledTex) );
     float4 c4 = gTextureDirt.Sample(   samLinear, float2(pin.TiledTex) ); 
    
    
     float4 t  = gBlendMap.Sample( samLinear, pin.Tex ); 
     
     float4 texColor = c0;
     texColor = lerp(texColor, c1, t.r);
     texColor = lerp(texColor, c2, t.g);
     texColor = lerp(texColor, c3, t.b);
     texColor = lerp(texColor, c4, t.a);
    
    
     ////////////// Lighting
    
     float4 litColor = texColor;
    
     float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
     float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
     float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
         
     float4 A, D, S;
     ComputeDirectionalLight(gMaterial, gDirLight, normalW, toEye, A, D, S);
             
     ambient += A;
     diffuse += D;
     spec    += S;
      
     litColor = texColor * (ambient + diffuse) + spec; // TEST

  //  float4 litColor = float4(1.0f, 1.0f, 0.0f, 0.0f);

    return litColor;
}