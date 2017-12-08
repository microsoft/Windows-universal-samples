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

cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 model;
    float2   texCoordScale;
    float2   texCoordOffset;
}

// A constant buffer that stores each set of view and projection matrices in column-major format.
cbuffer ViewProjectionConstantBuffer : register(b1)
{
    float4x4 viewProjection[2];
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    min16float3 pos      : POSITION0;
    min16float2 texCoord : TEXCOORD0;
    uint        instId   : SV_InstanceID;
};

// Per-vertex data passed to the geometry shader.
// Note that the render target array index will be set by the geometry shader
// using the value of viewId.
struct VertexShaderOutput
{
    min16float4 pos      : SV_POSITION;
    min16float2 texCoord : TEXCOORD0;
#ifdef USE_VPRT
    uint        viewId   : SV_RenderTargetArrayIndex; // SV_InstanceID % 2
#else
    uint        viewId   : TEXCOORD16;  // SV_InstanceID % 2
#endif
};

// Simple shader to do vertex processing on the GPU.
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float4 pos = float4(input.pos, 1.0f);

    // Note which view this vertex has been sent to. Used for matrix lookup.
    // Taking the modulo of the instance ID allows geometry instancing to be used
    // along with stereo instanced drawing; in that case, two copies of each 
    // instance would be drawn, one for left and one for right.
    uint viewportIndex = input.instId % 2;

    // Transform the vertex position into world space.
    pos = mul(model, pos);

    // Correct for perspective and project the vertex position onto the screen.
    pos = mul(viewProjection[viewportIndex], pos);
    output.pos = (min16float4)pos;

    float2 texCoord = float2(input.texCoord);

    texCoord = texCoord * texCoordScale + texCoordOffset;

    // Pass through the texture coordinates with a scale and offset
    output.texCoord = (min16float2)texCoord;

    // Set the instance ID. The pass-through geometry shader will set the
    // render target array index to whatever value is set here.
    output.viewId = viewportIndex;

    return output;
}
