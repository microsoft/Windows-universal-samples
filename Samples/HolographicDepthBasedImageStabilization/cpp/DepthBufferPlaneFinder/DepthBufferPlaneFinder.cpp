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

#include "pch.h"
#include "DepthBufferPlaneFinder.h"
#include "Common\DirectXHelper.h"

#include "ApplyLinearSmoothingToLeastSquares.h"
#include "ReadBufferForLeastSquaresFit.h"


#define HOLOLENS_SCREEN_WIDTH 1268
#define HOLOLENS_SCREEN_HEIGHT 720

#define IDEAL_FRAME_RATE 60.f

#define ENABLE_SMOOTHING
//#define ENABLE_COERCE

using namespace HolographicDepthBasedImageStabilization;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
DepthBufferPlaneFinder::DepthBufferPlaneFinder(
    const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
    Init(deviceResources);

    XMStoreFloat4x4(&m_smoothATA, DirectX::XMMatrixIdentity());
    XMStoreFloat4(&m_smoothATb, DirectX::XMVectorZero());
}

void DepthBufferPlaneFinder::Init(
    const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
    m_deviceResources = deviceResources;

    CreateDeviceDependentResources();

    QueryPerformanceFrequency(&m_qpcFrequency);
    QueryPerformanceCounter(&m_qpcLastFrame);
}

