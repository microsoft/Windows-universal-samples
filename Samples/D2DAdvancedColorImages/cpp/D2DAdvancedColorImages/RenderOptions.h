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

#include "pch.h"

namespace D2DAdvancedColorImages
{
    /// <summary>
    /// Supported HDR to SDR tonemappers. Each tonemapper is implemented as a custom Direct2D effect.
    /// </summary>
    public enum class TonemapperKind
    {
        Reinhard,
        Filmic,
        Disabled,
    };

    /// <summary>
    /// Associates a tonemapping type used by the renderer to a descriptive string bound to UI.
    /// </summary>
    public ref class TonemapperOption sealed
    {
    public:
        TonemapperOption(Platform::String^ description, TonemapperKind type)
        {
            this->description = description;
            this->type = type;
        }

        property Platform::String^ Description
        {
            Platform::String^ get() { return description; }
        }

        property TonemapperKind Tonemapper
        {
            TonemapperKind get() { return type; }
        }

    private:
        Platform::String^       description;
        TonemapperKind          type;
    };

    /// <summary>
    /// Maps DXGI_COLOR_SPACE_TYPE to a valid WinRT enumeration. Every enum value
    /// must be defined as an existing DXGI_COLOR_SPACE_TYPE value to allow for casting.
    /// Only the DXGI_COLOR_SPACE_TYPE values needed for this app are defined.
    /// </summary>
    public enum class DxgiColorspace
    {
        Custom = DXGI_COLOR_SPACE_CUSTOM,
        Srgb_Full = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709,
        Srgb_Studio = DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709,
        Scrgb_Full = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709,
        Hdr10_Full = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020,
        Hdr10_Studio = DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020,
    };

    /// <summary>
    /// Associates a DXGI colorspace used by the renderer to a descriptive string bound to UI.
    /// </summary>
    public ref class ColorspaceOption sealed
    {
    public:
        ColorspaceOption(Platform::String^ description, DxgiColorspace cs)
        {
            this->description = description;
            this->colorspace = cs;
        }

        property Platform::String^ Description
        {
            Platform::String^ get() { return description; }
        }

        property DxgiColorspace Colorspace
        {
            DxgiColorspace get() { return colorspace; }
        }

    private:
        Platform::String^       description;
        DxgiColorspace          colorspace;
    };

    /// <summary>
    /// Allows databinding of render options to the UI: image colorspace and tonemapper.
    /// </summary>
    public ref class RenderOptionsViewModel sealed
    {
    public:
        RenderOptionsViewModel()
        {
            // The first index is chosen by default. Ensure this is in sync with DirectXPage.
            colorspaces = ref new Platform::Collections::VectorView<ColorspaceOption^>
            {
                ref new ColorspaceOption(L"Use image's color profile", DxgiColorspace::Custom),
                ref new ColorspaceOption(L"sRGB (full range)", DxgiColorspace::Srgb_Full),
                ref new ColorspaceOption(L"sRGB (studio range)", DxgiColorspace::Srgb_Studio),
                ref new ColorspaceOption(L"scRGB (full range)", DxgiColorspace::Scrgb_Full),
                ref new ColorspaceOption(L"HDR10 (full range)", DxgiColorspace::Hdr10_Full),
                ref new ColorspaceOption(L"HDR10 (studio range)", DxgiColorspace::Hdr10_Studio),
            };

            // The first index is chosen by default. Ensure this is in sync with DirectXPage.
            tonemappers = ref new Platform::Collections::VectorView<TonemapperOption^>
            {
                ref new TonemapperOption(L"ACES Filmic", TonemapperKind::Filmic),
                ref new TonemapperOption(L"Reinhard", TonemapperKind::Reinhard),
                ref new TonemapperOption(L"Disabled", TonemapperKind::Disabled),
            };
        }

        property Windows::Foundation::Collections::IVectorView<ColorspaceOption^>^ Colorspaces
        {
            Windows::Foundation::Collections::IVectorView<ColorspaceOption^>^ get()
            {
                return this->colorspaces;
            }
        }

        property Windows::Foundation::Collections::IVectorView<TonemapperOption^>^ Tonemappers
        {
            Windows::Foundation::Collections::IVectorView<TonemapperOption^>^ get()
            {
                return this->tonemappers;
            }
        }

    private:
        Platform::Collections::VectorView<ColorspaceOption^>^ colorspaces;
        Platform::Collections::VectorView<TonemapperOption^>^ tonemappers;
    };
}