//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

// These values should match those in DftTransform. These values control
// the number of threads in each thread group. Higher values mean more work is
// done in parallel on the GPU - they don't have to be these particular values.
// However for Shader Model 5, z <= 64 and x*y*z <= 1024.
#define NUMTHREADS_X 32
#define NUMTHREADS_Y 32
#define NUMTHREADS_Z 1

#define PI 3.14159265358979

Texture2D<float4> InputTexture : register(t0);
SamplerState InputSampler : register(s0);

RWTexture2D<float4> OutputTexture;

// These are default constants passed by D2D. See PixelShader and VertexShader
// projects for how to pass custom values into a shader.
cbuffer systemConstants : register(b0)
{
    int4 resultRect; // Represents the input rectangle to the shader
    int2 outputOffset;
    float2 sceneToInput0X;
    float2 sceneToInput0Y;
};

// The image does not necessarily begin at (0,0) on InputTexture. The shader needs
// to use the coefficients provided by Direct2D to map the requested image data to
// where it resides on the texture.
float2 ConvertInput0SceneToTexelSpace(float2 inputScenePosition)
{
    float2 ret;
    ret.x = inputScenePosition.x * sceneToInput0X[0] + sceneToInput0X[1];
    ret.y = inputScenePosition.y * sceneToInput0Y[0] + sceneToInput0Y[1];
    return ret;
}

// numthreads(x, y, z)
// This specifies the number of threads in each dispatched threadgroup.
// For Shader Model 5, z <= 64 and x*y*z <= 1024
[numthreads(NUMTHREADS_X, NUMTHREADS_Y, NUMTHREADS_Z)]
void main(
    // dispatchThreadId - Uniquely identifies a given execution of the shader, most commonly used parameter.
    // Definition: (groupId.x * NUM_THREADS_X + groupThreadId.x, groupId.y * NUMTHREADS_Y + groupThreadId.y, groupId.z * NUMTHREADS_Z + groupThreadId.z)
    uint3 dispatchThreadId  : SV_DispatchThreadID,

    // groupThreadId - Identifies an individual thread within a thread group.
    // Range: (0 to NUMTHREADS_X - 1, 0 to NUMTHREADS_Y - 1, 0 to NUMTHREADS_Z - 1)
    uint3 groupThreadId     : SV_GroupThreadID,

    // groupId - Identifies which thread group the individual thread is being executed in.
    // Range defined in DFTHorizontalTransform::CalculateThreadgroups
    uint3 groupId           : SV_GroupID, 

    // One dimensional indentifier of a compute shader thread within a thread group.
    // Range: (0 to NUMTHREADS_X * NUMTHREADS_Y * NUMTHREADS_Z - 1)
    uint  groupIndex        : SV_GroupIndex
    )
{
    uint width = resultRect[2] - resultRect[0];
    uint height = resultRect[3] - resultRect[1];

    // It is likely that the compute shader will execute beyond the bounds of the input image, since the shader is executed in chunks sized by
    // the threadgroup size defined in DFTHorizontalTransform::CalculateThreadgroups. For this reason each shader should ensure the current
    // dispatchThreadId is within the bounds of the input image before proceeding.
    if (dispatchThreadId.x >= width || dispatchThreadId.y >= height)
    {
        return;
    }
  
    // This code represents the inner summation in the Discrete Fourier Transform (DFT) equation.
    float2 value = float2(0,0);
    for (uint n = 0; n < width; n++)
    {
        // The dispatchThreadId coordinates should be offset by the top left corner of the resultRect
        // to account for cases when the image does not begin at [0,0] (if the image has been translated
        // before being passed into the effect).
        float4 color = InputTexture.SampleLevel(
                InputSampler, 
                ConvertInput0SceneToTexelSpace(float2(n+0.5, dispatchThreadId.y + 0.5) + resultRect.xy), // Add 0.5 to hit the center of the pixel.
                0);

        float lum = color.r * 0.2125 +
                    color.g * 0.7154 +
                    color.b * 0.0721;

        float arg = -2.0 * PI * (float)dispatchThreadId.x * (float)n / (float)width;
        float sinArg, cosArg;
        sincos(arg,sinArg,cosArg);
        value += lum * float2(cosArg,sinArg);
    }

    // The x value here represents the real component of the calculation while the y value represents the
    // imaginary. The ordering doesn't matter, as long as it is consistent between the horizontal and
    // vertical shaders. Since alpha not used, it is set to '1' to enable internal opacity optimizations in Direct2D
    OutputTexture[dispatchThreadId.xy + outputOffset.xy + resultRect.xy] = float4(value.x, value.y, 0, 1);
}