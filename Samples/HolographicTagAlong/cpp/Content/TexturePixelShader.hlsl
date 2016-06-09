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
    min16float4 pos         : SV_POSITION;
    min16float3 color       : COLOR0;
    min16float2 texCoord    : TEXCOORD1;
};

Texture2D       tex         : t0;
SamplerState    samp        : s0;

// The pixel shader renders texture, which may be modified using a color value.
min16float4 main(PixelShaderInput input) : SV_TARGET
{
    min16float3 textureValue = (min16float3)(tex.Sample(samp, input.texCoord));
    min16float3 multiplier   = input.color;
    return min16float4(textureValue * multiplier, 1.f);
}
