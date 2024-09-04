//Bloom pixel shader
//Extracts pixels over a certain brightness threshold, and multiplies those pixels by a strength
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer BloomBuffer : register(b0)
{
    float strength;
    float threshold;
    float2 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    //https://learnopengl.com/Advanced-Lighting/Bloom
    //https://pingpoli.medium.com/the-bloom-post-processing-effect-9352fa800caf
    
    float4 fragmentColor = shaderTexture.Sample(SampleType, input.tex);
    
    float brightness = dot(fragmentColor.rgb, float3(0.2126, 0.7152, 0.0722));
    float4 brightColor;
    
    if (brightness > threshold)
    {
        brightColor = float4(strength * fragmentColor.rgb, 1.f);
    }
    else
    {
        brightColor = float4(0.0f, 0.0f, 0.0f, 1.f);
    }

    
    return brightColor;

}