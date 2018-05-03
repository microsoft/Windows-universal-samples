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

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

////////////////////////////////////////////////////////////////////////////////
// Future work: Support more color formats
// Currently unused, additional work needs to be done to support YUV
float3 ConvertYUVtoRGB(float3 yuv)
{
    // Matrix derived from https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
    static const float3x3 YUVtoRGBCoeffMatrix =
    {
        1.164383f,  1.164383f, 1.164383f,
        0.000000f, -0.391762f, 2.017232f,
        1.596027f, -0.812968f, 0.000000f
    };

    // These values are derived from (16 / 255), (128 / 255)
    yuv -= float3(0.062745f, 0.501960f, 0.501960f);
    yuv = mul(yuv, YUVtoRGBCoeffMatrix);

    return saturate(yuv);
}

////////////////////////////////////////////////////////////////////////////////
// Future work: Support more color formats
// Currently unused, additional work needs to be done to support YUY2
float3 ConvertYUY2toRGB(float4 textureSample, float2 texCoord, float textureWidth)
{
    // Derived from https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx
    // Y0->R8, U0->G8, Y1->B8, and V0->A8.

    const float y0 = textureSample.r;
    const float u = textureSample.g;
    const float y1 = textureSample.b;
    const float v = textureSample.a;

    // left or right pixel?
    const float y = uint(texCoord.x * textureWidth) & 1 ? y1 : y0;

    return ConvertYUVtoRGB(float3(y, u, v));
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    return texture0.Sample(sampler0, input.tex);
}