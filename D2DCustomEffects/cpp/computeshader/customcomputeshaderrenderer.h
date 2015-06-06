#pragma once

#include "DeviceResources.h"
#include "DftEffect.h"

namespace CustomComputeShader
{
    // This sample renderer instantiates a basic rendering pipeline.
    class CustomComputeShaderRenderer
    {
    public:
        CustomComputeShaderRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceIndependentResources();
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void Update() {};
        void Render();

        // Process lifetime management handlers.
        void SaveInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state) {};
        void LoadInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state) {};

    private:
        void UpdateImageScale(float scale);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                m_deviceResources;
        Microsoft::WRL::ComPtr<IWICFormatConverter>         m_formatConvert;
        Microsoft::WRL::ComPtr<ID2D1ImageSourceFromWic>     m_imageSource;
        Microsoft::WRL::ComPtr<ID2D1TransformedImageSource> m_transformedImage;
        Microsoft::WRL::ComPtr<ID2D1Effect>                 m_dftEffect;

        bool                                                m_isWindowClosed;
        D2D1_SIZE_U                                         m_imageSize;
        float                                               m_scale;
    };
}

