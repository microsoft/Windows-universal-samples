//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

struct VertexOutput
{
    float4 pos : SV_POSITION;
    float2 targetTexCoord : TEXCOORD0;
    float targetDepthNormalized : TEXCOORD1;
};

Texture2D targetTexture : register(t0);
Texture2D<float2> targetDepthTexture : register(t1);
SamplerState defaultSampler : register(s0);

float4 main(VertexOutput input) : SV_TARGET
{
    // Sample the depth buffer from the 1st pass of the coordinate mapper
    const float depthNormalized = targetDepthTexture.Sample(defaultSampler, input.targetTexCoord).x;

    // Add a small bias to avoid z-fighting 
    static const float depthBias = 0.005;

    ////////////////////////////////////////////////////////////////////////////////
    // Step 6.) Discard pixels that were occluded from the target camera's perspective
    if (input.targetDepthNormalized - depthBias > depthNormalized)
    {
        // Alternatively you can visualize which pixels are getting discarded by rendering a solid color
        // replace "discard" in the line below with "return float4(1.0f, 0.0f, 1.0f, 1.0f)" to visualize
        // occluded pixels as magenta instead of discarding them.

        discard;
    }

    return targetTexture.Sample(defaultSampler, input.targetTexCoord);
}