void DepthBufferPlaneFinder::SetProjectionMatrix(
    float4x4 const& projectionMatrix)
{
    // DepthBufferPlaneFinder uses the projection matrix in column-major format.
    XMMATRIX newProjectionMatrixTranspose = XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix));
    
    // If we already have the same projection matrix, we can exit early.
    XMMATRIX currentProjectionMatrixTranspose = XMLoadFloat4x4(&m_projectionMatrix);
    if (XMVector4Equal(currentProjectionMatrixTranspose.r[0],newProjectionMatrixTranspose.r[0]) &&
        XMVector4Equal(currentProjectionMatrixTranspose.r[1],newProjectionMatrixTranspose.r[1]) &&
        XMVector4Equal(currentProjectionMatrixTranspose.r[2],newProjectionMatrixTranspose.r[2]) &&
        XMVector4Equal(currentProjectionMatrixTranspose.r[3],newProjectionMatrixTranspose.r[3]))
    {
        return;
    }

    // This is a new matrix. Store it, and compute the constants for it.
    XMStoreFloat4x4(&m_projectionMatrix, newProjectionMatrixTranspose);

    // NOTE: The rest of this function is called infrequently, so it is not highly optimized.

    // Here, we acquire the scale and offset (2D transform) from "depth space" to clip space.
    assert(m_projectionMatrix.m[3][2] != 0.f);
    const float inv_e = 1.0f / m_projectionMatrix.m[3][2];

    // This is the scale and offset from projected space, to clip space.
    const XMFLOAT2 clipFromProjectedScale = {
        m_projectionMatrix.m[0][0] * inv_e,
        m_projectionMatrix.m[1][1] * inv_e };
    const XMFLOAT2 clipFromProjectedOffset = {
        m_projectionMatrix.m[0][2] * inv_e,
        m_projectionMatrix.m[1][2] * inv_e };

    // Next, we go from clip space - which is in the range [-1, 1] - to the range [0, 1].
    const XMFLOAT2 zeroOneFromClipScale = { 0.5f, 0.5f };
    const XMFLOAT2 zeroOneFromClipOffset = { 0.5f, 0.5f };

    // This is the scale and offset from [0, 1] to pixel coordinates.
    const XMFLOAT2 originalPixelCentersFromZeroOneScale = {
        static_cast<float>(HOLOLENS_SCREEN_WIDTH),
        static_cast<float>(HOLOLENS_SCREEN_HEIGHT) };
    const XMFLOAT2 originalPixelCentersFromZeroOneOffset = { -0.5f, -0.5f };

    // Since we sub-sample the depth buffer, we have to go from the full screen size to the
    // scaled depth buffer size.
    const XMFLOAT2 subSampledFromOriginalScale = {
        1.0f / c_subSampleRate,
        1.0f / c_subSampleRate };
    const XMFLOAT2 subSampledFromOriginalOffset = { 0.0f, 0.0f };

    // Here, we combine the scale elements of each transform using the distributive property
    // of multiplication, then take the inverse by finding the reciprocal.
    const XMVECTOR scale1 = XMLoadFloat2(&subSampledFromOriginalScale) * XMLoadFloat2(&originalPixelCentersFromZeroOneScale);
    const XMVECTOR scale2 = scale1 * XMLoadFloat2(&zeroOneFromClipScale);
    const XMVECTOR scale3 = scale2 * XMLoadFloat2(&clipFromProjectedScale);
    const XMVECTOR inverseScale = XMVectorDivide(XMVectorReplicate(1.f), scale3);
    XMStoreFloat2(&m_projectedFromPixelCentersScale, inverseScale);

    // Here, we combine the offset elements of each transform by sequentially scaling and adding 
    // the previous offset.
    const XMVECTOR projectedFromPixelCentersOffsetInverse1 =
        XMLoadFloat2(&subSampledFromOriginalOffset) +
        (XMLoadFloat2(&originalPixelCentersFromZeroOneOffset) * XMLoadFloat2(&subSampledFromOriginalScale));
    const XMVECTOR projectedFromPixelCentersOffsetInverse2 =
        projectedFromPixelCentersOffsetInverse1 + 
        (XMLoadFloat2(&zeroOneFromClipOffset) * scale1);
    const XMVECTOR projectedFromPixelCentersOffsetInverse3 =
        projectedFromPixelCentersOffsetInverse2 + 
        (XMLoadFloat2(&clipFromProjectedOffset) * scale2);
    
    // We store the inverse of the transform.
    XMStoreFloat2(
        &m_projectedFromPixelCentersOffset,
        -projectedFromPixelCentersOffsetInverse3 * inverseScale);

    // We use the inverse transforms computed above to construct a transform matrix that turns a 
    // pixel center position in u, v coordinates into a 2D ray in projective space, which we will
    // use in the least-squares plane fit.
    m_pixelCenterToRayAffine.m[0][0] = m_projectedFromPixelCentersScale.x;
    m_pixelCenterToRayAffine.m[0][1] = 0.0f;
    m_pixelCenterToRayAffine.m[0][2] = m_projectedFromPixelCentersOffset.x;
    m_pixelCenterToRayAffine.m[1][0] = 0.0f;
    m_pixelCenterToRayAffine.m[1][1] = m_projectedFromPixelCentersScale.y;
    m_pixelCenterToRayAffine.m[1][2] = m_projectedFromPixelCentersOffset.y;
    m_pixelCenterToRayAffine.m[2][0] = 0.f;
    m_pixelCenterToRayAffine.m[2][1] = 0.f;
    m_pixelCenterToRayAffine.m[2][2] = 1;

    // Now, we determine the depth buffer transform.
    
    // This is the transform from depth clip space to inverse-Z space.
    const float clipFromInvZScale  = m_projectionMatrix.m[2][3] * inv_e;
    const float clipFromInvZOffset = m_projectionMatrix.m[2][2] * inv_e;

    // This transform is from clip space to buffer space. If your clip space is from [-1, 1], then 
    // you should change both of these values to 0.5f.
    const float bufFromClipScale  = 1.0f;
    const float bufFromClipOffset = 0.0f;

    // We use a depth buffer made of 16-bit unsigned integers, so the max depth value is 65,535 (UINT16_MAX).
    const float bufFromUnitBufScale  = static_cast<float>(UINT16_MAX);
    const float bufFromUnitBufOffset = 0.f;

    // As above, we combine the transform scale and offset components, and store the inverse.
    // The depth value will be transformed from unsigned 16-bit integer range, into projective space by
    // using this inverse scale and offset.
    m_valueToMetricInverseDepthScale  = 1.0f / (bufFromUnitBufScale * bufFromClipScale * clipFromInvZScale);
    const float zFromBufferOffset     = bufFromUnitBufOffset + (bufFromUnitBufScale * (bufFromClipOffset + (bufFromClipScale * clipFromInvZOffset)));
    m_valueToMetricInverseDepthOffset = -zFromBufferOffset * m_valueToMetricInverseDepthScale;
}

