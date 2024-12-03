// pixel shader used for post processing
// applies motion blur effect
// calculates current/previous viewpoints and samples depth to apply correct effect 

// Texture and sampler registers
Texture2D texture0 : register(t0);
Texture2D depth0 : register(t1);
SamplerState Sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix view_inverse;
    matrix view_previous;
    float blur_strength;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	// Get the depth buffer value at this pixel.
    float zOverW = depth0.Sample(Sampler0, input.tex);
	// H is the viewport position at this pixel in the range -1 to 1.
    float4 H = float4(input.tex.x * 2 - 1, (1 - input.tex.y) * 2 - 1, zOverW, 1);
	// Transform by the view-projection inverse.
    float4 D = mul(H, view_inverse);
	// Divide by w to get the world position.
    float4 worldPos = D / D.w;
	
	
    // Current viewport position    
    float4 currentPos = H;
    // Use the world position, and transform by the previous view-   
    // projection matrix.    
    float4 previousPos = mul(worldPos, view_previous);
    // Convert to nonhomogeneous points [-1,1] by dividing by w. 
    previousPos /= previousPos.w;
	
	
	
	// calculate velocity based on current and previous frames
    float2 velocity = (currentPos - previousPos).xy * blur_strength;
	
	// invert x velocity to blur in correct direction (counter to movement)
    velocity.x = -velocity.x;
	
	// get distance from camera to pixel using depth buffer data
	// blur more the closer the pixel is to camera
    float dist = 1.0f - zOverW;
    velocity *= dist;
	
	// Get the initial color at this pixel.
    float4 color = texture0.Sample(Sampler0, input.tex);
    input.tex += velocity;
	
    //if the colour is black return (stops the smearing on the background)
    if (color.r == 0 && color.g == 0 && color.b == 0)
    {
        return float4(0, 0, 0, 1);
    }
    
    int num_samples = 8;
    for (int i = 1; i < num_samples; ++i, input.tex += velocity)
    { // Sample the color buffer along the velocity vector.   
        float4 currentColor = texture0.Sample(Sampler0, input.tex);
        // Add the current color to our color sum.   
        color += currentColor;
    }
    // Average all of the samples to get the final blur color.    
    float4 finalColor = color / num_samples;
    return finalColor;
    //zOverW /= 10.0f;
    //return zOverW;

}

//float4 main(InputType input) : SV_TARGET
//{
//    float depthValue;
//	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
//    //depthValue = input.position.z / input.position.w;
//    //depthValue = input.depthPos.z / input.depthPos.w;
    
//    //depthValue = (input.position.z / input.depthPos.w) / 1000.0f;
    
//    //return float4(depthValue, depthValue, depthValue, 1.0f);
    
//    //float texture0.
//    //Sample(Sampler0, input.position);
    
//    //return float4(depthValue, depthValue, depthValue, 0.0f);

//    return depth0.Sample(Sampler0, input.tex);
//    //return float4(input.depthPos.z, input.depthPos.z, input.depthPos.z, 1.0f);

//}