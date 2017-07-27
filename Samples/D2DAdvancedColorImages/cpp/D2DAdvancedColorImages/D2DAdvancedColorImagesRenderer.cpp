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

#include "pch.h"
#include "D2DAdvancedColorImagesRenderer.h"
#include "DirectXPage.xaml.h"
#include "DirectXHelper.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;

D2DAdvancedColorImages::D2DAdvancedColorImages(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    m_imageSize(),
    m_useToneMapping(true),
    m_numberOfProfiles(0),
    m_zoom(1.0f),
    m_offset()
{
    // Register to be notified if the GPU device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

D2DAdvancedColorImages::~D2DAdvancedColorImages()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

void D2DAdvancedColorImages::CreateDeviceIndependentResources()
{
    // Register the custom effect.
    DX::ThrowIfFailed(
        ToneMapEffect::Register(m_deviceResources->GetD2DFactory())
        );
}

void D2DAdvancedColorImages::CreateDeviceDependentResources()
{
    // All this app's device-dependent resources also depend on
    // the loaded image, so they are all created in
    // CreateImageDependentResources.
}

void D2DAdvancedColorImages::ReleaseDeviceDependentResources()
{
}

// Whenever the app window is resized or changes displays, this method is used
// to update the app's sizing and advanced color state.
void D2DAdvancedColorImages::CreateWindowSizeDependentResources()
{
    UpdateAdvancedColorState();

    // In case the window size changed, update the app's image scaling values.
    FitImageToWindow();
}

// Reads the provided data stream and decodes a JPEG XR image from it. Retrieves the
// ICC color profile attached to that image, if any. These resources are device-
// independent.
void D2DAdvancedColorImages::LoadImage(IStream* imageStream)
{
    auto wicFactory = m_deviceResources->GetWicImagingFactory();

    // First, decode the provided image.

    ComPtr<IWICBitmapDecoder> decoder;
    DX::ThrowIfFailed(
        wicFactory->CreateDecoderFromStream(
            imageStream,
            nullptr,
            WICDecodeMetadataCacheOnDemand,
            &decoder
            )
        );

    ComPtr<IWICBitmapFrameDecode> frame;
    DX::ThrowIfFailed(
        decoder->GetFrame(0, &frame)
        );

    // Check that the image data is in a floating-point format. This app
    // only supports JPEG XR images containing scRGB content, expressed
    // in an extended-range, floating-point format.

    WICPixelFormatGUID pixelFormat;
    DX::ThrowIfFailed(
        frame->GetPixelFormat(&pixelFormat)
        );

    ComPtr<IWICComponentInfo> componentInfo;
    DX::ThrowIfFailed(
        wicFactory->CreateComponentInfo(
            pixelFormat,
            &componentInfo
            )
        );

    ComPtr<IWICPixelFormatInfo2> pixelFormatInfo;
    DX::ThrowIfFailed(
        componentInfo.As(&pixelFormatInfo)
        );

    WICPixelFormatNumericRepresentation formatNumber;
    DX::ThrowIfFailed(
        pixelFormatInfo->GetNumericRepresentation(&formatNumber)
        );

    if (formatNumber != WICPixelFormatNumericRepresentationFloat)
    {
        // Terminate image loading if the image is not in a floating-point format.
        return;
    }

    // Convert the image to a pixel format supported by Direct2D. Note that WIC
    // performs an implicit gamma conversion when converting between a fixed-point/
    // integer pixel format (sRGB gamma) and a float-point pixel format (linear gamma).
    // In this case, no gamma adjustment is performed since the source content
    // is already in a floating-point format. Gamma adjustment, if specified by
    // the ICC profile, will be performed by the color management effect.

    DX::ThrowIfFailed(
        wicFactory->CreateFormatConverter(&m_formatConvert)
        );

    DX::ThrowIfFailed(
        m_formatConvert->Initialize(
            frame.Get(),
            GUID_WICPixelFormat64bppPRGBAHalf, // Equivalent to DXGI_FORMAT_R16G16B16A16_FLOAT.
            WICBitmapDitherTypeNone,
            nullptr,
            0.0f,
            WICBitmapPaletteTypeCustom
            )
        );

    UINT width;
    UINT height;
    DX::ThrowIfFailed(
        m_formatConvert->GetSize(&width, &height)
        );
    m_imageSize = Size(static_cast<float>(width), static_cast<float>(height));

    // Attempt to read the embedded color profile from the image.
    DX::ThrowIfFailed(
        wicFactory->CreateColorContext(&m_wicColorContext)
        );
    DX::ThrowIfFailed(
        frame->GetColorContexts(
            1,
            m_wicColorContext.GetAddressOf(),
            &m_numberOfProfiles
            )
        );
}

// Configures a Direct2D image source, a color management effect, and a
// tone mapping effect, based on the loaded image. 
void D2DAdvancedColorImages::CreateImageDependentResources()
{
    auto d2dFactory = m_deviceResources->GetD2DFactory();
    auto context = m_deviceResources->GetD2DDeviceContext();

    // Load the image from WIC using ID2D1ImageSource.
    DX::ThrowIfFailed(
        m_deviceResources->GetD2DDeviceContext()->CreateImageSourceFromWic(
            m_formatConvert.Get(),
            &m_imageSource
            )
        );

    // Next, configure the app's effect pipeline, consisting of a color management effect
    // followed by a tone mapping effect.

    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_D2D1ColorManagement, &m_colorManagementEffect)
        );

    DX::ThrowIfFailed(
        m_colorManagementEffect->SetValue(
            D2D1_COLORMANAGEMENT_PROP_QUALITY,
            D2D1_COLORMANAGEMENT_QUALITY_BEST   // Required for floating point and DXGI color space support.
            )
        );

    // The color management effect takes a source color space and a destination color space,
    // and performs the appropriate math to convert images between them. Color contexts can be
    // "hard-coded" (by providing a set of color primaries and a white point), but in this case
    // we will use the color context attached to the image. This color context, if it exists,
    // was retrieved by WIC in the LoadImage method. If it doesn't exist, default to scRGB.

    ComPtr<ID2D1ColorContext> sourceColorContext;
    if (m_numberOfProfiles >= 1)
    {
        DX::ThrowIfFailed(
            context->CreateColorContextFromWicColorContext(
                m_wicColorContext.Get(),
                &sourceColorContext
                )
            );
    }
    else
    {
        DX::ThrowIfFailed(
            context->CreateColorContext(
                D2D1_COLOR_SPACE_SCRGB,
                nullptr,
                0,
                &sourceColorContext
                )
            );
    }

    DX::ThrowIfFailed(
        m_colorManagementEffect->SetValue(
            D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT,
            sourceColorContext.Get()
            )
        );

    // For the destination color space, we will use scRGB. This is appropriate for this app's
    // choice of swap chain pixel format.
    ComPtr<ID2D1ColorContext1> destColorContext;
    DX::ThrowIfFailed(
        context->CreateColorContextFromDxgiColorSpace(
            DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709,
            &destColorContext
            )
        );

    DX::ThrowIfFailed(
        m_colorManagementEffect->SetValue(
            D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT,
            destColorContext.Get()
            )
        );

    // Instantiate and configure the tone mapping effect, which is a custom Direct2D effect
    // implemented in ToneMapEffect.cpp and ToneMapEffect.hlsl. It performs a very basic
    // HDR-to-SDR tone mapping operation.
    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_CustomToneMapEffect, &m_toneMapEffect)
        );

    m_toneMapEffect->SetInputEffect(0, m_colorManagementEffect.Get());
}