void DepthBufferPlaneFinder::ConvertToMetric(
    LeastSquaresData const& data,
    XMMATRIX& metricATA,
    XMVECTOR& metricATb) const
{
    // Load from data.
    XMFLOAT3X3 pixelSpaceATA;
    XMFLOAT4 pixelSpaceATb;
    pixelSpaceATA.m[0][0] = static_cast<float>(data.sumUU);
    pixelSpaceATA.m[0][1] = static_cast<float>(data.sumUV);
    pixelSpaceATA.m[0][2] = static_cast<float>(data.sumU);
    pixelSpaceATA.m[1][0] = pixelSpaceATA.m[0][1];
    pixelSpaceATA.m[1][1] = static_cast<float>(data.sumVV);
    pixelSpaceATA.m[1][2] = static_cast<float>(data.sumV);
    pixelSpaceATA.m[2][0] = pixelSpaceATA.m[0][2];
    pixelSpaceATA.m[2][1] = pixelSpaceATA.m[1][2];
    pixelSpaceATA.m[2][2] = static_cast<float>(data.sumWeights);

    pixelSpaceATb.x = static_cast<float>(data.sumUW);
    pixelSpaceATb.y = static_cast<float>(data.sumVW);
    pixelSpaceATb.z = static_cast<float>(data.sumW);
    pixelSpaceATb.w = 1.0f;
    
    // Convert to metric.
    {
        // Set up affine.
        XMMATRIX xy_from_uv1 = XMLoadFloat3x3(&m_pixelCenterToRayAffine);

        // Set up depth transform.
        XMVECTOR invz_from_w1 = XMVectorSet(m_valueToMetricInverseDepthScale, m_valueToMetricInverseDepthOffset, 0.f, 0.f);

        // Left-multiply ATA by affine.
        XMMATRIX affineATA = xy_from_uv1 * XMLoadFloat3x3(&pixelSpaceATA);

        // Right-multiply by the transpose of the affine transform matrix.
        metricATA = affineATA * XMMatrixTranspose(xy_from_uv1);

        // ATb_metric <-- affine * ATb_pixel * depth_affine'
        // First transform from depth-buffer values to metric inverse depth
        // To compute the affine transform here need the un-multiplied-by-W 
        // terms too
        XMVECTOR invDepthRhs;
        XMVECTOR valueToMetricInverseDepthScale = XMVectorReplicate(m_valueToMetricInverseDepthScale);
        XMVECTOR valueToMetricInverseDepthOffset = XMVectorReplicate(m_valueToMetricInverseDepthOffset);
        
        XMVECTOR sumTermOne = XMVectorSet((float)data.sumUW, (float)data.sumVW, (float)data.sumW, 1.f);
        XMVECTOR sumTermTwo = XMVectorSet((float)data.sumU, (float)data.sumV, (float)data.sumWeights, 1.f);
        invDepthRhs = XMVectorSetW(
            XMVectorMultiplyAdd(valueToMetricInverseDepthScale, sumTermOne, XMVectorMultiply(valueToMetricInverseDepthOffset, sumTermTwo)),
            1.f);

        // Left-multiply by affine.
        metricATb = XMVector3Transform(invDepthRhs, XMMatrixTranspose(xy_from_uv1));
    }
}

bool DepthBufferPlaneFinder::TrySolveLeastSquaresFit(
    XMMATRIX const& ATA,
    XMVECTOR const& ATb,
    XMVECTOR& outPlane) const
{
    assert(XMVectorGetW(ATA.r[0]) == 0.f);
    assert(XMVectorGetW(ATA.r[1]) == 0.f);
    assert(XMVectorGetW(ATA.r[2]) == 0.f);
    assert(XMVectorGetX(ATA.r[3]) == 0.f);
    assert(XMVectorGetY(ATA.r[3]) == 0.f);
    assert(XMVectorGetZ(ATA.r[3]) == 0.f);
    assert(XMVectorGetW(ATA.r[3]) != 0.f);

    // Notes:
    //   * This determinant is only used to know if the matrix is singular (cannot be inverted).
    //   * Even though ATA is a 3x3 matrix, the 4x4 representation of it must still be non-
    //     singular - that is, we still expect element [4,4] to be non-zero.
    XMVECTOR determinant;
    XMMATRIX inverseOfATA = XMMatrixInverse(&determinant, ATA);
    
    if (XMVector4Equal(determinant, XMVectorZero()))
    {
        return false;
    }
    else
    {
        outPlane = XMVector3Transform(ATb, inverseOfATA);
        outPlane = XMVectorSetW(outPlane, -1.0f);

        return true;
    }
}


