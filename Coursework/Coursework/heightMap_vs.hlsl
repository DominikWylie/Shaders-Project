Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);


cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

cbuffer HeightMapBuffer : register(b1)
{
    float heightMultiplier;
    float vectorLength;
    float yEdit;
    float padding;
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


// Helper function to calculate the normal from the heightmap
float3 CalculateNormal(float2 texCoord, float4 position)
{
    float heightCenter = texture0.SampleLevel(sampler0, texCoord, 0).r * heightMultiplier;
    float3 positionCentre = float3(position.x, heightCenter, position.z);
    
    float vecLength = vectorLength;

    float heightForward = texture0.SampleLevel(sampler0, texCoord + float2(0.0, vecLength), 0).r * heightMultiplier;
    float3 vecforward = float3(position.x, heightForward, position.z + vecLength);
    
    float heightLeft = texture0.SampleLevel(sampler0, texCoord + float2(-vecLength, 0.0f), 0).r * heightMultiplier;
    float3 vectorLeft = float3(position.x - vecLength, heightLeft, position.z);
    
    float heightDown = texture0.SampleLevel(sampler0, texCoord + float2(0.0f, -vecLength), 0).r * heightMultiplier;
    float3 vectorDown = float3(position.x, heightDown, position.z - vecLength);
    
    float heightRight = texture0.SampleLevel(sampler0, texCoord + float2(vecLength, 0.0), 0).r * heightMultiplier;
    float3 vecRight = float3(position.x + vecLength, heightRight, position.z); 
    
    vecforward -= positionCentre;
    vecRight -= positionCentre;
    vectorLeft -= positionCentre;
    vectorDown -= positionCentre;
    
    vecforward = normalize(vecforward);
    vecRight = normalize(vecRight);
    vectorLeft = normalize(vectorLeft);
    vectorDown = normalize(vectorDown);
    
    float3 crossP1 = cross(vecforward, vecRight);
    float3 crossP2 = cross(vectorDown, vectorLeft);
    
    float3 crossFinal = crossP1 + crossP2;
    
    //i thought it was being mesed up by the scalling 
    //but its the same if i remove the sealling and higher resolution
    crossFinal.y = crossFinal.y * 500;

    float3 normal = normalize(crossFinal);
    
    return normal;
}

OutputType main(InputType input, uint vertexID : SV_VertexID)
{
    OutputType output;

    input.position.y += texture0.SampleLevel(sampler0, input.tex, 0).r * heightMultiplier;
    

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector in the pixel shader.
    output.normal = CalculateNormal(input.tex, output.position);
    //output.normal = mul(output.normal, (float3x3) worldMatrix);
    //output.normal = normalize(output.normal);
    
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    
    output.viewVector = cameraPosition.xyz - output.worldPosition;
    output.viewVector = normalize(output.viewVector);
    
    output.lightViewPos = mul(input.position, worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

    return output;
}