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
struct CameraIntrinsics
{
    uint imageWidth;
    uint imageHeight;
};

struct DepthMapInformation
{
    float depthMapUnitsToMeters;
    float depthRangeMinimumInMeters;
    float depthRangeMaximumInMeters;
};

struct DepthMapper
{
    CameraIntrinsics intrinsics;
    DepthMapInformation info;
    Texture2D<float> depthMap;
    Texture2D<float2> unprojectionMap;
    SamplerState unprojectionSampler;
};

float ReadDepthAsMeters(DepthMapper mapper, int2 pixel)
{
    float depth = mapper.depthMap.Load(int3(pixel, 0)) * mapper.info.depthMapUnitsToMeters;

    // discard any values outside of [depthRangeMinimum, depthRangeMaximum]
    if (depth > mapper.info.depthRangeMaximumInMeters || depth < mapper.info.depthRangeMinimumInMeters)
    {
        depth = 0.0f;
    }

    return depth;
}

float2 SampleUnprojectionMap(DepthMapper mapper, float2 texCoord)
{
    return mapper.unprojectionMap.SampleLevel(mapper.unprojectionSampler, texCoord, 0);
}

// Map from [depthMin, depthMax] to [0, 1]
float PackDepth(float depthInMeters, DepthMapInformation info)
{
    return (depthInMeters - info.depthRangeMinimumInMeters) / (info.depthRangeMaximumInMeters - info.depthRangeMinimumInMeters);
}

// Map from [0, 1] to [depthMin, depthMax]
float UnpackDepth(float depthNormalized, DepthMapInformation info)
{
    return depthNormalized * (info.depthRangeMaximumInMeters - info.depthRangeMinimumInMeters) + info.depthRangeMinimumInMeters;
}

struct CameraIntrinsicsProjection
{
    CameraIntrinsics intrinsics;
    float4x4 projectionMatrix;
    Texture2D<float2> distortionMap;
    SamplerState distortionSampler;
};

float2 ImageSpaceToTexCoord(CameraIntrinsics intrinsics, float2 imageSpace)
{
    return imageSpace / float2(float(intrinsics.imageWidth), float(intrinsics.imageHeight));
}

// CameraIntrinsics model is Y+ up, but ImageSpace and TextureCoordinates are Y+ down. 
// This function will convert between them.
float2 ConvertImageSpace(CameraIntrinsics intrinsics, float2 input)
{
    return float2(input.x, float(intrinsics.imageHeight) - input.y);
}

float2 ProjectAndDistortToImageSpace(CameraIntrinsicsProjection projection, float3 cameraCoordinate)
{
    // Project 3D point onto the pinhole camera:
    const float2 undistortedCoordinate = cameraCoordinate.xy / cameraCoordinate.z;
    const float4 imageSpace = mul(projection.projectionMatrix, float4(undistortedCoordinate, 0.0, 1.0));

    // Convert from the Camera's ImageSpace to texture coordinates [0, imageWidth] -> [0, 1]
    const float2 texCoord = ImageSpaceToTexCoord(projection.intrinsics, imageSpace.xy);

    return projection.distortionMap.SampleLevel(projection.distortionSampler, texCoord, 0);
}

float3 ProjectAndDistortToNDCSpace(CameraIntrinsicsProjection projection, float3 cameraCoordinate, DepthMapInformation info)
{
    const float2 imageSpace = ProjectAndDistortToImageSpace(projection, cameraCoordinate);

    // Convert to Y+ up for NDC space.
    const float2 convertedImageSpace = ConvertImageSpace(projection.intrinsics, imageSpace);

    // Normalize to [0,1]
    const float2 texCoord = ImageSpaceToTexCoord(projection.intrinsics, convertedImageSpace);

    // Scale to [0,2] and translate to [-1,1], i.e. NDC space
    const float2 ndcCoord = texCoord * 2.0f - 1.0f;

    // Scale depth from camera space to normalized space
    return float3(ndcCoord, PackDepth(cameraCoordinate.z, info));
}

// Given these vertexIds, where am I located?
// Note: Y+ is down, X+ is right
//
//  0-----1    5
//  |    /    /|
//  |  /    /  |
//  |/    /    |
//  2    4-----3

static const int2 vertexIdToOffset[6] = {
    { 0, 0 },
    { 1, 0 },
    { 0, 1 },

    { 1, 1 },
    { 0, 1 },
    { 1, 0 },
};

// Given these vertexIds, where are the neighbors?
// Note: Y+ is down, X+ is right
//
//  0-----1    5
//  |    /    /|
//  |  /    /  |
//  |/    /    |
//  2    4-----3

static const int2 vertexIdToNeighborOffsets[6][2] = {
    { { +1, +0 },{ +0, +1 } },
    { { -1, +0 },{ -1, +1 } },
    { { +0, -1 },{ +1, -1 } },

    { { -1, +0 },{ +0, -1 } },
    { { +1, +0 },{ +1, -1 } },
    { { +0, +1 },{ -1, +1 } },
};

float GetDepthAtPixel(DepthMapper mapper, int2 currentPixel, uint vertexIndex)
{
    // How far away can neighbors be before we cull this triangle? 
    static const float neighborDiscontinuityThresholdInMeters = 0.10f;

    float currentDepthInMeters = ReadDepthAsMeters(mapper, currentPixel);

    [unroll]
    for (int i = 0; i < 2; ++i)
    {
        const int2 neighborOffset = vertexIdToNeighborOffsets[vertexIndex][i];

        const float neighborDepthInMeters = ReadDepthAsMeters(mapper, currentPixel + neighborOffset);

        // This neighbor is too far away, mark ourselves as invalid so the triangle is culled
        if (abs(neighborDepthInMeters - currentDepthInMeters) > neighborDiscontinuityThresholdInMeters)
        {
            currentDepthInMeters = 0.0f;
        }
    }

    return currentDepthInMeters;
}

////////////////////////////////////////////////////////////////////////////////
// Returns the the 3D point represented by a pixel in the depth map.
float3 Get3DPointAtDepthPixel(DepthMapper mapper, int2 depthPixel, uint vertexIndex)
{
    const float depth = GetDepthAtPixel(mapper, depthPixel, vertexIndex);

    if (depth == 0.0f)
    {
        // return Quiet NaN for invalid points, this will result in the triangle being culled
        return asfloat(0x7FC00000).xxx;
    }
    else
    {
        // Determine where the 3D point is located by using the depth unprojection map
        const float2 texCoord = ImageSpaceToTexCoord(mapper.intrinsics, float2(depthPixel));

        const float2 unprojectionRay = SampleUnprojectionMap(mapper, texCoord);

        return float3(unprojectionRay * depth, depth);
    }
}