// Make a prior estimate based on two constraints:
// 1. Plane normal is along z=1.
// 2. Inverse-distance along z-axis to plane is given.
// N.b. (2) is not really "distance to plane"; that constraint
// cannot be easily incorporated.
void DepthBufferPlaneFinder::MakeSimplePrior(
    float frontoParallelInformation,
    float inverseDistanceInformation,
    float inversePlaneDistance,
    XMMATRIX& priorATA,
    XMVECTOR& priorATb) const
{
    priorATA.r[0] = XMVectorSet(frontoParallelInformation, 0.f, 0.f, 0.f);
    priorATA.r[1] = XMVectorSet(0.f, frontoParallelInformation, 0.f, 0.f);
    priorATA.r[2] = XMVectorSet(0.f, 0.f, frontoParallelInformation, 0.f);
    priorATA.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    priorATb      = XMVectorSet(0.f, 0.f, sqrt(inverseDistanceInformation) * inversePlaneDistance, 0.f);
}

void DepthBufferPlaneFinder::Coerce(
    XMVECTOR const& planeEq,
    XMMATRIX const& ATA,
    XMVECTOR const& ATb,
    XMVECTOR& coercedPlaneEq) const
{
    // Note that planeEq, etc. are treated as 3-element vectors by this function.

    assert(XMVectorGetW(planeEq)  == -1.f);
    assert(XMVectorGetW(ATA.r[0]) ==  0.f);
    assert(XMVectorGetW(ATA.r[1]) ==  0.f);
    assert(XMVectorGetW(ATA.r[2]) ==  0.f);

    const float z = XMVectorGetZ(planeEq);
    constexpr float epsilon = 1.0e-24f;
    if ((-epsilon < z) && (z < epsilon))
    {
        coercedPlaneEq = planeEq;
        return;
    }

    const float sideways = XMVectorGetX(XMVector2LengthEst(planeEq));
    const float along = fabsf(z);
    const float tanTheta = sideways / along;
    if (tanTheta < 1.0e-4f)
    {
        // Plane is very nearly fronto-parallel. Nothing to do.
        coercedPlaneEq = planeEq;
    }
    else
    {
        const float asFrac = atanf(tanTheta) / XM_PIDIV2; // 0: fronto-parallel 1: 90 deg off
        const float remappedFrac = asFrac - powf(asFrac, 8.0f);
        const float remappedAngle = XM_PIDIV2 * remappedFrac;
        const float remappedTan = tanf(remappedAngle);

        const float ratio = remappedTan / tanTheta;
        XMVECTOR splat = XMVectorSet(ratio, ratio, 1.f, 0.f);
        XMVECTOR newNormal = planeEq * splat;

        // Have new direction. Now solve for distance, i.e. for scale factor on newNormal.
        // PlaneEq = [ s * newNormal  -1]'
        // d[abc]/ds = newNormal

        const float newInfo = XMVectorGetX(XMVector3Dot(newNormal, (XMVector3Transform(newNormal, ATA))));
        const float newGrad = XMVectorGetX(XMVector3Dot(newNormal, ATb));

        assert(newInfo != 0.0f);

        const float scale = newGrad / newInfo;

        // Tack on a w-value of -1 to complete the 4-element vector.
        coercedPlaneEq = XMVectorSetW(scale * newNormal, -1.f);
    }
}

