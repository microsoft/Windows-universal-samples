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

#include "DeviceResources.h"
#include "ToneMapEffect.h"

namespace SDKTemplate
{
    class D2DAdvancedColorImages : public DX::IDeviceNotify
    {
    public:
        D2DAdvancedColorImages(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~D2DAdvancedColorImages();

        void CreateDeviceIndependentResources();
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void Draw();

        void LoadImage(IStream* imageStream);
        void CreateImageDependentResources();
        void ReleaseImageDependentResources();
        void FitImageToWindow();

        void UpdateAdvancedColorState();

        // IDeviceNotify methods handle device lost and restored.
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                    m_deviceResources;

        // WIC and Direct2D resources.
        Microsoft::WRL::ComPtr<IWICFormatConverter>             m_formatConvert;
        Microsoft::WRL::ComPtr<IWICColorContext>                m_wicColorContext;
        Microsoft::WRL::ComPtr<ID2D1ImageSourceFromWic>         m_imageSource;
        Microsoft::WRL::ComPtr<ID2D1TransformedImageSource>     m_scaledImage;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_colorManagementEffect;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_toneMapEffect;

        // Other renderer members.
        bool                                                    m_useToneMapping;
        Windows::Foundation::Size                               m_imageSize;
        unsigned int                                            m_numberOfProfiles;
        float                                                   m_zoom;
        D2D1_POINT_2F                                           m_offset;
    };
}