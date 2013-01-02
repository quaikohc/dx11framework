static const float SMAP_SIZE = 1024.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};


float CalcShadowFactor(SamplerComparisonState samShadow,  Texture2D shadowMap, float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w;
    
    float depth = shadowPosH.z; // Depth in NDC space

    const float dx = SMAP_DX;   // Texel size.

    float percentLit = 0.0f;
    const float2 offsets[9] = 
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for(int i = 0; i < 9; ++i) // PCF
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit /= 9.0f;
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


