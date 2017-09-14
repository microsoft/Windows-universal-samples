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

// Implements an extremely rudimentary HDR-to-SDR tone mapping effect. This is a
// simplified version of the Reinhard tone mapping algorithm, applied directly
// to the RGB channels. It successfully maps any content into the SDR range
// (i.e. 0.0f - 1.0f), but highlights will tend to be compressed, and color
// shifting can occur.
D2D_PS_ENTRY(main)
{
    // Basic formula: for each color value x of each pixel, return (x / (x+1)).
    float4 color = D2DGetInput(0);
    return (color / (color + 1.0f));
}