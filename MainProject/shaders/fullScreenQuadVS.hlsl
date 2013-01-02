
struct InputVS
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct OutputVS
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD;
};


OutputVS main(in InputVS input)
{
    OutputVS output;

    output.pos = input.pos;
    output.tex = input.tex;

    return output;
}