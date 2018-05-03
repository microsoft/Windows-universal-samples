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

#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"

namespace CameraStreamCoordinateMapper
{
    // Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
    class TextRenderer
    {
    public:
        enum class TextAlignment
        {
            TopLeft, TopRight, BottomLeft, BottomRight
        };

        TextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Update(std::wstring_view text);
        void Render(TextAlignment alignment);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>           m_deviceResources;

        // Resources related to text rendering.
        DWRITE_TEXT_METRICS	                           m_textMetrics = {};
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>   m_whiteBrush;
        Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock> m_stateBlock;
        Microsoft::WRL::ComPtr<IDWriteTextLayout>      m_textLayout;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>      m_textFormat;
    };
}