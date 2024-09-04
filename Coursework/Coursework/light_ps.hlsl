//Light pixel shader
//Performs light and color calculations, and applie shadows
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture[3] : register(t1);

SamplerState SampleType : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    struct DirectionalLight
    {
        float4 ambient;
        float4 diffuse;
        float4 specular;
        float3 direction;
        float padding;
    }
		DirectionalLights[2];
    struct PointLight
    {
        float4 ambient;
        float4 diffuse;
        float4 specular;

        float3 position;
        float range;

        float3 attenuation;
        float padding;
    }
        PointLights[1];
    struct Spotlight
    {
        float4 ambient;
        float4 diffuse;
        float4 specular;

        float3 position;
        float range;

        float3 direction;
        float spot;

        float3 attenuation;
        float padding;
    }
        SpotLights[1];
    
    float3 cameraPosition;
    float shininess;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[4] : TEXCOORD2;
};

float4 calculateDirectionalLighting(DirectionalLight L, float3 normal, float3 cameraPos)
{
    float4 ambient = (0.f, 0.f, 0.f, 0.f);
    float4 diffuse = (0.f, 0.f, 0.f, 0.f);
    float4 spec = (0.f, 0.f, 0.f, 0.f);
    
    float3 lightVec = -L.direction;
    
    ambient = L.ambient;
    
    float diffuseFactor = dot(lightVec, normal);
    
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, cameraPos), 0.0f), 0.5f);
        
        diffuse = diffuseFactor * L.diffuse;
        spec = specFactor * L.specular;

    }
    
    return ambient + diffuse + spec;
}

float4 ComputePointLight(PointLight L, float3 pos, float3 normal, float3 toCamera)
{
    //Introduction to 3D Game Programming with Directx11 by Frank Luna
    float4 ambient = float4(0.f, 0.f, 0.f, 0.f);
    float4 diffuse = float4(0.f, 0.f, 0.f, 0.f);
    float4 spec = float4(0.f, 0.f, 0.f, 0.f);
    
    float3 lightVec = L.position - pos;
    
    float d = length(lightVec);
    
    if (d > L.range)
    {
        return (0.f, 0.f, 0.f, 0.f);
    }
    
    lightVec /= d;
    
    ambient = L.ambient;
    
    float diffuseFactor = dot(lightVec, normal);
    
    
     [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toCamera), 0.0f), 0.5f);
        
        diffuse = diffuseFactor * L.diffuse;
        spec = specFactor * L.specular;
    }
    
    float att = 1.0f / dot(L.attenuation, float3(1.0f, d, d * d));
    
    diffuse *= att;
    spec *= att;
    
    return ambient + diffuse + spec;
    
}

float4 ComputeSpotLight(Spotlight L, float3 pos, float3 normal, float3 toCamera)
{
    //Introduction to 3D Game Programming with Directx11 by Frank Luna
    float4 ambient = (0.f, 0.f, 0.f, 0.f);
    float4 diffuse = (0.f, 0.f, 0.f, 0.f);
    float4 spec = (0.f, 0.f, 0.f, 0.f);
    
    float3 lightVec = L.position - pos;
    
    float d = length(lightVec);
    
    if (d > L.range)
    {
        return (0.f, 0.f, 0.f, 0.f);
    }
    
    lightVec /= d;
    
    ambient = L.ambient;
    
    float diffuseFactor = dot(lightVec, normal);
    
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toCamera), 0.0f), 0.5f);

        diffuse = diffuseFactor * L.diffuse;
        spec = specFactor * L.specular;
    }
    
    float spot = pow(max(dot(-lightVec, L.direction), 0.0f), L.spot);
    
    float att = spot / dot(L.attenuation, float3(1.0f, d, d * d));
    
    ambient *= spot;
    diffuse *= att;
    spec *= att;
    
    return ambient + diffuse + spec;

}

float4 calculateSpotLight(Spotlight L, float3 normal, float3 worldPosition)
{
    //Introduction to 3D Game Programming with Directx11 by Frank Luna
    float intensity = saturate(dot(normal, -L.direction));
    float4 colour = L.ambient;

    float3 lightVector = L.position - worldPosition;
    float distance = length(lightVector);

    lightVector = normalize(lightVector);

    intensity = 1 - smoothstep(0.f, 30, distance);
    intensity = smoothstep(20, saturate(20 + 0.1), saturate(dot(-L.direction, lightVector)));

    colour += L.diffuse * intensity;

    return colour;
}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
    float3 vertNormal = input.normal;
    float4 textureColour;
    float4 lightColour = float4(0, 0, 0, 1);
    float shadowMapBias = 0.005f;


    float3 toCamera = input.worldPosition - cameraPosition;
    toCamera = normalize(toCamera);

    for (int i = 0; i < 2; i++)
    {
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
        if (hasDepthData(pTexCoord))
        {
            if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
                lightColour += calculateDirectionalLighting(DirectionalLights[i], vertNormal, cameraPosition);

            }
        }
    }
    
    lightColour += ComputePointLight(PointLights[0], input.worldPosition, vertNormal, toCamera);

    //for (int i = 2; i < 8; i++)
    //{
    //    float2 pTexCoord = getProjectiveCoords(input.lightViewPos[2]);

    //    if (hasDepthData(pTexCoord))
    //    {
    //        if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[2], shadowMapBias))
    //        {

    //        }
    //    }
    //}

    float2 pTexCoord = getProjectiveCoords(input.lightViewPos[3]);
    if (hasDepthData(pTexCoord))
    {
        if (!isInShadow(depthMapTexture[2], pTexCoord, input.lightViewPos[3], shadowMapBias))
        {
            lightColour += ComputeSpotLight(SpotLights[0], input.worldPosition, vertNormal, toCamera);
        }
    }
    
    lightColour = saturate(lightColour);

    textureColour = shaderTexture.Sample(SampleType, input.tex);
    textureColour = saturate(textureColour);
    
    return lightColour * textureColour;
}