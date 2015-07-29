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

namespace D2DPhotoAdjustment
{
    // Stores photo adjustment property values that are set on
    // the Direct2D effect pipeline.
    struct PhotoAdjustmentProperties
    {
    public:
        double StraightenValue;
        double TemperatureValue;
        double TintValue;
        double SaturationValue;
        double ContrastValue;
        double ShadowsValue;
        double ClarityValue;
        double HighlightsValue;
        double HsMaskRadiusValue;
    };
}