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

#include "..\Common\DeviceResources.h"
#include "LeastSquaresData.h"

namespace HolographicDepthBasedImageStabilization
{
    // DepthBufferPlaneFinder uses least-squares fit to estimate the plane that best fits the 
    // visible geometry of a scene by examining data in a depth buffer.
    //
    // We consider samples of the depth buffer as points in pixel space:
    //     [u v 1 inv_z]'
    //
    // The equation for the plane is:
    //     a*u + b*v + c = inverse_z
    //
    // The coefficients we do least squares over are a, b, and c. The plane is represented in vector 
    // form as:
    //     [a b c -1]
    //
    // Least-squares fit estimates the plane equation given enough samples of u, v, and inverse_z, 
    // which are used to construct the normal equation. The normal equation has the following form:
    //     A'Ax = A'b
    // 
    // The solution is found by multiplying against the inverse of A'A:
    //     x = [(A'A)^-1] A'b
    //
    // The normal equation components are stored - instead of the solution - so that techniques can
    // be applied to the components to manipulate the result. The C++ syntax used for the normal 
    // equation components is ATA and ATb. These acronyms are used in parameter names, fields, and 
    // so on throughout this code.
    // 
    // Before estimating the plane, we smooth ATA and ATb across frames and combine the result with 
    // a simple prior. This ensures reasonable behavior of the plane with fast motion, and when 
    // little or no data is available.
    // 
    // Note that the plane fit uses data gathered from the depth buffer for the previous frame. The 
    // compute shader output buffer is mapped by the CPU, which can potentially stall until the 
    // resource is available; waiting one frame for the dispatch call to complete provides better 
    // timing to avoid blocking for too long.
    //
    class DepthBufferPlaneFinder
    {
    public:
        DepthBufferPlaneFinder() { };
        DepthBufferPlaneFinder(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void Init(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void SetProjectionMatrix(
            Windows::Foundation::Numerics::float4x4 const& projectionMatrix);
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        bool TryFindPlaneNormalAndDistance(
            DX::CameraResources* cameraResources,
            Windows::Foundation::Numerics::float3& outPlaneNormalInCameraSpace,
            float& outDistanceToPointInCameraSpace);

    private:

        // Uses the projection matrix constants to convert from projective space back to metric.
        void ConvertToMetric(
            LeastSquaresData const& data,
            DirectX::XMMATRIX& metricATA,
            DirectX::XMVECTOR& metricATb) const;

        // Attempts to solve the linear equation. Assumes that the incoming matrix ATA is (A'A), 
        // and ATb is (A'b).
        //
        // In DepthBufferPlaneFinder, ATA and ATb are 3x3 and 3x1. Four-dimensional vector math 
        // is used to compute the result to allow the use of CPU intrinsics. The extra row and 
        // column of ATA are set to [0, 0, 0, 1].
        //
        // If ATA is singular, i.e. it does not have an inverse, this function will return false.
        // Otherwise, it will return true.
        //
        bool TrySolveLeastSquaresFit(
            DirectX::XMMATRIX const& ATA,
            DirectX::XMVECTOR const& ATb,
            DirectX::XMVECTOR& outPlane) const;

        // Creates a valid prior, with a 3x3 ATA and 3x1 ATb.
        void MakeSimplePrior(
            float frontoParallelInformation,
            float inverseDistanceInformation,
            float inversePlaneDistance,
            DirectX::XMMATRIX& priorATA,
            DirectX::XMVECTOR& priorATb) const;

        // Only uses the first 3 elements of planeEq. A w-value of -1 is tacked onto the output.
        void Coerce(
            DirectX::XMVECTOR const& planeEq,
            DirectX::XMMATRIX const& ATA,
            DirectX::XMVECTOR const& ATb,
            DirectX::XMVECTOR& coercedPlaneEq) const;

        // Expects planeIn to be a 4-dimensional vector.
        void StoreEstimatedPlane(
            DirectX::XMVECTOR const& planeIn, 
            Windows::Foundation::Numerics::float3& planeOut, 
            float& stabilizationDistanceOut) const;

        inline float GetSmoothingFramerateAdjustmentMultiplier();

        void DispatchDepthResolve(
            ID3D11DeviceContext* d3dContext, 
            DX::CameraResources* pCameraResources, 
            float const& maxDepth, 
            unsigned int const& outputWidth, 
            unsigned int const& outputHeight) const;
        
        bool MapResourceAndSumLeastSquares(
            ID3D11DeviceContext* d3dContext, 
            DX::CameraResources* pCameraResources, 
            float const& maxDepth, 
            float const& minDepth, 
            unsigned int const& outputWidth, 
            unsigned int const& outputHeight, 
            LeastSquaresData& data) const;
    

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>        m_deviceResources;

        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_depthResolveComputeShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>        m_constantBuffer;
        
        // Variables used with the rendering loop.
        bool                    m_loadingComplete                   = false;
        bool                    m_depthBufferMappedOnGpu            = false;

        // Variables used with least-squares fit.        
        const unsigned int      c_subSampleRate                     = DEPTH_BUFFER_SUBSAMPLE_RATE;
        const float             c_distanceBias                      = 1.0f;
        const float             c_frontoParallelBias                = 1.0f;
        const float             c_priorFramesBias                   = 0.8f;
        const float             c_currentFrameBias                  = 0.2f;
        float                   m_valueToMetricInverseDepthScale    = 1.f;
        float                   m_valueToMetricInverseDepthOffset   = 0.f;

        DirectX::XMFLOAT4X4     m_smoothATA;
        DirectX::XMFLOAT4       m_smoothATb;
        

        // Projection coefficients.
        // These remain constant with the projection matrix.
        DirectX::XMFLOAT4X4     m_projectionMatrix;
        DirectX::XMFLOAT3X3     m_pixelCenterToRayAffine;
        DirectX::XMFLOAT2       m_projectedFromPixelCentersScale;
        DirectX::XMFLOAT2       m_projectedFromPixelCentersOffset;

        LARGE_INTEGER           m_qpcFrequency;
        LARGE_INTEGER           m_qpcLastFrame;
    };
}