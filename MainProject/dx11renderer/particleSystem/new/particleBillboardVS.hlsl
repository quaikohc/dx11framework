
struct VSParticleIn
{
    float3 Pos  : POSITION;
    float3 Vel  : VELOCITY;
    float Life  : LIFE;
    float Size  : SIZE;
};

struct GSParticleIn
{
    float3 Pos            : POSITION;
    float Life            : LIFE;       // [0..1]
    float Size            : SIZE;
};


GSParticleIn main(VSParticleIn input)
{
    GSParticleIn output;
    
    output.Pos  = input.Pos;
    output.Life = input.Life;
    output.Size = input.Size;
    
    return output;
}