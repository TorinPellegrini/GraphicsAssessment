//Depth manipulation vertex shader
//Does height map vertex calculations for the purposes of accurate depth values
Texture2D mtexture : register(t0);
SamplerState msampler : register(s0);

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

float GetHeightDisplacement(float2 uv)
{
    float offset = mtexture.SampleLevel(msampler, uv, 0).r;
    return offset * 30.f;
}

float3 CalcNormal(float2 uv)
{
    float tw = 256.0f;
    float val;
    mtexture.GetDimensions(0, tw, tw, val);
    float uvOff = 1.0f / 100.f;
    float heightN = GetHeightDisplacement(float2(uv.x, uv.y + uvOff));
    float heightS = GetHeightDisplacement(float2(uv.x, uv.y - uvOff));
    float heightE = GetHeightDisplacement(float2(uv.x + uvOff, uv.y));
    float heightW = GetHeightDisplacement(float2(uv.x - uvOff, uv.y));

    float WorldStep = 100 * uvOff;
    float3 tan = normalize(float3(2.0f * WorldStep, heightE - heightW, 0));
    float3 bitan = normalize(float3(0, heightN - heightS, 2.0f * WorldStep));
    return cross(bitan, tan);
}


OutputType main(InputType input)
{
    OutputType output;

    float3 newNormal = CalcNormal(input.tex); // Call the CalcNormal function

    //Calculate the position of vertices with the heightmap and using the new normals
    float3 displacement = newNormal;
    input.position.y += GetHeightDisplacement(input.tex);
    output.position = float4(input.position.xyz + displacement, 1.0f);



    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.worldPosition = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    
    // Calculate the position of the vertice as viewed by the light source.
    for (int i = 0; i < 4; i++) {
        output.lightViewPos[i] = mul(input.position, worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
    }

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only and normalize.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    

    return output;
}