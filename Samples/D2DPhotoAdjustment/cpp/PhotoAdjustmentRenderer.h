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
#include "PhotoAdjustmentProperties.h"

namespace D2DPhotoAdjustment
{
    class PhotoAdjustmentRenderer : public DX::IDeviceNotify
    {
    public:
        PhotoAdjustmentRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~PhotoAdjustmentRenderer();

        void CreateDeviceIndependentResources();
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void OnColorProfileChanged(_In_ Windows::Graphics::Display::DisplayInformation^ sender);
        void UpdatePhotoAdjustmentValues(PhotoAdjustmentProperties properties);
        void Draw();

        // IDeviceNotify methods handle device lost and restored.
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        void UpdateZoomState();
        void UpdateDisplayColorContext(_In_ Windows::Storage::Streams::DataReader^ colorProfileDataReader);
        float ConvertDipProperty(float valueInDips);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                    m_deviceResources;
        Platform::Agile<Windows::UI::Input::GestureRecognizer>  m_gestureRecognizer;
        Microsoft::WRL::ComPtr<IWICFormatConverter>             m_formatConvert;

        // Device-independent resources.
        Microsoft::WRL::ComPtr<IWICColorContext>                m_wicColorContext;

        // Device-dependent resources.
        Microsoft::WRL::ComPtr<ID2D1ImageSourceFromWic>         m_imageSource;
        Microsoft::WRL::ComPtr<ID2D1TransformedImageSource>     m_scaledImage;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_colorManagement;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_straighten;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_temperatureTint;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_saturation;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_contrast;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_highlightsShadows;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_outputEffect;

        // Image view state.
        Windows::Foundation::Size                               m_imageSize;
        Windows::Foundation::Size                               m_panelSize;
        float                                                   m_zoom;
        bool                                                    m_isWindowClosed;

        // Effect properties measured in DIPs.
        float                                                   m_hsMaskRadiusDips;

        // Adjustable parameters.
        float                                                   m_maxZoom;
    };
}