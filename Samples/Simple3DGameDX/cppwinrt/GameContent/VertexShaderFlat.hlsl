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

#include "ConstantBuffers.hlsli"

PixelShaderFlatInput main(VertextShaderInput input)
{
    PixelShaderFlatInput output = (PixelShaderFlatInput)0;

    output.position = mul(mul(mul(input.position, world), view), projection);
    output.textureUV = input.textureUV;

    // compute view space normal
    float3 normal = normalize (mul(mul(input.normal.xyz, (float3x3)world), (float3x3)view));

    // Vertex pos in view space (normalize in pixel shader)
    float3 vertexToEye = -mul(mul(input.position, world), view).xyz;

    // Compute view space vertex to light vectors (normalized)
    float3 vertexToLight0 = normalize(mul(lightPosition[0], view ).xyz + vertexToEye);
    float3 vertexToLight1 = normalize(mul(lightPosition[1], view ).xyz + vertexToEye);
    float3 vertexToLight2 = normalize(mul(lightPosition[2], view ).xyz + vertexToEye);
    float3 vertexToLight3 = normalize(mul(lightPosition[3], view ).xyz + vertexToEye);

    output.diffuseColor = diffuseColor * 
        (max(0.0f, dot(normal, vertexToLight0)) +
        max(0.0f, dot(normal, vertexToLight1)) +
        max(0.0f, dot(normal, vertexToLight2)) +
        max(0.0f, dot(normal, vertexToLight3))) * 0.5f;

    return output;
}