void DepthBufferPlaneFinder::StoreEstimatedPlane(
    XMVECTOR const& planeIn, 
    float3& planeOut,
    float& stabilizationDistanceOut) const
{
    assert(!XMVectorGetX(XMVectorIsNaN(planeIn)));

    // Estimate the magnitude of the equation.
    float directionMagnitude = XMVectorGetX(XMVector3LengthEst(planeIn));
    if (directionMagnitude == 0.0f)
    {
        directionMagnitude = 1.0f;
    }

    // Normalize the plane equation.
    const float invDirectionMagnitude = 1.0f / directionMagnitude;
    XMVECTOR unitNormal = planeIn * XMVectorReplicate(invDirectionMagnitude);
    const float distance = XMVectorGetW(unitNormal);
    
    // Find point on plane.
    float z = XMVectorGetZ(unitNormal);
    constexpr float epsilon = 1.0e-24f;
    if (((-epsilon < z) && (z < epsilon)) &&
        ((-epsilon < distance) && (distance < epsilon)))
    {
        // Guard against invalid results.
        stabilizationDistanceOut = -1.f;
    }
    else
    {
        stabilizationDistanceOut = distance / z;

        // Store the plane equation.
        XMStoreFloat3(&planeOut, unitNormal);
    }
}

inline float DepthBufferPlaneFinder::GetSmoothingFramerateAdjustmentMultiplier()
{
    // Determine frame time.
    LARGE_INTEGER qpcThisFrame;
    QueryPerformanceCounter(&qpcThisFrame);

    const unsigned long long qpcDifference = qpcThisFrame.QuadPart - m_qpcLastFrame.QuadPart;
    const double timeBetweenFrames = static_cast<double>(qpcDifference) / 
                                     static_cast<double>(m_qpcFrequency.QuadPart);

    // Store the QPC result for the next frame.
    m_qpcLastFrame = qpcThisFrame;

    // Relate the frame time back to the ideal frame time.
    return min(1.f, static_cast<float>(timeBetweenFrames) * IDEAL_FRAME_RATE);
}

void DepthBufferPlaneFinder::DispatchDepthResolve(
    ID3D11DeviceContext* d3dContext, 
    DX::CameraResources* pCameraResources, 
    float const& maxDepth, 
    unsigned int const& outW, 
    unsigned int const& outH) const
{
    // Update the constant buffer.
    XMFLOAT4 parameters;
    parameters.x = maxDepth;
    d3dContext->UpdateSubresource(
        m_constantBuffer.Get(),
        0,
        nullptr,
        &parameters,
        0,
        0);

    // Unset the depth buffer so we can do a custom resolve.
    d3dContext->OMSetRenderTargets(0, nullptr, nullptr);

    // This is where we'll output the resolved depth values.
    ID3D11UnorderedAccessView* resolvedDepthView = m_depthBufferMappedOnGpu ?
        pCameraResources->GetResolvedDepthTextureViewMappable() : 
        pCameraResources->GetResolvedDepthTextureView();
    d3dContext->CSSetUnorderedAccessViews(0, 1, &resolvedDepthView, nullptr);

    // This is the depth buffer generated in the previous frame.
    // At 60FPS, which most holographic apps should run at, a one-frame lag time is not 
    // significant in generating the stabilization plane.
    ID3D11ShaderResourceView* depthSRV = pCameraResources->GetDepthShaderResourceView();
    d3dContext->CSSetShaderResources(0, 1, &depthSRV);
    d3dContext->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    d3dContext->CSSetShader(m_depthResolveComputeShader.Get(), nullptr, 0);

    // This sample arbitrarily uses 16x2 threads per group.
    // NOTE: The application should be measured for best dispatch dimension performance.
    d3dContext->Dispatch(
        outW / 16 + (outW % 16 != 0), // ceilf(static_cast<float>(outW) / 16.f), 
        outH /  2 + (outH %  2 != 0), // ceilf(static_cast<float>(outH) / 2.f),
        1);

    // Clear the compute shader resource slots.
    constexpr ID3D11ShaderResourceView*  dummySRV = nullptr;
    constexpr ID3D11UnorderedAccessView* dummyUAV = nullptr;
    constexpr ID3D11Buffer*              dummyBuffer = nullptr;
    d3dContext->CSSetShaderResources     (0, 1, &dummySRV);
    d3dContext->CSSetUnorderedAccessViews(0, 1, &dummyUAV, nullptr);
    d3dContext->CSSetConstantBuffers     (0, 1, &dummyBuffer);
}

