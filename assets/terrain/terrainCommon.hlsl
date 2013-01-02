





struct DirectionalLight
{
    float4      Ambient;
    float4      Diffuse;
    float4      Specular;
    float3      Direction;
    float       pad;
};

struct Material
{
    float4      Ambient;
    float4      Diffuse;
    float4      Specular; // w = SpecPower
    float4      Reflect;
};


cbuffer cbPerFrame
{
    DirectionalLight gDirLight;
    float3 gEyePosW;
    float pad;
    float4 gFogColor;

    float gTexelCellSpaceU;
    float gTexelCellSpaceV;
    float gWorldCellSpace;
    float pad2;

    float4x4 gViewProj;
    
    float4 gWorldFrustumPlanes[6];

    Material gMaterial;
};





struct VertexIn
{
    float3 PosL     : POSITION;
    float2 Tex      : TEXCOORD0;
    float2 BoundsY  : TEXCOORD1;
};

struct VertexOut
{
    float3 PosW     : POSITION;
    float2 Tex      : TEXCOORD0;
    float2 BoundsY  : TEXCOORD1;
};


struct PatchTess
{
    float EdgeTess[4]   : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};


struct HullOut
{
    float3 PosW     : POSITION;
    float2 Tex      : TEXCOORD0;
};

struct DomainOut
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float2 Tex      : TEXCOORD0;
    float2 TiledTex : TEXCOORD1;
};


float CalcTessFactor(float3 p)
{
    float d = distance(p, gEyePosW);
    
    //float d = max( abs(p.x-gEyePosW.x), abs(p.z-gEyePosW.z) );
    
    float s = saturate( (d - 20) / (500 - 20) );
    
    return pow(2, (lerp(6, 0, s)) );
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