void D2DAdvancedColorImages::ReleaseImageDependentResources()
{
    m_imageSource.Reset();
    m_scaledImage.Reset();
    m_colorManagementEffect.Reset();
    m_toneMapEffect.Reset();
}

// Update zoom state to keep the image fit to the app window.
void D2DAdvancedColorImages::FitImageToWindow()
{
    if (m_imageSource)
    {
        Size panelSize = m_deviceResources->GetLogicalSize();

        // Letterbox but never exceed max scale.
        m_zoom = min(
            min(
                panelSize.Width / m_imageSize.Width,
                panelSize.Height / m_imageSize.Height
                ),
            1.0f
            );

        // Center the image.
        m_offset = D2D1::Point2F(
            (panelSize.Width - (m_imageSize.Width * m_zoom)) / 2.0f,
            (panelSize.Height - (m_imageSize.Height * m_zoom)) / 2.0f
            );

        // When using ID2D1ImageSource, the recommend method of scaling is to use
        // ID2D1TransformedImageSource. It is inexpensive to recreate this object.
        D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES props =
        {
            D2D1_ORIENTATION_DEFAULT,
            m_zoom,
            m_zoom,
            D2D1_INTERPOLATION_MODE_LINEAR, // This is ignored when using DrawImage.
            D2D1_TRANSFORMED_IMAGE_SOURCE_OPTIONS_NONE
        };

        DX::ThrowIfFailed(
            m_deviceResources->GetD2DDeviceContext()->CreateTransformedImageSource(
                m_imageSource.Get(),
                &props,
                &m_scaledImage
                )
            );

        // Set the new image as the new source to the effect pipeline.
        m_colorManagementEffect->SetInput(0, m_scaledImage.Get());
    }
}

// Uses DXGI APIs to query the capabilities of the app's current display, and
// enables HDR-to-SDR tone mapping if the display does not meet a particular
// brightness threshold or is not in advanced color mode.
void D2DAdvancedColorImages::UpdateAdvancedColorState()
{
    DXGI_OUTPUT_DESC1 outputDesc = m_deviceResources->GetCurrentOutputDesc1();

    // Decide whether or not the app needs to apply tone mapping, based on the brightness
    // capabilities of the display (maximum luminance at least 300) and the mode of the display.
    if ((outputDesc.MaxLuminance >= 300.0f) && (outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020))
    {
        m_useToneMapping = false;
    }
    else
    {
        m_useToneMapping = true;
    }
}

// Renders the loaded JPEG XR image, optionally applying HDR-to-SDR tone mapping,
// if appropriate.
void D2DAdvancedColorImages::Draw()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->BeginDraw();

    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    d2dContext->SetTransform(m_deviceResources->GetOrientationTransform2D());

    if (m_scaledImage)
    {
        if (m_useToneMapping)
        {
            // Draw the full effect pipeline, which includes the tone mapping effect.
            d2dContext->DrawImage(m_toneMapEffect.Get(), m_offset);
        }
        else
        {
            // Omit the tone mapping effect and just draw the color-managed image.
            d2dContext->DrawImage(m_colorManagementEffect.Get(), m_offset);
        }
    }

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_deviceResources->Present();
}

// Notifies renderers that device resources need to be released.
void D2DAdvancedColorImages::OnDeviceLost()
{
    ReleaseImageDependentResources();
    ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void D2DAdvancedColorImages::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateImageDependentResources();
    CreateWindowSizeDependentResources();

    Draw();
}
