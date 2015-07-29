//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include <math.h>

namespace DX
{
    // Convert DirectX error codes to exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DX API errors.
            throw Platform::Exception::CreateException(hr);
        }
    }

    // Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
    inline float ConvertDipsToPixels(float dips, float dpi)
    {
        static const float dipsPerInch = 96.0f;
        return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
    }

    // Converts a length in physical pixels to a length in device-independent pixels (DIPs).
    inline float ConvertPixelsToDips(float pixels)
    {
        static const float dipsPerInch = 96.0f;
        return pixels * dipsPerInch / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi; // Do not round.
    }

    // Converts a length in physical pixels to a length in device-independent pixels (DIPs) using the provided DPI. This removes the need
    // to call this function from a thread associated with a CoreWindow, which is required by DisplayInformation::GetForCurrentView().
    inline float ConvertPixelsToDips(float pixels, float dpi)
    {
        static const float dipsPerInch = 96.0f;
        return pixels * dipsPerInch / dpi; // Do not round.
    }

#if defined(_DEBUG)
    // Check for SDK Layer support.
    inline bool SdkLayersAvailable()
    {
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
            0,
            D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
            nullptr,                    // Any feature level will do.
            0,
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            nullptr,                    // No need to keep the D3D device reference.
            nullptr,                    // No need to know the feature level.
            nullptr                     // No need to keep the D3D device context reference.
            );

        return SUCCEEDED(hr);
    }
#endif
}
