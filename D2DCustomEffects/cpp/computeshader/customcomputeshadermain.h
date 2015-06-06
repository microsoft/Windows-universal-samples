#pragma once

#include "DeviceResources.h"
#include "CustomComputeShaderRenderer.h"
#include "SampleOverlay.h"

// Renders Direct2D content on the screen.
namespace CustomComputeShader
{
    class CustomComputeShaderMain : public DX::IDeviceNotify
    {
    public:
        CustomComputeShaderMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~CustomComputeShaderMain();
        void CreateWindowSizeDependentResources();
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

        std::unique_ptr<CustomComputeShaderRenderer> m_sceneRenderer;
        std::unique_ptr<SampleOverlay> m_sampleOverlay;
    };
}