// Texture shader
// Used to combine two textures together

// Texture and sampler registers
Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
    float4 firstTexture = texture0.Sample(Sampler0, input.tex);
    float4 secondTexture = texture1.Sample(Sampler0, input.tex);
    return firstTexture+secondTexture;
}