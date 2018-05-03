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

#include "CameraIntrinsicsHelpers.hlsl"

struct VertexInput
{
    uint vertID : SV_VertexID;
    uint instID : SV_InstanceID;
};

struct VertexOutput
{
    float4 pos : SV_POSITION;
    float2 sourceTexCoord : TEXCOORD0;
};

cbuffer DepthCoordinateMapperConstants : register(b0)
{
    CameraIntrinsics depthIntrinsics;
    CameraIntrinsics sourceIntrinsics;
    CameraIntrinsics targetIntrinsics;
    float4x4 depthToSource;
    float4x4 depthToTarget;
    float4x4 sourceProjectionMatrix;
    float4x4 targetProjectionMatrix;
    float depthMapUnitsToMeters;
    float depthRangeMinimumInMeters;
    float depthRangeMaximumInMeters;
    uint depthMapImageWidthMinusOne;
};

Texture2D<float> depthTexture : register(t0);
Texture2D<float2> depthUnprojectionMap : register(t1);
Texture2D<float2> sourceDistortionMap : register(t2);
Texture2D<float2> targetDistortionMap : register(t3);

SamplerState defaultSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////
// The core algorithm:
// 1.) Read each point from the depth image and undistort + unprojects it into 3D space (using depthUnprojectionMap)
// 2.) Transforms that 3D point into "source" and "target" camera space (using depthToSource and depthToTarget matrices)
// 3.) Projects + distorts it into source camera 2D space (using sourceProjectionMatrix and sourceDistortionMap) 
//     to determine which pixel to sample from 
// 4.) Projects + distorts it into target camera 2D space (using targetProjectionMatrix and targetDistortionMap) 
//     to determine where the final pixel is rendered on screen. 
//
// The most common scenario has "source" and "target" as the same camera (Color), but 
// the algorithm is generic to allow other scenarios. 

VertexOutput main(VertexInput input)
{
    // We need to figure out which point in the quad we are located at, so we can sample the depth map
    //
    // - There are 6 vertices in each quad which is represented as a triangle list
    //
    // - Each vertex of the quad has an id: SV_VertexId
    //
    // Example Quad (split to show the 2 distinct triangles)
    //
    //   0----1*5     0-----1    5
    //   |    /|      |    /    /|
    //   |  /  | ---> |  /    /  |
    //   |/    |      |/    /    |
    //  2*4----3      2    4-----3
    //
    ////////////////////////////////////////////////////////////////////////////////
    //
    // - There are (width-1) quads per row, and (height-1) quads per column.
    //
    // - Each quad is an instance with an id: SV_InstanceId
    //
    // Example Depth Image of 4x3 which gives 6 quads:
    //
    //   0-----1-----2-----*
    //   |    /|    /|    /|
    //   |  /  |  /  |  /  |
    //   |/    |/    |/    |
    //   3-----4-----5-----*
    //   |    /|    /|    /|
    //   |  /  |  /  |  /  |
    //   |/    |/    |/    |
    //   *-----*-----*-----*
    //
    // Using SV_VertexID and SV_InstanceID we can determine which pixel to look-up in the depth image
    //

    VertexOutput output;

    DepthMapper mapper;
    mapper.intrinsics = depthIntrinsics;
    mapper.info.depthMapUnitsToMeters = depthMapUnitsToMeters;
    mapper.info.depthRangeMinimumInMeters = depthRangeMinimumInMeters;
    mapper.info.depthRangeMaximumInMeters = depthRangeMaximumInMeters;
    mapper.depthMap = depthTexture;
    mapper.unprojectionMap = depthUnprojectionMap;
    mapper.unprojectionSampler = defaultSampler;

    CameraIntrinsicsProjection sourceProjection;
    sourceProjection.intrinsics = sourceIntrinsics;
    sourceProjection.projectionMatrix = sourceProjectionMatrix;
    sourceProjection.distortionMap = sourceDistortionMap;
    sourceProjection.distortionSampler = defaultSampler;

    CameraIntrinsicsProjection targetProjection;
    targetProjection.intrinsics = targetIntrinsics;
    targetProjection.projectionMatrix = targetProjectionMatrix;
    targetProjection.distortionMap = targetDistortionMap;
    targetProjection.distortionSampler = defaultSampler;

    ////////////////////////////////////////////////////////////////////////////////
    // Step 1.) Sample the depth map 
    const uint vertId = input.vertID;
    const int2 vertOffset = vertexIdToOffset[vertId];

    const int x = vertOffset.x + input.instID % depthMapImageWidthMinusOne;
    const int y = vertOffset.y + input.instID / depthMapImageWidthMinusOne;

    const int2 depthPixel = int2(x, y);

    const float4 depthCameraPoint = float4(Get3DPointAtDepthPixel(mapper, depthPixel, vertId), 1.0f);

    ////////////////////////////////////////////////////////////////////////////////
    // Step 2.) Transforms that 3D point into "source" and "target" camera space
    const float4 targetCameraPoint = mul(depthToTarget, depthCameraPoint);
    const float4 sourceCameraPoint = mul(depthToSource, depthCameraPoint);

    ////////////////////////////////////////////////////////////////////////////////
    // Step 3.) Project and distort into "source" camera space to determine which pixel to sample from 
    const float2 sourceImageSpace = ProjectAndDistortToImageSpace(sourceProjection, sourceCameraPoint.xyz);

    output.sourceTexCoord = ImageSpaceToTexCoord(sourceIntrinsics, sourceImageSpace);

    ////////////////////////////////////////////////////////////////////////////////
    // Step 4.) Project and distort into "target" camera space for final rendering
    output.pos = float4(ProjectAndDistortToNDCSpace(targetProjection, targetCameraPoint.xyz, mapper.info), 1.0f);

    return output;
}