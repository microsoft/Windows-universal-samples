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

#pragma once

#define DEPTH_ARRAY_SIZE (UINT16_MAX + 1)

// Weights each pixel by using the depth value to look up a weight that is precomputed.
__forceinline float WeightDepth(unsigned int const& depth, float const* depthWeightArray)
{
    return depthWeightArray[depth];
}

// Precomputes the depth weight.
static void InitDepthCurveArray(
    float const& nearPlane,
    float const& farPlane,
    unsigned int const& invalidDepthValue,
    float* outDepthWeightArray)
{
    // Set up constants.
    constexpr float maxDepthAsFloat = static_cast<float>(UINT16_MAX);
    constexpr float oneOverMaxDepthAsFloat = 1.f / maxDepthAsFloat;

    // Create a transform from depth space in integer form, to meters in floating-point.
    const float denom = -farPlane * nearPlane;
    const float fromOneOverZNearFarToOneOverZ = (farPlane - nearPlane) / denom;
    const float fromOneOverZNearFarToOneOverZCoefficient = farPlane / denom;
    
    for (unsigned int i = 0; i < DEPTH_ARRAY_SIZE; ++i)
    {
        // Convert to meters.
        const float oneOverZDepthSpace = static_cast<float>(i) * oneOverMaxDepthAsFloat;
        const float zInMeters = 1.0f / (oneOverZDepthSpace * fromOneOverZNearFarToOneOverZ - fromOneOverZNearFarToOneOverZCoefficient);
        
        // Here, we provide a simple weight function by using the square of the depth value, in meters.
        // This biases the result towards far-away objects. This is desirable because far-away objects
        // use less pixels, and we need a way to give them the same consideration in the plane fit as a 
        // close object. If we do not do this, the plane will not be able to fit both close and far-away 
        // objects at the same time, and in scenes where both are visible you will get a lot of color 
        // separation on the far-away objects.
        outDepthWeightArray[i] = pow(zInMeters, 2);

        // You can also apply more complex equations to create the depth weights. This can be useful if 
        // you know in advance how the depth buffer will be filled in, based on the content that your app 
        // provides.
        //
        // For example, one of our test scenes did well with the following power curve. This provides a
        // heavy weight for far away pixels, just like the z^2 curve, but it uses a sharper transition to
        // the heavy region and has a falloff to the far plane. This operates in depth space, which has
        // a range of [0, 1]:
        //   constexpr float minimumNonZeroValue = 0.00000001f;
        //   outDepthWeightArray[i] = 2.f * oneOverZDepthSpace * (-pow(oneOverZDepthSpace, 32.f) + pow(oneOverZDepthSpace, 8.f)) + minimumNonZeroValue;

        // We disregard background pixels by giving them a weight of 0.
        if (i == invalidDepthValue)
        {
            outDepthWeightArray[i] = 0.f;
        }
    }
}

