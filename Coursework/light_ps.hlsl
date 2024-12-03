Texture2D texture0 : register(t0);
Texture2D depthMapTexture : register(t1);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

struct Light
{
    float4 diffuseColour;
    float4 ambientColour;
    float4 specular;

    float3 lightDirection;
    float penumbraAngle;
    float3 lightposition;
    float coneAngle;
    
    float specularpower;
    float3 padding;
};

cbuffer LightBuffer : register(b0)
{
    Light light[3];
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
};

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
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

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, 
float4 specularColour, float specularPower)
{
    //float3 R = reflect(normalize(lightDirection), normalize(normal));
    
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    //float3 lightVector;
    //float3 coneDirection;
    //float cosTheta;
	
    float constantFactor = 0.5f;
    float linearFactor = 0.125f;
    float quadraticFactor = 0.0f;
    
    //textureColour = texture0.Sample(sampler0, input.tex);
    
    //lightVector = normalize(lightposition - input.worldPoition);
    
    //coneDirection = normalize(-lightDirection);
    
    //cosTheta = dot(coneDirection, lightVector);
    
    //if (cosTheta > cos(coneAngle))
    //{
    //    attenuation = 1 / (constantFactor + (linearFactor *
    //    length(input.worldPoition - lightposition)) + (quadraticFactor *
    //    pow(length(input.worldPoition - lightposition), 2)));
        
    //    lightColour = ambientColour + attenuation * 
    //    calculateLighting(lightVector, input.normal, diffuseColour);

    //}
    //else
    //{
    //    lightColour = float4(0, 0, 0, 1);
    //}
    
    //return lightColour + textureColour;
    
    textureColour = texture0.Sample(sampler0, input.tex);
    
    float4 finalLights = float4(0, 0, 0, 0);
    
    bool addShadow = false;
    
    for (int i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            //light view pos is taken off light 0
            float shadowMapBias = 0.005f;
            float2 pTexCoord = getProjectiveCoords(input.lightViewPos);

            if (hasDepthData(pTexCoord))
            {
                // Has depth map data
                if (isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
                {
                    // is NOT in shadow, therefore light
                    addShadow = true;
                }
            }
        }
        
        float distance = length(input.worldPosition - light[i].lightposition);
    
        float attenuation = 1 / (constantFactor + (linearFactor * distance) + (quadraticFactor * pow(distance, 2)));
       
        float3 lightVector = normalize(light[i].lightposition - input.worldPosition);
    
        float4 diffuseLightColour = light[i].ambientColour + calculateLighting(lightVector, input.normal, light[i].diffuseColour);
	
        float4 specularColour = calcSpecular(lightVector, input.normal, input.viewVector, light[i].specular, light[i].specularpower);
        
        //only doing it with light 0 for now
        if (!addShadow)
        {
            finalLights += (diffuseLightColour + specularColour) * textureColour;
        }
        else
        {
            finalLights += light[i].ambientColour * textureColour;
            
            //mess with the way it looks, i think its not coming from the light source properly but might 
            //just be a case that its desogned for a directiolal light? if so add a 4th or set one to directional only
        }
    }
    
    return finalLights;

}

//debug
//float4 main(InputType input) : sv_target
//{
//    float3 normalcolor = 0.5f * (input.normal + 1.0f); // convert normal to color space (-1 to 1) to (0 to 1)

//    return float4(normalcolor, 1.0f);
    
//    //float4 test = (0.2f, 0.2f, 0.8f, 0);
    
//    // float4(input.normal, 1);
//}

