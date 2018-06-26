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
    float2 sourceTexCoord : TEXCOORD0;
};

Texture2D sourceTexture : register(t0);
SamplerState defaultSampler : register(s0);

float4 main(VertexOutput input) : SV_TARGET
{
    return sourceTexture.Sample(defaultSampler, input.sourceTexCoord);
}