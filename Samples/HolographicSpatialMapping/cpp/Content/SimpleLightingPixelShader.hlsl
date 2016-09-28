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

// A constant buffer that stores per-mesh data.
cbuffer ModelConstantBuffer : register(b0)
{
    float4x4      modelToWorld;
    min16float4x4 normalToWorld;
    min16float3   colorFadeFactor;
};

// A constant buffer that stores each set of view and projection matrices in column-major format.
cbuffer ViewProjectionConstantBuffer : register(b1)
{
    float4   cameraPosition;
    float4   lightPosition;
    float4x4 viewProjection[2];
};

// Per-pixel data.
struct PixelShaderInput
{
    min16float4 screenPos   : SV_POSITION;
    min16float3 worldPos    : POSITION0;
    min16float3 worldNorm   : NORMAL0;
    min16float3 color       : COLOR0;
    uint        idx         : TEXCOORD0;
};

// The pixel shader applies simplified Blinn-Phong BRDF lighting.
min16float4 main(PixelShaderInput input) : SV_TARGET
{
    min16float3 lightDiffuseColorValue = min16float3(1.f, 1.f, 1.f);
    min16float3 objectBaseColorValue    = min16float3(input.color);

    // N is the surface normal, which points directly away from the surface.
    min16float3 N = normalize(input.worldNorm);

    // L is the light incident vector, which is a normal that points from the surface to the light.
    min16float3 lightIncidentVectorNotNormalized = min16float3(lightPosition.xyz - input.worldPos);
    min16float  distanceFromSurfaceToLight = length(lightIncidentVectorNotNormalized);
    min16float  oneOverDistanceFromSurfaceToLight = min16float(1.f) / distanceFromSurfaceToLight;
    min16float3 L = normalize(lightIncidentVectorNotNormalized);

    // V is the camera incident vector, which is a normal that points from the surface to the camera.
    min16float3 V = normalize(min16float3(cameraPosition.xyz - input.worldPos));

    // H is a normalized vector that is halfway between L and V.
    min16float3 H = normalize(L + V);

    // We take the dot products of N with L and H.
    min16float nDotL = dot(N, L);
    min16float nDotH = dot(N, H);

    // The dot products should be clamped to 0 as a lower bound.
    min16float clampedNDotL = max(min16float(0.f), nDotL);
    min16float clampedNDotH = max(min16float(0.f), nDotH);

    // We can then use dot(N, L) to determine the diffuse lighting contribution.
    min16float3 diffuseColor = lightDiffuseColorValue * objectBaseColorValue * clampedNDotL;

    // The specular contribution is based on dot(N, H).
    const min16float  specularExponent   = min16float(4.f);
    const min16float3 specularColorValue = min16float3(1.f, 1.f, 0.9f);
    const min16float3 specularColor      = specularColorValue * pow(clampedNDotH, specularExponent) * oneOverDistanceFromSurfaceToLight;

    // Now, we can sum the ambient, diffuse, and specular contributions to determine the lighting value for the pixel.
    const min16float3 surfaceLitColor = objectBaseColorValue * min16float(0.2f) + diffuseColor * min16float(0.6f) + specularColor * min16float(0.2f);

    // In this example, new surfaces are treated differently by highlighting them in a different
    // color. This allows you to observe changes in the spatial map that are due to new meshes,
    // as opposed to mesh updates.
    const min16float3 oneMinusColorFadeFactor = min16float3(1.f, 1.f, 1.f) - (min16float3)colorFadeFactor;
    const min16float3 fadedColor = (surfaceLitColor * oneMinusColorFadeFactor) + (min16float3(0.75f, 0.1f, 0.1f) * (min16float3)colorFadeFactor);

    return min16float4(fadedColor, 1.f);
}
