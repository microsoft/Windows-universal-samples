#pragma once

#include "DeviceResources.h"
#include "BasicTimer.h"
#include "WaveEffect.h"

namespace CustomVertexShader
{
    // This sample renderer instantiates a basic rendering pipeline.
    class CustomVertexShaderRenderer
    {
    public:
        CustomVertexShaderRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceIndependentResources();
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void UpdateManipulationState(_In_ Windows::UI::Input::ManipulationUpdatedEventArgs^ args);
        void Update();
        void Render();

        // Process lifetime management handlers.
        void SaveInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state);
        void LoadInternalState(_In_ Windows::Foundation::Collections::IPropertySet^ state);

    private:
        void UpdateImageScale(float scale);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                    m_deviceResources;
        Platform::Agile<Windows::UI::Input::GestureRecognizer>  m_gestureRecognizer;
        Microsoft::WRL::ComPtr<IWICFormatConverter>             m_formatConvert;
        Microsoft::WRL::ComPtr<ID2D1ImageSourceFromWic>         m_imageSource;
        Microsoft::WRL::ComPtr<ID2D1TransformedImageSource>     m_transformedImage;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_waveEffect;

        D2D1_SIZE_U                                             m_imageSize;
        // Stretch the image in the X and Y dimensions.
        float                                                   m_skewX;
        float                                                   m_skewY;
        bool                                                    m_isWindowClosed;

        BasicTimer^                                             m_timer;
    };
}

