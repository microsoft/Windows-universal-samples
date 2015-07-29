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
#include "StereoProjection.h"

#if !defined(XMASSERT)
#define XMASSERT(Expression) ((void)0)
#endif // !XMASSERT

using namespace DirectX;

void StereoCreateDefaultParameters(_Out_ STEREO_PARAMETERS* stereoParameters)
{
    XMASSERT(stereoParameters != nullptr);

    // Default assumption is 1920x1200 resolution, a 22" LCD monitor, and a 2' viewing distance
    stereoParameters->viewerDistanceInches = 24.0f;
    stereoParameters->pixelResolutionWidth = 1920.0f;
    stereoParameters->pixelResolutionHeight = 1024.0f; // 480.0f; // 1200
    stereoParameters->displaySizeInches = 22.0f;

    stereoParameters->stereoSeparationFactor = 1.0f;
    stereoParameters->stereoExaggerationFactor = 1.0f;
}

__forceinline bool _XMMatrixStereoProjectionHelper (
    const STEREO_PARAMETERS* stereoParameters,
    _Out_ float* virtualProjection,
    _Out_ float* zNearWidth,
    _Out_ float* zNearHeight,
    float fovAngleY,
    float aspectHByW,
    float nearZ
    )
{
    // note that most people have difficulty fusing images into 3D
    // if the separation equals even just the human average. by
    // reducing the separation (interocular distance) by 1/2, we
    // guarantee a larger subset of people will see full 3D

    // the conservative setting should always be used. the only problem
    // with the conservative setting is that the 3D effect will be less
    // impressive on smaller screens (which makes sense, since your eye
    // cannot be tricked as easily based on the smaller fov). to simulate
    // the effect of a larger screen, use the liberal settings (debug only)

    // Conservative Settings: * max acuity angle: 0.8f degrees * interoc distance: 1.25 inches

    // Liberal Settings: * max acuity angle: 1.6f degrees * interoc distance: 2.5f inches

    // maximum visual accuity angle allowed is 3.2 degrees for
    // a physical scene, and 1.6 degrees for a virtual one.
    // thus we cannot allow an object to appear any closer to
    // the viewer than 1.6 degrees (divided by two for most
    // half-angle calculations)

    static const float maxStereoDistance = 85; // 780 inches (should be between 10 and 20m)
    static const float maxVisualAcuityAngle = 0.8f * (XM_PI / 180.0f);  // radians
    static const float interocularDistance = 1.25f; // inches

    bool comfortableResult = true;

    float displayHeight = stereoParameters->displaySizeInches / sqrtf(aspectHByW * aspectHByW + 1.0f);
    float displayWidth = displayHeight * aspectHByW;
    float halfInterocular = 0.5f * interocularDistance * stereoParameters->stereoExaggerationFactor;
    float halfPixelWidth  = displayWidth / stereoParameters->pixelResolutionWidth * 0.5f;
    float halfMaximumAcuityAngle = maxVisualAcuityAngle * 0.5f * stereoParameters->stereoExaggerationFactor;

    float maxSeparationAcuityAngle = atanf(halfInterocular / maxStereoDistance);
    float maxSeparationDistance = halfPixelWidth / tanf(maxSeparationAcuityAngle);
    float refinedMaxStereoDistance = maxStereoDistance - maxSeparationDistance;
    float fovHalfAngle = fovAngleY / 2.0f;

    if (refinedMaxStereoDistance < 0.0f || maxSeparationDistance > 0.1f * maxStereoDistance)
    {
        // Pixel resolution is too low to offer a comfortable stereo experience
        comfortableResult = false;
    }

    float refinedMaxSeparationAcuityAngle = atanf(halfInterocular / refinedMaxStereoDistance);
    float physicalZNearDistance = halfInterocular / tanf(halfMaximumAcuityAngle);

    float nearZSeparation = tanf(refinedMaxSeparationAcuityAngle) * (refinedMaxStereoDistance - physicalZNearDistance);

    *zNearHeight = cosf(fovHalfAngle) / sinf(fovHalfAngle);
    *zNearWidth = *zNearHeight / aspectHByW;
    *virtualProjection = (nearZSeparation * nearZ * (*zNearWidth * 4.0f)) / (2.0f * nearZ);

    return comfortableResult;
}


XMMATRIX MatrixStereoProjectionFovLH (
    _In_opt_ const STEREO_PARAMETERS* stereoParameters,
    STEREO_CHANNEL channel,
    float fovAngleY,
    float aspectHByW,
    float nearZ,
    float farZ,
    STEREO_MODE stereoMode
    )
{
    float virtualProjection = 0.0f;
    float zNearWidth = 0.0f;
    float zNearHeight = 0.0f;
    float invertedAngle;
    XMMATRIX patchedProjection;
    XMMATRIX proj;
    STEREO_PARAMETERS defaultParameters;

    XMASSERT(channel == STEREO_CHANNEL::LEFT || channel == STEREO_CHANNEL::RIGHT);
    XMASSERT(stereoMode == STEREO_MODE::NORMAL || stereoMode == STEREO_MODE::INVERTED);
    XMASSERT(!XMScalarNearEqual(fovAngleY, 0.0f, 0.00001f * 2.0f));
    XMASSERT(!XMScalarNearEqual(aspectHByW, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(farZ, nearZ, 0.00001f));

    proj = XMMatrixIdentity();

    if (stereoParameters == nullptr)
    {
        StereoCreateDefaultParameters(&defaultParameters);
        stereoParameters = &defaultParameters;
    }

    XMASSERT(pStereoParameters->stereoSeparationFactor >= 0.0f && stereoParameters->stereoSeparationFactor <= 1.0f);
    XMASSERT(pStereoParameters->stereoExaggerationFactor >= 1.0f && stereoParameters->stereoExaggerationFactor <= 2.0f);

    _XMMatrixStereoProjectionHelper(stereoParameters, &virtualProjection, &zNearWidth, &zNearHeight, fovAngleY, aspectHByW, nearZ);

    virtualProjection *= stereoParameters->stereoSeparationFactor; // incorporate developer defined bias

    //
    // By applying a translation, we are forcing our cameras to be parallel
    //

    invertedAngle = atanf(virtualProjection / (2.0f * nearZ));

    proj = XMMatrixPerspectiveFovLH(fovAngleY, aspectHByW, nearZ, farZ);

    if (channel == STEREO_CHANNEL::LEFT)
    {
        if (stereoMode > STEREO_MODE::NORMAL)
        {
            XMMATRIX rots, trans;
            rots = XMMatrixRotationY(invertedAngle);
            trans = XMMatrixTranslation(-virtualProjection, 0, 0);
            patchedProjection = XMMatrixMultiply(XMMatrixMultiply(rots, trans), proj);
        }
        else
        {
            XMMATRIX trans;
            trans = XMMatrixTranslation(-virtualProjection, 0, 0);
            patchedProjection = XMMatrixMultiply(trans, proj);
        }
    }
    else
    {
        if (stereoMode > STEREO_MODE::NORMAL)
        {
            XMMATRIX rots, trans;
            rots = XMMatrixRotationY(-invertedAngle);
            trans = XMMatrixTranslation(virtualProjection, 0, 0);
            patchedProjection = XMMatrixMultiply(XMMatrixMultiply(rots, trans), proj);
        }
        else
        {
            XMMATRIX trans;
            trans = XMMatrixTranslation(virtualProjection, 0, 0);
            patchedProjection = XMMatrixMultiply(trans, proj);
        }
    }

    return patchedProjection;
}

