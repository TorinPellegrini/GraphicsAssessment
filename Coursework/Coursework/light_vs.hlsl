//Light vertex shader
//Returns the positions of veritces and calculates the light view positions for shadows

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[4];
    matrix lightProjectionMatrix[4];
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[4] : TEXCOORD2;
};


OutputType main(InputType input)
{
    OutputType output;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;
    
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    output.worldPosition = mul(input.position, worldMatrix).xyz;

    output.lightViewPos[0] = mul(float4(input.position.xyz, 1.f), worldMatrix);
    output.lightViewPos[0] = mul(output.lightViewPos[0], lightViewMatrix[0]);
    output.lightViewPos[0] = mul(output.lightViewPos[0], lightProjectionMatrix[0]);

    output.lightViewPos[1] = mul(float4(input.position.xyz, 1.f), worldMatrix);
    output.lightViewPos[1] = mul(output.lightViewPos[1], lightViewMatrix[1]);
    output.lightViewPos[1] = mul(output.lightViewPos[1], lightProjectionMatrix[1]);

    output.lightViewPos[2] = mul(float4(input.position.xyz, 1.f), worldMatrix);
    output.lightViewPos[2] = mul(output.lightViewPos[2], lightViewMatrix[2]);
    output.lightViewPos[2] = mul(output.lightViewPos[2], lightProjectionMatrix[2]);

    output.lightViewPos[3] = mul(float4(input.position.xyz, 1.f), worldMatrix);
    output.lightViewPos[3] = mul(output.lightViewPos[3], lightViewMatrix[3]);
    output.lightViewPos[3] = mul(output.lightViewPos[3], lightProjectionMatrix[3]);

    return output;
}