bool DepthBufferPlaneFinder::MapResourceAndSumLeastSquares(
    ID3D11DeviceContext* d3dContext, 
    DX::CameraResources* pCameraResources, 
    float const& maxDepth, 
    float const& minDepth, 
    unsigned int const& outW, 
    unsigned int const& outH, 
    LeastSquaresData& data) const
{
    // Process the current frame's depth values using the CPU.
    ID3D11Buffer* resolvedDepthBufferCPUMappable = m_depthBufferMappedOnGpu ? 
        pCameraResources->GetMappableDefaultDepthTexture() : 
        pCameraResources->GetCPUResolvedDepthTexture();

    // Map the depth values for CPU read.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    DX::ThrowIfFailed(
        d3dContext->Map(resolvedDepthBufferCPUMappable, 0, D3D11_MAP_READ, 0, &mappedResource)
        );
    
    // NOTE: If your max value is something other than UINT16_MAX (65535) or 0, you will need to 
    //       modify this somehow - e.g. pass in the invalid depth value.
    const unsigned short invalidDepthValue = (maxDepth > minDepth) ? UINT16_MAX : 0;

    const unsigned int rowPitch = outW * sizeof(uint16_t);
    ReadBufferForLeastSquaresFit(
        data, 
        reinterpret_cast<uint16_t*>(mappedResource.pData), 
        pCameraResources->GetDepthWeightArray(),
        rowPitch, 
        outW, 
        outH, 
        invalidDepthValue);

    // Unmap the resource.
    d3dContext->Unmap(resolvedDepthBufferCPUMappable, 0);

    if (!m_depthBufferMappedOnGpu)
    {
        // Now schedule a copy on the GPU for the current frame's depth values, so we can read 
        // them back next frame.
        d3dContext->CopyResource(
            pCameraResources->GetCPUResolvedDepthTexture(),
            pCameraResources->GetResolvedDepthTexture());
    }

    return true;
}

// Renders one frame using the vertex and pixel shaders.
// On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
// VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
// a pass-through geometry shader is also used to set the render 
// target array index.
bool DepthBufferPlaneFinder::TryFindPlaneNormalAndDistance(
    DX::CameraResources* pCameraResources, 
    float3& outPlaneNormalInCameraSpace, 
    float& outDistanceToPlaneInMeters)
{
    // Loading is asynchronous. Resources must be created before drawing can occur.
    if (!m_loadingComplete)
    {
        return false;
    }

    const auto d3dContext = m_deviceResources->GetD3DDeviceContext();
    
    UINT numViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    D3D11_VIEWPORT viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    d3dContext->RSGetViewports(&numViewports, viewports);
    const float minDepth = viewports[0].MinDepth;
    
    float maxDepth = viewports[0].MaxDepth;
    if ((maxDepth == 0) && (minDepth == 0))
    {
        maxDepth = 1.f;
    }

    // Each thread reduces the input buffer by the factor specified using c_subSampleRate.
    const int outputWidth  = lround(HOLOLENS_SCREEN_WIDTH)  / c_subSampleRate;
    const int outputHeight = lround(HOLOLENS_SCREEN_HEIGHT) / c_subSampleRate;

    // Map the CPU-readable graphics resource and use it for least-squares fit.
    // By now, the previous frame's dispatch should be done, or close enough to being done that
    // blocking with the call to Map() is not likely to cause significant delay. Note that this
    // is not true if the engine does not enforce a 1-frame delay on CPU, at most.
    LeastSquaresData data;
    const bool couldMapResource = MapResourceAndSumLeastSquares(d3dContext, pCameraResources, maxDepth, minDepth, outputWidth, outputHeight, data);

    // Dispatch the compute shader.
    // Once this happens, the output buffer should not be mapped until the dispatch is completed.
    DispatchDepthResolve(d3dContext, pCameraResources, maxDepth, outputWidth, outputHeight);
    
    // Quit if there was an error reading the data.
    if (!couldMapResource) return false;
        
    // Convert to metric.
    XMMATRIX metricATA;
    XMVECTOR metricATb;
    ConvertToMetric(data, metricATA, metricATb);

#ifdef ENABLE_SMOOTHING
    // Compute the smoothed result.

    // Determine the smoothing multiplier to adjust for frame render time.
    const float smoothingMultiplier = GetSmoothingFramerateAdjustmentMultiplier();

    // Adjust smoothing constant for framerate.
    const float currentFrameBiasAdjustedForFramerate = smoothingMultiplier * c_currentFrameBias;

    // Apply linear smoothing to the linear system, (A'A) x = A'b
    XMMATRIX smoothATA = XMLoadFloat4x4(&m_smoothATA);
    XMVECTOR smoothATb = XMLoadFloat4(&m_smoothATb);
    ApplyLinearSmoothingToLeastSquares(
        metricATA, metricATb, 
        smoothATA, smoothATb, 
        currentFrameBiasAdjustedForFramerate,
        smoothATA, smoothATb);
    XMStoreFloat4x4(&m_smoothATA, smoothATA);
    XMStoreFloat4(&m_smoothATb, smoothATb);
#endif
        
    // NOTE: The prior inverse distance is hard-coded using a negative value. If your depth buffer
    //       produces positive values in the unmodified ATb, change this to positive.
    XMMATRIX priorATA;
    XMVECTOR priorATb;

    MakeSimplePrior(
        c_frontoParallelBias, // frontoParallelBias: default value 1.0f
        c_distanceBias,       // distance bias: default value 1.0f 
        -0.5f,
        priorATA, 
        priorATb);

    // Combine the info and gradient with the simple prior.
#ifdef ENABLE_SMOOTHING
    XMMATRIX combinedATA = priorATA + smoothATA;
    XMVECTOR combinedATb = priorATb + smoothATb;
#else
    XMMATRIX combinedATA = priorATA + metricATA;
    XMVECTOR combinedATb = priorATb + metricATb;
#endif
    
    // Try to do the least-squares fit.
    // This can fail if ATA is not invertible.
    XMVECTOR plane;
    const bool planeEstimationSuccess = TrySolveLeastSquaresFit(
        combinedATA, 
        combinedATb,
        plane);

#ifdef ENABLE_COERCE
    Coerce(plane, combinedATA, combinedATb, plane);
#endif

    if (planeEstimationSuccess)
    {
        StoreEstimatedPlane(plane, outPlaneNormalInCameraSpace, outDistanceToPlaneInMeters);

        // Assert the distance is in front of the camera.
        assert(outDistanceToPlaneInMeters > 0.f);

        // Assert the plane result is a number (not NaN).
        assert(outPlaneNormalInCameraSpace == outPlaneNormalInCameraSpace);
    }

    return planeEstimationSuccess;
}

