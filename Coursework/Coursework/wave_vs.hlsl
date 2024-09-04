//Wave vertex shader
//Calculates wave vertices using a gerstner wave formula.
//Also calculates light view positions for shadows

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

cbuffer TimeBuffer : register(b1)
{
    struct Wave
    {
        float2 direction;
        float steepness;
        float wavelength;
    } waves[6];
    float time;
    float3 padding;
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

float3 GerstnerWave(float2 direction, float steepness, float wavelength, float3 p, inout float3 tangent, inout float3 binormal)
{
    //https://catlikecoding.com/unity/tutorials/flow/waves/
    
    //k = 2pi/wavelength, k controls the wavelength
    float k = 2 * 3.14159 / wavelength;
    //c = phase speed, takes gravity into account (9.8)
    float c = sqrt(9.8 / k);
    //d = direction
    float2 d = normalize(direction);
    //f = Direction of the wave
    float f = k * (dot(d, p.xz) - c * time);
    //a = normalized steepness of the wave
    float a = steepness / k;

    tangent += float3(
         1-d.x * d.x * (steepness * sin(f)),
        +d.x * (steepness * cos(f)),
        -d.x * d.y * (steepness * sin(f))
    );

    binormal += float3(
        -d.x * d.y * (steepness * sin(f)),
        +d.y * (steepness * cos(f)),
       1-d.y * d.y * (steepness * sin(f))
    );

    return float3(d.x * (a * cos(f)), a * sin(f), d.y * (a * cos(f)));
}

OutputType main(InputType input)
{
    OutputType output;

    float3 tangent = (0.f, 0.f, 0.f);
    float3 binormal = (0.f, 0.f, 0.f);
    float3 position = input.position.xyz;

    float3 p = position;
    for (int i = 0; i < 6; i++)
    {
        p += GerstnerWave(waves[i].direction, waves[i].steepness, waves[i].wavelength, position, tangent, binormal);
    }
    input.position.xyz = p;
    float3 normal = normalize(cross(binormal/6, tangent/6));
    output.normal = normal;

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

    return output;
}