// Tessellation Hull Shader
// Prepares control points for tessellation
// Calculates offset of positions based off of heightmap texture
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer CameraBuffer : register(b0)
{
    float3 cameraPos;
    float padding;
};

cbuffer MatrixBuffer : register(b1)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[4];
    matrix lightProjectionMatrix[4];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

//Computes midpoint of three input points
float3 ComputePatchMid(float3 a, float3 b, float3 c)
{
    return (a + b + c) / 3.f;
}

//Computes scaled value based on distance between two points
float ComputeScaled(float3 f, float3 t)
{
    float d = distance(f, t);
    float maxD = 20;
    float minD = 1;
    return (d - minD) / (maxD - minD);
}

//Implements Level of Detail (LOD) computation based on camera position and midpoint
//Lerps between a minimum and maximum distance
float DoLOD(float3 midP)
{
    float d = ComputeScaled(cameraPos, midP);
    return lerp(1, 6, saturate(1-d));
}

//Computes control points for tesselation based on heightmap texture
//Transforms vertex positions to world space based on sampled texture values
ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> ip, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;
    float3 midPoints[3];
    float4 textureColour1 = texture0.SampleLevel(sampler0, ip[0].tex, 0);
    float4 textureColour2 = texture0.SampleLevel(sampler0, ip[1].tex, 0);
    float4 textureColour3 = texture0.SampleLevel(sampler0, ip[2].tex, 0);

    float3 position1 = mul(float4(ip[0].position.xyz, 1.0f), worldMatrix);
    float3 position2 = mul(float4(ip[1].position.xyz, 1.0f), worldMatrix);
    float3 position3 = mul(float4(ip[2].position.xyz, 1.0f), worldMatrix);

    position1.y += (textureColour1.x * 30);
    position2.y += (textureColour2.x * 30);
    position3.y += (textureColour3.x * 30);

    // Find formula for 6 control patch points
    midPoints[0] = ComputePatchMid(position1, position2, position3);
    midPoints[1] = ComputePatchMid(position2, position3, position1);
    midPoints[2] = ComputePatchMid(position3, position1, position2);

    float dist0 = DoLOD(midPoints[0]);
    output.inside = dist0;

    output.edges[0] = min(dist0, DoLOD(midPoints[0]));
    output.edges[1] = min(dist0, DoLOD(midPoints[1]));
    output.edges[2] = min(dist0, DoLOD(midPoints[2]));

    return output;
}


[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    float4 textureColour = texture0.SampleLevel(sampler0, patch[pointId].tex, 0);

    float positionOffset = patch[pointId].position.y + (textureColour.r * 30);
    float4 finalPos = patch[pointId].position;
    finalPos.y += positionOffset;

    output.position = finalPos;
    output.tex = patch[pointId].tex;

    return output;
}
