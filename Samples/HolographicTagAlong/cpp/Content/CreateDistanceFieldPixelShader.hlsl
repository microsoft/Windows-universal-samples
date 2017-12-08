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

// The pixel multiplier is a function of the ratio of source to target resolution.
#define PIXEL_MULTIPLIER_X 8
#define PIXEL_MULTIPLIER_Y 8

// Ideal kernel size is coupled to the source texture resolution.
#define KERNEL_SIZE             64
#define KERNEL_RADIUS          (KERNEL_SIZE / 2)
#define ONE_OVER_KERNEL_SIZE   (1.f / KERNEL_SIZE)

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    min16float4 screenSpacePos : SV_POSITION;
};

Texture2D tex : t0;

// Writes the output of a signed distance function, centered at the pixel's position, searching
// along the X and Y axes for the closest line boundary in each direction.
min16float2 main(PixelShaderInput input) : SV_TARGET
{
    // The pixel center should be taken in source texture space.
    int2 pixelCenter = int2(input.screenSpacePos.x, input.screenSpacePos.y) * 
                       int2(PIXEL_MULTIPLIER_X, PIXEL_MULTIPLIER_Y);

    // Determine where the pixel is in relation to the line boundary.
    // If the pixel is above the threshold, it is inside the region that will be drawn.
    const float thresholdForActivePixel = 0.5f;
    bool pixelIsIn = tex.Load(int3(pixelCenter, 0)).x >= thresholdForActivePixel;
    
    // Search for the closest pixel of the opposing status.
    const int   veryLargeDistanceSquared = 1e9;
    int2        minDistance = veryLargeDistanceSquared.xx;
    for (int i = 0; i < KERNEL_SIZE; ++i)
    {
        // Offset to use in this iteration of the bidirectional search.
        int    offset = i - KERNEL_RADIUS;

        // Look up the current X and Y pixels.
        int3   pixelXLocation = int3(pixelCenter + int2(offset, 0), 0);
        int3   pixelYLocation = int3(pixelCenter + int2(0, offset), 0);
        float2 texelValues    = float2(tex.Load(pixelXLocation).x, tex.Load(pixelYLocation).x);
            
        // Determine whether or not the pixel is of the opposing status, in vs. out.
        bool2  neighboringPixelIsInteresting = false;
        /*
        if (pixelIsIn)
        {
            neighboringPixelIsInteresting.x = texelValues.x < thresholdForActivePixel;
            neighboringPixelIsInteresting.y = texelValues.y < thresholdForActivePixel;
        }
        else
        {
            neighboringPixelIsInteresting.x = texelValues.x >= thresholdForActivePixel;
            neighboringPixelIsInteresting.y = texelValues.y >= thresholdForActivePixel;
        }*/
        // This provides a result equivalent to the comparisons above, but without branching.
        neighboringPixelIsInteresting = 
            (pixelIsIn  & (texelValues <  thresholdForActivePixel)) |
          ((!pixelIsIn) & (texelValues >= thresholdForActivePixel));

        // For interesting pixels only, store the distance if it is closer.
        int distance = abs(offset);
        minDistance = neighboringPixelIsInteresting && (distance < minDistance) ? distance : minDistance;
    }

    // Transform the distance function from the range [-KERNEL_RADIUS, KERNEL_RADIUS] to the range [0, 1].
    // -KERNEL_RADIUS should map to 0, and KERNEL_RADIUS should map to 1.
    float2 mappedDistanceValue = saturate(minDistance * ONE_OVER_KERNEL_SIZE + 0.5f);

    // The distance function is signed.
    if (pixelIsIn)
    {
        // A falloff adds some accuracy to the edge interpolation.
        mappedDistanceValue = mappedDistanceValue * mappedDistanceValue;
    }
    else
    {
        // A falloff adds some accuracy to the edge interpolation.
        // Also transform the pixel to the range [0.0, 0.5].
        mappedDistanceValue = -mappedDistanceValue * mappedDistanceValue + 0.5f;
    }

    return min16float2(mappedDistanceValue.xy);
}
