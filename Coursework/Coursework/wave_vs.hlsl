cbuffer matrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
}

cbuffer waveBuffer : register(b1)
{
    float amplitude;
    float frequency;
    float time;
    float steepness;
}

cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition;
    float cameraPadding;
}

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
    float3 viewVector : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
};

OutputType main( InputType input )
{
    OutputType output;
    
    		//waves
    float wave = (sin((input.position.x * frequency) + time) + cos((input.position.z * frequency) + time)) * amplitude;
	
    input.position.xyz += float4(input.normal.xyz, 0) * float4(wave, wave, wave, 0);
    
    
    // Calculate the wave height at the current position
    float waveHeightX = (sin((input.position.x * frequency) + time) + cos((input.position.z * frequency) + time)) * amplitude;

// Calculate the wave height at nearby positions for x-axis derivative
    float waveHeightXPlus = (sin(((input.position.x + 0.1) * frequency) + time) + cos((input.position.z * frequency) + time)) * amplitude;
    float waveHeightXMinus = (sin(((input.position.x - 0.1) * frequency) + time) + cos((input.position.z * frequency) + time)) * amplitude;

// Calculate the wave height at nearby positions for z-axis derivative
    float waveHeightZPlus = (sin((input.position.x * frequency) + time) + cos(((input.position.z + 0.1) * frequency) + time)) * amplitude;
    float waveHeightZMinus = (sin((input.position.x * frequency) + time) + cos(((input.position.z - 0.1) * frequency) + time)) * amplitude;

// Calculate partial derivatives using central differences
    float dHeightX = waveHeightXPlus - waveHeightXMinus;
    float dHeightZ = waveHeightZPlus - waveHeightZMinus;

// Calculate the normal
    float3 normal = normalize(float3(-dHeightX, 1.0f, -dHeightZ));

// Assign the normal to the input structure
    input.normal = normal;
    
    	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    
    output.viewVector = cameraPosition.xyz - output.worldPosition;
    output.viewVector = normalize(output.viewVector);

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    output.lightViewPos = mul(input.position, worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);
        
    return output;
}