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

namespace HolographicDepthBasedImageStabilization
{
    struct LeastSquaresData
    {
        // Sum(JTJ) and Sum(JTw) are accumulated as explicit element-wise products.
        double sumUU        = 0;
        double sumUV        = 0;
        double sumVV        = 0;
        double sumU         = 0;
        double sumV         = 0;
        double sumW         = 0;
        double sumUW        = 0;
        double sumVW        = 0;
        double sumWeights   = 0;
    };
}

inline float __vectorcall VectorHorizontalSum(__m128 x)
{
    __m128 temp = _mm_hadd_ps(x, x);
    return _mm_cvtss_f32(_mm_hadd_ps(temp, temp));
}