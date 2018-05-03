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
    float2 targetTexCoord : TEXCOORD0;
    float targetDepthNormalized : TEXCOORD1;
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

cbuffer VisualizeDepthMeshConstants : register(b1)
{
    float4x4 worldToProj;
}

Texture2D<float> depthTexture : register(t0);
Texture2D<float2> depthUnprojectionMap : register(t1);
Texture2D<float2> targetDistortionMap : register(t2);

SamplerState defaultSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////
// The core algorithm for visualizing from alternate perspectives:
// 1.) Read each point from the depth image and undistort + unprojects it into 3D space (using depthUnprojectionMap)
// 2.) Transforms that 3D point into "target" camera space (using depthToTarget matrices)
// 3.) Projects + distorts it into target camera 2D space (using targetProjectionMatrix and targetDistortionMap) 
//     to determine where the final pixel was rendered on screen (so we can sample the depth buffer in the pixel shader)
// 4.) Compute the depth of this point in "target" camera space and store it in the vertex
// 5.) Compute the position of the 3D point given the app provided camera/projection
// 6.) Discard pixels that were occluded from the target camera's perspective

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
    
    const float4 depthWorldPoint = float4(Get3DPointAtDepthPixel(mapper, depthPixel, vertId), 1.0f);
    
    ////////////////////////////////////////////////////////////////////////////////
    // Step 2.) Transforms that 3D point into "target" camera space
    const float4 targetCameraPoint = mul(depthToTarget, depthWorldPoint);
    
    ////////////////////////////////////////////////////////////////////////////////
    // Step 3.) Project and distort into "target" camera space to determine which pixel to sample from the 1st pass
    const float2 targetImageSpace = ProjectAndDistortToImageSpace(targetProjection, targetCameraPoint.xyz);
    
    output.targetTexCoord = ImageSpaceToTexCoord(targetIntrinsics, targetImageSpace);

    ////////////////////////////////////////////////////////////////////////////////
    // Step 4.) Store the depth of this vertex to test in the pixel shader for occlusion
    output.targetDepthNormalized = PackDepth(targetCameraPoint.z, mapper.info);

    ////////////////////////////////////////////////////////////////////////////////
    // Step 5.) Project for rendering in the free-roam perspective

    // CameraIntrinsics space is Z+ forward, but our rendering is Z- forward
    const float4 updatedCameraPoint = float4(targetCameraPoint.xy, -targetCameraPoint.z, 1.0f);

    output.pos = mul(worldToProj, updatedCameraPoint);

    return output;
}