// Manipulation pixel shader
//Calculate lighting and normals for a heightmap

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D depthMapTexture[3] : register(t2);


SamplerState sampler0 : register(s0);
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
    //Introduction to 3D Game Programming with Directx11 by Frank Luna
    float4 ambient = (0.f, 0.f, 0.f, 0.f);
    float4 diffuse = (0.f, 0.f, 0.f, 0.f);
    float4 spec = (0.f, 0.f, 0.f, 0.f);
    
    float3 lightVec = -L.direction;
    
    ambient = L.ambient;
    
    float diffuseFactor = dot(lightVec, normal);
    
    [flatten]
    if(diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, cameraPos), 0.0f), 0.5f);
        
        diffuse = diffuseFactor * L.diffuse;
        spec = specFactor * L.specular;

    }
    
    return ambient+diffuse+spec;
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


float3 CalcNormal(float2 uv)
{
    //Calculates new normals based off of heightmap texture
    float gTexelCellSpaceU = 0.008f;
    float gTexelCellSpaceV = 0.008f;
    
    float gWorldCellSpace = 0.008f;
    
    float2 leftTex = uv + float2(-gTexelCellSpaceU, 0.0f);
    float2 rightTex = uv + float2(gTexelCellSpaceU, 0.0f);
    float2 bottomTex = uv + float2(0.0f, gTexelCellSpaceV);
    float2 topTex = uv + float2(0.0f, -gTexelCellSpaceV);
    
    float leftY = texture0.SampleLevel(sampler0, leftTex, 0).r;
    float rightY = texture0.SampleLevel(sampler0, rightTex, 0).r;
    float bottomY = texture0.SampleLevel(sampler0, bottomTex, 0).r;
    float topY = texture0.SampleLevel(sampler0, topTex, 0).r;
    
    float3 tangent = normalize(float3(2.0f * gWorldCellSpace, rightY - leftY, 0.0f));
    float3 bitan = normalize(float3(0.0f, bottomY - topY, -2.0f * gWorldCellSpace));
    float3 normalW = cross(tangent, bitan);

    return normalW;
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
	//float4 lightColour;
    float3 newNormal = CalcNormal(input.tex);
    float3 vertNormal = input.normal;
    float4 textureColour;
    float4 lightColour = float4(0, 0, 0, 1);
    float shadowMapBias = 0.005f;

    float3 toCamera = input.worldPosition - cameraPosition;
    toCamera = normalize(toCamera);

    newNormal = lerp(vertNormal, newNormal, 0.5);

    for (int i = 0; i < 2; i++)
    {
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
        if (hasDepthData(pTexCoord))
        {
            if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
                lightColour += calculateDirectionalLighting(DirectionalLights[i], newNormal, cameraPosition);

            }
        }
    }
    
    lightColour += ComputePointLight(PointLights[0], input.worldPosition, newNormal, toCamera);

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
            lightColour += ComputeSpotLight(SpotLights[0], input.worldPosition, newNormal, toCamera);
        }
    }


	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    lightColour = saturate(lightColour);
    textureColour = texture1.Sample(sampler0, input.tex);
    textureColour = saturate(textureColour);
    
    
    
    return lightColour * textureColour;

}