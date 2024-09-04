// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices for the heightmap
// Also calculates light view positions for shadow

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[4];
    matrix lightProjectionMatrix[4];
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPos;
    float padding;
}

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct InputType
{
    float4 position : SV_POSITION;
    float4 tex : TEXCOORD0;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[4] : TEXCOORD2;
};

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 3> patch)
{
    OutputType output;
    float3 vertexPosition;
    vertexPosition = (uvwCoord.x * patch[0].position) + (uvwCoord.y * patch[1].position) + (uvwCoord.z * patch[2].position);
    output.tex = (uvwCoord.x * patch[0].tex) + (uvwCoord.y * patch[1].tex) + (uvwCoord.z * patch[2].tex);

    // Offset vertices using the height map
    output.position = mul(float4(vertexPosition, 1.f), worldMatrix);
    output.worldPosition = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.lightViewPos[0] = mul(float4(vertexPosition, 1.f), worldMatrix);
    output.lightViewPos[0] = mul(output.lightViewPos[0], lightViewMatrix[0]);
    output.lightViewPos[0] = mul(output.lightViewPos[0], lightProjectionMatrix[0]);
           
    output.lightViewPos[1] = mul(float4(vertexPosition, 1.f), worldMatrix);
    output.lightViewPos[1] = mul(output.lightViewPos[1], lightViewMatrix[1]);
    output.lightViewPos[1] = mul(output.lightViewPos[1], lightProjectionMatrix[1]);
           
    output.lightViewPos[2] = mul(float4(vertexPosition, 1.f), worldMatrix);
    output.lightViewPos[2] = mul(output.lightViewPos[2], lightViewMatrix[2]);
    output.lightViewPos[2] = mul(output.lightViewPos[2], lightProjectionMatrix[2]);

    output.lightViewPos[3] = mul(float4(vertexPosition, 1.f), worldMatrix);
    output.lightViewPos[3] = mul(output.lightViewPos[3], lightViewMatrix[3]);
    output.lightViewPos[3] = mul(output.lightViewPos[3], lightProjectionMatrix[3]);

    return output;
}
