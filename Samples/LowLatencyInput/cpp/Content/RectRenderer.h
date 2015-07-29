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
#include "..\Common\Constants.h"

namespace SDKTemplate
{
    // This sample renderer instantiates a basic rendering pipeline.
    class RectRenderer
    {

    public:
        RectRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Render();
        void UpdateRectTransform(Windows::Foundation::Point newPosition, Windows::UI::Input::ManipulationDelta newDelta);
        bool HitTest( Windows::Foundation::Point position);
        void SetRectSize(float newSize);
        void ResetRect();
        void SetBrushColor(D2D1::ColorF newColor);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> deviceResources;

        // A brush with which to draw the rect.
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;

        // The rect to be drawn.
        D2D1_RECT_F rect;

        //The rect's width/height
        float rectSize;

        //Maintains the transform of the rect
        D2D1::Matrix3x2F rectTransform;
    };
}

