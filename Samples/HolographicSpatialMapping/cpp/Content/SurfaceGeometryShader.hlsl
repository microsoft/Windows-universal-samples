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

// Per-vertex data from the vertex shader.
struct GeometryShaderInput
{
    min16float4 screenPos   : SV_POSITION;
    min16float3 worldPos    : POSITION0;
    min16float3 worldNorm   : NORMAL0;
    min16float3 color       : COLOR0;
    uint        instId      : TEXCOORD0;
};

// Per-vertex data passed to the rasterizer.
struct GeometryShaderOutput
{
    min16float4 screenPos   : SV_POSITION;
    min16float3 worldPos    : POSITION0;
    min16float3 worldNorm   : NORMAL0;
    min16float3 color       : COLOR0;
    uint        idx         : TEXCOORD0;
    uint        rtvId       : SV_RenderTargetArrayIndex;
};

// This geometry shader is a pass-through that leaves the geometry unmodified
// and sets the render target array index.
[maxvertexcount(3)]
void main(triangle GeometryShaderInput input[3], inout TriangleStream<GeometryShaderOutput> outStream)
{
    GeometryShaderOutput output;
    [unroll(3)]
    for (int i = 0; i < 3; ++i)
    {
        output.screenPos = input[i].screenPos;
        output.worldPos = input[i].worldPos;
        output.worldNorm  = input[i].worldNorm;
        output.color = input[i].color;
        output.idx   = input[i].instId;
        output.rtvId = input[i].instId;
        outStream.Append(output);
    }
}
