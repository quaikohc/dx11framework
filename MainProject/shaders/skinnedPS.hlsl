
SamplerState basicSampler : register(s0);

Texture2D gDiffuseMap: register(t0);
Texture2D gNormalMap: register(t1);



struct DirectionalLight
{
    float4  Ambient;
    float4  Diffuse;
    float4  Specular;
    float3  Direction;
    float   pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};

cbuffer cbPerFrame: register(cb0)
{
    float4              startTestValuePerFrame;
    DirectionalLight    gDirLight;
    float3              gEyePosW;
    float               pad;
    float4              endTestValuePerFrame;
};

cbuffer cbPerObject: register(cb1)
{
    float4              startTestValuePerObject;
    float4x4            gWorld;
    float4x4            gWorldInvTranspose;
    float4x4            gWorldViewProj;
    float4x4            gWorldViewProjTex;
    float4x4            gTexTransform;
    float4x4            gBoneTransforms[96];
    Material            gMaterial;
    float4              endTestValuePerObject;
}; 

struct VertexOut
{
    float4 PosH       : SV_POSITION;
    float3 PosW       : POSITION;
    float3 NormalW    : NORMAL;
    float4 TangentW   : TANGENT;
    float2 Tex        : TEXCOORD0;
};

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float4 tangentW)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;
    
    float3 N = unitNormalW;
    float3 T = normalize(tangentW.xyz - dot(tangentW.xyz, N)*N);
    float3 B = tangentW.w*cross(N, T);
    
    float3x3 TBN = float3x3(T, B, N);
    
    float3 bumpedNormalW = mul(normalT, TBN);
    
    return bumpedNormalW;
}

void ComputeDirectionalLight(Material mat, DirectionalLight L, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightVec = -L.Direction;
    
    ambient = mat.Ambient * L.Ambient;	
    
    float diffuseFactor = dot(lightVec, normal);
    
    [flatten]
    if( diffuseFactor > 0.0f )
    {
        float3 v         = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec    = specFactor * mat.Specular * L.Specular;
    }
}

float4 main(VertexOut pin) : SV_Target
{
    pin.NormalW = normalize(pin.NormalW);
    
    float3 toEye = gEyePosW - pin.PosW;
    
    float distToEye = length(toEye);
    
    toEye /= distToEye;
    
    float4 texColor = float4(1, 1, 1, 1);

    texColor = gDiffuseMap.Sample( basicSampler, pin.Tex );

    float3 normalMapSample = gNormalMap.Sample(basicSampler, pin.Tex).rgb;
    float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, pin.NormalW, pin.TangentW);
     
    
    float4 litColor = texColor;

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 A, D, S;
    ComputeDirectionalLight(gMaterial, gDirLight, bumpedNormalW, toEye, A, D, S);
    
    //ambient += ambientAccess*A;    

    litColor = texColor*(ambient + diffuse) + spec;

   // litColor.a = gMaterial.Diffuse.a * texColor.a;
//test
    litColor    = texColor;//float4(1.0f, 1.0f, 0.0f, 0.0f);

    return litColor;
}