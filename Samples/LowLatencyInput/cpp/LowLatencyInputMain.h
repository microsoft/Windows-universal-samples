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
#include "Content\RectRenderer.h"

// Renders Direct2D and 3D content on the screen.
namespace SDKTemplate
{
    class LowLatencyInputMain : public DX::IDeviceNotify
    {
    public:
        LowLatencyInputMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~LowLatencyInputMain();
        void StartRenderLoop();
        void StopRenderLoop();
        Concurrency::critical_section& GetCriticalSection() { return criticalSection; }
        
        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        void UpdateRectTransform(Windows::Foundation::Point newPosition, Windows::UI::Input::ManipulationDelta newDelta);
        void SetRectSize(float newSize);
        void SetBrushColor(D2D1::ColorF color);
        void ResetRect();
        bool HitTest( Windows::Foundation::Point position);

    private:
        void Render();

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> deviceResources;

        std::unique_ptr<RectRenderer> sceneRenderer;

        Windows::Foundation::IAsyncAction^ renderLoopWorker;
        Concurrency::critical_section criticalSection;

    };
}