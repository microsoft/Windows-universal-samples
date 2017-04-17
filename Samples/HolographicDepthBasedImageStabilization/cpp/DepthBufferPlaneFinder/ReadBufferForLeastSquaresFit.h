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

#include "LeastSquaresData.h"

namespace HolographicDepthBasedImageStabilization {
    
    // This function uses CPU intrinsics to achieve a higher level of optimization
    // than would be possible with a math library.
    //
    // Note that the order of elements in _mm_set_ps is different from XMVECTOR.
    // We used the following code to validate that the elements are loaded in the
    // correct order:
    //    unsigned short data[] = { 0,1,2,3,4,5,6,7 };
    //   __m128  v1     = _mm_set_ps(data[3], data[2], data[1], data[0]);
    //   __m128i shorts = _mm_loadu_si128((__m128i*)data); 
    //   __m128  v2     = _mm_cvtepi32_ps(_mm_unpacklo_epi16(shorts, _mm_setzero_si128()));
    //   assert(memcmp(v2.m128_f32, v1.m128_f32, 4 * sizeof(float)) == 0);
    static void ReadBufferForLeastSquaresFit(
        LeastSquaresData& data,
        uint16_t* pData,
        float const* depthWeightArray,
        unsigned int const& rowPitch,
        unsigned int const& outWidth, 
        unsigned int const& outHeight,
        unsigned short const& invalidDepthValue)
    {
        using namespace DirectX;

        uint16_t* depthRow = reinterpret_cast<uint16_t*>(pData);
        for (unsigned int v = 0; v < outHeight; ++v)
        {
            __m128 sumU4          = _mm_setzero_ps();
            __m128 sumUU4         = _mm_setzero_ps();
            __m128 sumUV4         = _mm_setzero_ps();
            __m128 sumUW4         = _mm_setzero_ps();
            __m128 sumVW4         = _mm_setzero_ps();
            __m128 sumW4          = _mm_setzero_ps();
            __m128 sumWeights4    = _mm_setzero_ps();

            const __m128 v4 = _mm_set_ps1(static_cast<float>(v));
            __m128 u4 = _mm_set_ps(3, 2, 1, 0); // Pixel u-coordinates

            // Do 8 pixels at a time as a pair of 4-wide vectors. We do 8 so that we can 
            // load all 8 unsigned shorts in one go, but then we  transition to 4-wide 
            // floating point SSE for the actual work.
            // We will ignore a few columns at the end of each row, if the width is not
            // a multiple of 8.
            for (unsigned int u = 0; u + 7 < outWidth; u += 8)
            {
                // Compute (or look up) weights.
                // Indexing into the array here does not experience slowdown with the _mm_loadu_si128() just below.
                __m128 weights4lo = _mm_set_ps(
                    WeightDepth(depthRow[u + 3], depthWeightArray),
                    WeightDepth(depthRow[u + 2], depthWeightArray),
                    WeightDepth(depthRow[u + 1], depthWeightArray),
                    WeightDepth(depthRow[u + 0], depthWeightArray));
                __m128 weights4hi = _mm_set_ps(
                    WeightDepth(depthRow[u + 7], depthWeightArray),
                    WeightDepth(depthRow[u + 6], depthWeightArray),
                    WeightDepth(depthRow[u + 5], depthWeightArray),
                    WeightDepth(depthRow[u + 4], depthWeightArray));

                // The reciprocal of the depth values are stored in the depth map. Load up 8 of them because
                // that's the max we can do in one instruction
                const __m128i depthData = _mm_loadu_si128(reinterpret_cast<__m128i*>(depthRow + u)); 

                // Convert the int data loaded above to 8 floats. Store in two 4-wide SSE registers.
                const __m128 w4lo = _mm_cvtepi32_ps(_mm_unpacklo_epi16(depthData, _mm_setzero_si128()));
                const __m128 w4hi = _mm_cvtepi32_ps(_mm_unpackhi_epi16(depthData, _mm_setzero_si128()));
                
                // Accumulate first set of four pixels
                const __m128 weightedU4lo = _mm_mul_ps(weights4lo, u4);
                const __m128 weightedV4lo = _mm_mul_ps(weights4lo, v4);
                sumUU4       = _mm_add_ps(_mm_mul_ps(weightedU4lo, u4),   sumUU4);
                sumUV4       = _mm_add_ps(_mm_mul_ps(weightedU4lo, v4),   sumUV4);
                sumUW4       = _mm_add_ps(_mm_mul_ps(weightedU4lo, w4lo), sumUW4);
                sumVW4       = _mm_add_ps(_mm_mul_ps(weightedV4lo, w4lo), sumVW4);
                sumW4        = _mm_add_ps(_mm_mul_ps(weights4lo,   w4lo), sumW4);
                sumU4        = _mm_add_ps(sumU4, weightedU4lo);
                sumWeights4  = _mm_add_ps(sumWeights4, weights4lo);

                u4 = _mm_add_ps(u4, _mm_set_ps1(4));

                // Accumulate second set of four pixels
                const __m128 weightedU4hi = _mm_mul_ps(weights4hi, u4);
                const __m128 weightedV4hi = _mm_mul_ps(weights4hi, v4);
                sumUU4       = _mm_add_ps(_mm_mul_ps(weightedU4hi, u4),   sumUU4);
                sumUV4       = _mm_add_ps(_mm_mul_ps(weightedU4hi, v4),   sumUV4);
                sumUW4       = _mm_add_ps(_mm_mul_ps(weightedU4hi, w4hi), sumUW4);
                sumVW4       = _mm_add_ps(_mm_mul_ps(weightedV4hi, w4hi), sumVW4);
                sumW4        = _mm_add_ps(_mm_mul_ps(weights4hi,   w4hi), sumW4);
                sumU4        = _mm_add_ps(sumU4, weightedU4hi);
                sumWeights4  = _mm_add_ps(sumWeights4, weights4hi);

                u4 = _mm_add_ps(u4, _mm_set_ps1(4));
            }

            // Now add the four lanes horizontally together and add to the total
            // These horizontal sums are a bit slow, but we only do it once per row so it's probably ok
            // Could also do this at the end, but doing it per row should help with precision. 
            data.sumUU      += VectorHorizontalSum(sumUU4);
            data.sumUV      += VectorHorizontalSum(sumUV4);
            data.sumU       += VectorHorizontalSum(sumU4);
            data.sumW       += VectorHorizontalSum(sumW4);
            data.sumUW      += VectorHorizontalSum(sumUW4);
            data.sumVW      += VectorHorizontalSum(sumVW4);

            float sumWeightsRow = VectorHorizontalSum(sumWeights4);
            data.sumWeights += sumWeightsRow;
            data.sumVV      += sumWeightsRow * v * v;
            data.sumV       += sumWeightsRow * v;
        
            depthRow = reinterpret_cast<uint16_t*>(reinterpret_cast<byte*>(depthRow) + rowPitch);
        }
    }
}