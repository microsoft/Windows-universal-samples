//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "DeviceResources.h"
#include "DWriteColorTextRendererRenderer.h"
#include "SampleOverlay.h"

// Renders Direct2D and 3D content on the screen.
namespace DWriteColorTextRenderer
{
    class DWriteColorTextRendererMain : public DX::IDeviceNotify
    {
    public:
        DWriteColorTextRendererMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~DWriteColorTextRendererMain();
        void UpdateForWindowSizeChange();
        bool Render();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // Sample renderer class.
        std::unique_ptr<DWriteColorTextRendererRenderer> m_sceneRenderer;

        // Sample overlay class.
        std::unique_ptr<SampleOverlay> m_sampleOverlay;
    };
}