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

namespace CameraStreamCoordinateMapper
{
    // Constant buffer used to send MVP matrices to the vertex shader.
    struct alignas(16) ModelViewProjectionConstantBuffer
    {
        DirectX::XMFLOAT4X4 model;
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 projection;
    };

    struct VertexPositionTex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 tex;
    };

    struct alignas(16) CameraIntrinsics_Shader
    {
        uint32_t imageWidth;
        uint32_t imageHeight;
    };

    struct alignas(16) RasterizeDepthMeshConstants_Shader
    {
        CameraIntrinsics_Shader depthIntrinsics;
        CameraIntrinsics_Shader sourceIntrinsics;
        CameraIntrinsics_Shader targetIntrinsics;
        Windows::Foundation::Numerics::float4x4 depthToSource;
        Windows::Foundation::Numerics::float4x4 depthToTarget;
        Windows::Foundation::Numerics::float4x4 sourceProjectionMatrix;
        Windows::Foundation::Numerics::float4x4 targetProjectionMatrix;
        float depthMapUnitsToMeters;
        float depthRangeMinimumInMeters;
        float depthRangeMaximumInMeters;
        uint32_t depthMapImageWidthMinusOne;
    };

    struct alignas(16) VisualizeDepthMeshConstants_Shader
    {
        Windows::Foundation::Numerics::float4x4 worldToProj;
    };
}