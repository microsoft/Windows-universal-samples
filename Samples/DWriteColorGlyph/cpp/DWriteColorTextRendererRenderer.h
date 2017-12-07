//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "DeviceResources.h"
#include "CustomTextRenderer.h"

namespace DWriteColorTextRenderer
{
    // This sample renderer instantiates a basic rendering pipeline.
    class DWriteColorTextRendererRenderer
    {
    public:
        DWriteColorTextRendererRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void Render();

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // Sample-specific resources.
        Microsoft::WRL::ComPtr<IDWriteTextLayout>  m_dwriteTextLayout;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_dwriteTextFormat;
        Microsoft::WRL::ComPtr<CustomTextRenderer> m_textRenderer;
    };
}
