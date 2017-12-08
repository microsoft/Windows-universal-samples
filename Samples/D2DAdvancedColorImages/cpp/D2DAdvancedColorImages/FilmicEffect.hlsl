
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

// Custom effects using pixel shaders should use HLSL helper functions defined in
// d2d1effecthelpers.hlsli to make use of effect shader linking.
#define D2D_INPUT_COUNT 1           // The pixel shader takes 1 input texture.
#define D2D_INPUT0_SIMPLE

// Note that the custom build step must provide the correct path to find d2d1effecthelpers.hlsli when calling fxc.exe.
#include "d2d1effecthelpers.hlsli"

cbuffer constants : register(b0)
{
    float dpi : packoffset(c1.z);
};

static const float3x3 ACESInputMat =
{
    { 0.59719, 0.35458, 0.04823 },
    { 0.07600, 0.90834, 0.01566 },
    { 0.02840, 0.13383, 0.83777 }
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367 },
    { -0.10208,  1.10813, -0.00605 },
    { -0.00327, -0.07276,  1.07602 }
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

D2D_PS_ENTRY(main)
{
    float4 color = D2DGetInput(0);
    float3 filmic = mul(ACESInputMat, color.rgb);

    // Apply RRT and ODT
    filmic = RRTAndODTFit(filmic);

    filmic = mul(ACESOutputMat, filmic);

    // Clamp to [0, 1]
    filmic = saturate(filmic);

    return float4(filmic, color.a);
}

