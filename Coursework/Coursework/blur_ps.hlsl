//Blur pixel shader
//Blurs pixels using gaussian blur weights

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenHeight;
    float screenWidth;
    float2 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 colour;

    float weight[5];
    
    weight[0] = 0.227027;
    weight[1] = 0.1945946;
    weight[2] = 0.1216216;
    weight[3] = 0.054054;
    weight[4] = 0.016216;

    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float yTexelSize = 1.0f / screenHeight;
    float xTexelSize = 1.0f / screenWidth;
 
    for (int i = 0; i < 5; i++)
    {
        colour += shaderTexture.Sample(SampleType, input.tex + float2(xTexelSize * i, yTexelSize * i)) * weight[i];

        colour += shaderTexture.Sample(SampleType, input.tex - float2(xTexelSize * i, yTexelSize * i)) * weight[i];
    }
    
    // Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}