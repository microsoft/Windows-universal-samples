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
    void ApplyLinearSmoothingToLeastSquares(
        DirectX::XMMATRIX const& newATAMatrixIn, 
        DirectX::XMVECTOR const& newATbVectorIn, 
        DirectX::XMMATRIX const& oldATAMatrixIn, 
        DirectX::XMVECTOR const& oldATbVectorIn, 
        float const& currentFrameBias, 
        DirectX::XMMATRIX& ataMatrixOut, 
        DirectX::XMVECTOR& atbVectorOut)
    {
        using namespace DirectX;

        // Here, we use error detection to avoid letting edge cases affect the result.
        if (XMVector4IsNaN(oldATbVectorIn))
        {
            ataMatrixOut = newATAMatrixIn;
            atbVectorOut = newATbVectorIn;
        }
        else
        {
            ataMatrixOut = oldATAMatrixIn + ((newATAMatrixIn - oldATAMatrixIn) * currentFrameBias);
            atbVectorOut = oldATbVectorIn + ((newATbVectorIn - oldATbVectorIn) * currentFrameBias);
        }
    }
}