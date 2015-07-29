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

#include "BasicTimer.h"
#include "DeviceResources.h"
#include "CustomPixelShaderRenderer.h"
#include "SampleOverlay.h"

// Renders Direct2D content on the screen.
namespace CustomPixelShader
{
    class CustomPixelShaderMain : public DX::IDeviceNotify
    {
    public:
        CustomPixelShaderMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~CustomPixelShaderMain();
        void CreateWindowSizeDependentResources();
        void UpdatePointer(_In_ Windows::UI::Core::PointerEventArgs^ args);
        void Update();
        bool Render();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        // Process lifetime management handlers.
        void SaveInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state);
        void LoadInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        std::unique_ptr<CustomPixelShaderRenderer> m_sceneRenderer;
        std::unique_ptr<SampleOverlay> m_sampleOverlay;
    };
}