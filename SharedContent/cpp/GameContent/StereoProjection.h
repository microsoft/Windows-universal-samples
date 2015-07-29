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

typedef struct STEREO_PARAMETERS
{
    float viewerDistanceInches;
    float displaySizeInches;
    float pixelResolutionWidth;
    float pixelResolutionHeight;
    float stereoSeparationFactor;
    float stereoExaggerationFactor;
} STEREO_PARAMETERS;

// Enumeration for stereo channels (left and right).
typedef enum class STEREO_CHANNEL
{
    LEFT = 0,
    RIGHT
} STEREO_CHANNEL;

// Enumeration for stereo mode (normal or inverted).
typedef enum class STEREO_MODE
{
    NORMAL = 0,
    INVERTED,
} STEREO_MODE;

void StereoCreateDefaultParameters(_Out_ STEREO_PARAMETERS* pStereoParameters);

DirectX::XMMATRIX MatrixStereoProjectionFovLH(
    _In_opt_ const STEREO_PARAMETERS* stereoParameters,
    STEREO_CHANNEL channel,
    float fovAngleY,
    float aspectHByW,
    float nearZ,
    float farZ,
    STEREO_MODE stereoMode
    );

