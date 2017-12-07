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
#include "ReinhardEffect.h"
#include "FilmicEffect.h"
#include "RenderOptions.h"

namespace D2DAdvancedColorImages
{
    public enum class DisplayACKind
    {
        NotAdvancedColor,               // No AC capabilities: dynamic range, color gamut, bit depth
        AdvancedColor_LowDynamicRange,  // Wide color gamut, high bit depth, but low dynamic range
        AdvancedColor_HighDynamicRange  // Full high dynamic range, wide color gamut, high bit depth
    };

    struct ImageInfo
    {
        unsigned int                bitsPerPixel;
        bool                        isFloat;
        Windows::Foundation::Size   size;
        unsigned int                numProfiles;
    };

    // Provides an interface for an application to be notified of changes in the display's
    // advanced color state.
    public interface class IDisplayACStateChanged
    {
        virtual void OnDisplayACStateChanged(
            float maxLuminance,
            unsigned int bitDepth,
            DisplayACKind displayKind) = 0;
    };

    class D2DAdvancedColorImagesRenderer : public DX::IDeviceNotify
    {
    public:
        D2DAdvancedColorImagesRenderer(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            IDisplayACStateChanged^ handler
            );

        ~D2DAdvancedColorImagesRenderer();

        void CreateDeviceIndependentResources();
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void Draw();

        void CreateImageDependentResources();
        void ReleaseImageDependentResources();

        void FitImageToWindow();
        void SetRenderOptions(
            bool enableScaling,
            TonemapperKind tonemapper,
            float whiteLevelScale,
            DXGI_COLOR_SPACE_TYPE colorspace
            );

        ImageInfo LoadImage(IStream* imageStream);

        // IDeviceNotify methods handle device lost and restored.
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        void UpdateAdvancedColorState();
        void UpdateImageColorContext();
        void UpdateWhiteLevelScale();
        DisplayACKind GetDisplayACKind();

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                    m_deviceResources;

        // WIC and Direct2D resources.
        Microsoft::WRL::ComPtr<IWICFormatConverter>             m_formatConvert;
        Microsoft::WRL::ComPtr<IWICColorContext>                m_wicColorContext;
        Microsoft::WRL::ComPtr<ID2D1ImageSourceFromWic>         m_imageSource;
        Microsoft::WRL::ComPtr<ID2D1TransformedImageSource>     m_scaledImage;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_colorManagementEffect;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_whiteScaleEffect;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_reinhardEffect;
        Microsoft::WRL::ComPtr<ID2D1Effect>                     m_filmicEffect;

        // Other renderer members.
        TonemapperKind                                          m_tonemapperKind;
        bool                                                    m_userDisabledScaling;
        bool                                                    m_useTonemapping;
        float                                                   m_zoom;
        D2D1_POINT_2F                                           m_offset;
        DXGI_COLOR_SPACE_TYPE                                   m_imageColorSpace;
        float                                                   m_whiteLevelScale;

        DXGI_OUTPUT_DESC1                                       m_outputDesc;
        ImageInfo                                               m_imageInfo;

        // Registered handler for the display's advanced color state changes.
        IDisplayACStateChanged^                                 m_dispStateChangeHandler;
    };
}