void DepthBufferPlaneFinder::CreateDeviceDependentResources()
{
    // Load shaders asynchronously.
    task<std::vector<byte>> loadCSTask = DX::ReadDataAsync(L"ms-appx:///DepthResolve.cso");

    task<void> createCSTask = loadCSTask.then([this](const std::vector<byte>& fileData)
    {
        // Create the compute shader.
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateComputeShader(
                &fileData[0],
                fileData.size(),
                nullptr,
                &m_depthResolveComputeShader
                )
            );

        // Create the constant buffer.
        D3D11_BUFFER_DESC desc;
        memset (&desc, 0, sizeof(desc));

        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(float) * 4; // hold 1 floating-point parameter + 3 padding floats for 16-byte packing boundary
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&desc, NULL, &m_constantBuffer)
            );
    });
    
    task<void> getFeatureOptionsTask = create_task([this]()
    {
        auto d3dDevice = m_deviceResources->GetD3DDevice();

        // Determine whether current Direct3D device can support mapping default buffers. If not, 
        // fall back to using staging buffers to access data on the CPU.
        D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureOptions;
        DX::ThrowIfFailed(
            d3dDevice->CheckFeatureSupport(
                D3D11_FEATURE_D3D11_OPTIONS1,
                &featureOptions,
                sizeof(featureOptions))
            );

        m_depthBufferMappedOnGpu = featureOptions.MapOnDefaultBuffers ? true : false;
    });

    task<void> finalTask = (createCSTask && getFeatureOptionsTask).then([this]()
    {
        m_loadingComplete = true;
    });
}

void DepthBufferPlaneFinder::ReleaseDeviceDependentResources()
{
    m_loadingComplete  = false;
    m_depthResolveComputeShader.Reset();
    m_constantBuffer.Reset();
}
