cbuffer matrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

cbuffer waveBuffer : register(b1)
{
    float amplitude;
    float frequency;
    float time;
    float steepness;
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
    float4 depthPosition : TEXCOORD0;
};

OutputType main(InputType input)
{
    OutputType output;
    
    		//waves
    float wave = (sin((input.position.x * frequency) + time) + cos((input.position.z * frequency) + time)) * amplitude;
	
    input.position.xyz += float4(input.normal.xyz, 0) * float4(wave, wave, wave, 0);
    
    	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.depthPosition = output.position;
    
    return output;
}