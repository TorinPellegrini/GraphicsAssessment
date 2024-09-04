//Manipulation vertex shader
//Basic vertex shader, returns positions of vertices in the world, rendered mostly obsolete by domain shader
struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

OutputType main(InputType input)
{
    OutputType output;

    output.position = input.position;
    output.tex = input.tex;

    return output;
}