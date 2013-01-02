
Texture2D diffuseTexture;
SamplerState diffuseTextureSampler;

Texture2D normalTexture;
SamplerState normalTextureSampler;

Texture2D heightTexture;
SamplerState heightTextureSampler;


struct Light
{
    float3 dir;
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

cbuffer cbPerFrame
{
    Light light;
    float3 cameraPos;
    float4 globalAmbient;
    float2 scaleBias;
};

cbuffer cbPerObject
{
    float4x4 worldIT;
    float4x4 worldViewProj;
    float4x4 world;
};

struct VS_INPUT
{
    float3 position  : POSITION;
    float2 texCoord  : TEXCOORD0;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_OUTPUT_DIR
{
    float4 position   : SV_POSITION;
    float2 texCoord   : TEXCOORD0;
    float3 halfVector : TEXCOORD1;
    float3 lightDir   : TEXCOORD2;
    float4 diffuse    : COLOR0;
    float4 specular   : COLOR1;
};

VS_OUTPUT_DIR VS(VS_INPUT IN)
{
    VS_OUTPUT_DIR OUT;
    
    float3 worldPos   = mul(float4(IN.position, 1.0f), world).xyz;
    float3 lightDir   = -light.dir;
    float3 viewDir    = cameraPos - worldPos;
    float3 halfVector = normalize(normalize(lightDir) + normalize(viewDir));
    
    float3 n = mul(IN.normal,        (float3x3)worldIT);
    float3 t = mul(IN.tangent.xyz,   (float3x3)worldIT);
    float3 b = mul(IN.bitangent.xyz, (float3x3)worldIT);

    float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
                                  t.y, b.y, n.y,
                                  t.z, b.z, n.z);
    
    OUT.position   = mul(float4(IN.position, 1.0f), worldViewProj);
    OUT.texCoord   = IN.texCoord;
    OUT.halfVector = mul(halfVector, tbnMatrix);
    OUT.lightDir   = mul(lightDir, tbnMatrix);
    OUT.diffuse    = /*material.diffuse **/   light.diffuse;
    OUT.specular   = /*material.specular **/ light.specular;
    
    return OUT;
}


float4 PS(VS_OUTPUT_DIR IN, uniform bool bParallax) : COLOR
{
    float2 texCoord;
    float3 h = normalize(IN.halfVector);

    float height = heightTexture.Sample(heightTextureSampler, IN.texCoord).r;
    
    height   = height * scaleBias.x + scaleBias.y;
    texCoord = IN.texCoord + (height * h.xy);

    float3 l = normalize(IN.lightDir);
    float3 n = normalize(normalTexture.Sample(normalTextureSampler, texCoord).rgb * 2.0f - 1.0f);
    
    float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, /*material.shininess*/1.0f);
    
    float4 color = (/*material.ambient **/ (globalAmbient + light.ambient)) + (IN.diffuse * nDotL) + (IN.specular * power);
    
    return color * diffuseTexture.Sample(diffuseTextureSampler, texCoord);
}
