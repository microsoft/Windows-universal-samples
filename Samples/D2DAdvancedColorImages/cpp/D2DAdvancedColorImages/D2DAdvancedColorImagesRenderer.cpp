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

using namespace D2DAdvancedColorImages;

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

D2DAdvancedColorImagesRenderer::D2DAdvancedColorImagesRenderer(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    IDisplayACStateChanged^ handler
    ) :
    m_deviceResources(deviceResources),
    m_tonemapperKind(TonemapperKind::Disabled),
    m_userDisabledScaling(false),
    m_useTonemapping(true),
    m_imageInfo{},
    m_outputDesc{},
    m_zoom(1.0f),
    m_offset(),
    m_imageColorSpace(DXGI_COLOR_SPACE_CUSTOM),
    m_whiteLevelScale(1.0f),
    m_dispStateChangeHandler(handler)
{
    // Register to be notified if the GPU device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

D2DAdvancedColorImagesRenderer::~D2DAdvancedColorImagesRenderer()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

void D2DAdvancedColorImagesRenderer::CreateDeviceIndependentResources()
{
    // Register the custom tonemapper effects.
    DX::ThrowIfFailed(
        ReinhardEffect::Register(m_deviceResources->GetD2DFactory())
        );

    DX::ThrowIfFailed(
        FilmicEffect::Register(m_deviceResources->GetD2DFactory())
        );
}

void D2DAdvancedColorImagesRenderer::CreateDeviceDependentResources()
{
    // All this app's device-dependent resources also depend on
    // the loaded image, so they are all created in
    // CreateImageDependentResources.
}

void D2DAdvancedColorImagesRenderer::ReleaseDeviceDependentResources()
{
}

// Whenever the app window is resized or changes displays, this method is used
// to update the app's sizing and advanced color state.
void D2DAdvancedColorImagesRenderer::CreateWindowSizeDependentResources()
{
    FitImageToWindow();
    UpdateAdvancedColorState();
}

// White level scale is used to multiply the color values in the image; allows the user to
// adjust the brightness of the image on an HDR display.
void D2DAdvancedColorImagesRenderer::SetRenderOptions(
    bool disableScaling,
    TonemapperKind tonemapper,
    float whiteLevelScale,
    DXGI_COLOR_SPACE_TYPE colorspace
    )
{
    m_userDisabledScaling = disableScaling;
    FitImageToWindow();

    m_tonemapperKind = tonemapper;
    UpdateAdvancedColorState();

    m_imageColorSpace = colorspace;
    UpdateImageColorContext();

    // Image colorspace can affect white level scale, so call this last.
    m_whiteLevelScale = whiteLevelScale;
    UpdateWhiteLevelScale();

    Draw();
}

// Reads the provided data stream and decodes an image from it. Retrieves the
// ICC color profile attached to that image, if any. These resources are device-
// independent.
ImageInfo D2DAdvancedColorImagesRenderer::LoadImage(_In_ IStream* imageStream)
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

    // Check whether the image data is natively stored in a floating-point format, and
    // decode to the appropriate WIC pixel format.

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

    DX::ThrowIfFailed(pixelFormatInfo->GetBitsPerPixel(&m_imageInfo.bitsPerPixel));
    m_imageInfo.isFloat = (WICPixelFormatNumericRepresentationFloat == formatNumber) ? true : false;

    // When decoding, preserve the numeric representation (float vs. non-float)
    // of the native image data. This avoids WIC performing an implicit gamma conversion
    // which occurs when converting between a fixed-point/integer pixel format (sRGB gamma)
    // and a float-point pixel format (linear gamma). Gamma adjustment, if specified by
    // the ICC profile, will be performed by the Direct2D color management effect.

    WICPixelFormatGUID fmt = {};
    if (m_imageInfo.isFloat)
    {
        fmt = GUID_WICPixelFormat64bppPRGBAHalf; // Equivalent to DXGI_FORMAT_R16G16B16A16_FLOAT.
    }
    else
    {
        fmt = GUID_WICPixelFormat64bppPRGBA; // Equivalent to DXGI_FORMAT_R16G16B16A16_UNORM.
                                             // Many SDR images (e.g. JPEG) use <=32bpp, so it
                                             // is possible to further optimize this for memory usage.
    }

    DX::ThrowIfFailed(
        wicFactory->CreateFormatConverter(&m_formatConvert)
        );

    DX::ThrowIfFailed(
        m_formatConvert->Initialize(
            frame.Get(),
            fmt,
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

    m_imageInfo.size = Size(static_cast<float>(width), static_cast<float>(height));

    // Attempt to read the embedded color profile from the image.
    DX::ThrowIfFailed(
        wicFactory->CreateColorContext(&m_wicColorContext)
        );

    DX::ThrowIfFailed(
        frame->GetColorContexts(
            1,
            m_wicColorContext.GetAddressOf(),
            &m_imageInfo.numProfiles
            )
        );

    return m_imageInfo;
}

// Configures a Direct2D image pipeline, including source, color management, 
// tonemapping, and white level, based on the loaded image.
void D2DAdvancedColorImagesRenderer::CreateImageDependentResources()
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
    // and performs the appropriate math to convert images between them.
    UpdateImageColorContext();

    // The destination color space is the render target's (swap chain's) color space. This app uses an
    // FP16 swap chain, which requires the colorspace to be scRGB.
    ComPtr<ID2D1ColorContext1> destColorContext;
    DX::ThrowIfFailed(
        context->CreateColorContextFromDxgiColorSpace(
            DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709, // scRGB
            &destColorContext
            )
        );

    DX::ThrowIfFailed(
        m_colorManagementEffect->SetValue(
            D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT,
            destColorContext.Get()
            )
        );

    // White level scale is used to multiply the color values in the image; this allows the user
    // to adjust the brightness of the image on an HDR display.
    DX::ThrowIfFailed(context->CreateEffect(CLSID_D2D1ColorMatrix, &m_whiteScaleEffect));

    m_whiteScaleEffect->SetInputEffect(0, m_colorManagementEffect.Get());

    // Set the actual matrix in SetRenderOptions.

    // Instantiate and cache all of the tonemapping effects. Selection is performed in Draw().
    // Each tonemapper is implemented as a Direct2D custom effect; see the Tonemappers filter in the
    // Solution Explorer.
    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_CustomReinhardEffect, &m_reinhardEffect)
        );

    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_CustomFilmicEffect, &m_filmicEffect)
        );

    m_reinhardEffect->SetInputEffect(0, m_whiteScaleEffect.Get());
    m_filmicEffect->SetInputEffect(0, m_whiteScaleEffect.Get());
}

void D2DAdvancedColorImagesRenderer::ReleaseImageDependentResources()
{
    m_imageSource.Reset();
    m_scaledImage.Reset();
    m_colorManagementEffect.Reset();
    m_whiteScaleEffect.Reset();
    m_reinhardEffect.Reset();
    m_filmicEffect.Reset();
}

// Update zoom state to keep the image fit to the app window.
void D2DAdvancedColorImagesRenderer::FitImageToWindow()
{
    if (m_imageSource)
    {
        Size panelSize = m_deviceResources->GetLogicalSize();

        // Letterbox but never exceed max scale.
        m_zoom = min(
            min(
                panelSize.Width / m_imageInfo.size.Width,
                panelSize.Height / m_imageInfo.size.Height
                ),
            1.0f
            );

        // User can disable spatial scaling.
        m_zoom = m_userDisabledScaling ? 1.0f : m_zoom;

        if (m_userDisabledScaling)
        {
            m_offset = {};
        }
        else
        {
            // Center the image.
            m_offset = D2D1::Point2F(
                (panelSize.Width - (m_imageInfo.size.Width * m_zoom)) / 2.0f,
                (panelSize.Height - (m_imageInfo.size.Height * m_zoom)) / 2.0f
                );
        }

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
void D2DAdvancedColorImagesRenderer::UpdateAdvancedColorState()
{
    m_outputDesc = m_deviceResources->GetCurrentOutputDesc1();

    // Tonemapping is only needed on low dynamic range displays; the user can also force it off.
    if (m_tonemapperKind == TonemapperKind::Disabled ||
        GetDisplayACKind() == DisplayACKind::AdvancedColor_HighDynamicRange)
    {
        m_useTonemapping = false;
    }
    else
    {
        m_useTonemapping = true;
    }

    if (m_dispStateChangeHandler)
    {
        m_dispStateChangeHandler->OnDisplayACStateChanged(
            m_outputDesc.MaxLuminance,
            m_outputDesc.BitsPerColor,
            GetDisplayACKind()
            );
    }
}

// Based on the user's choice, either derive the source color context from the image
// (embedded ICC profile or metadata), or create a context using the user's specified colorspace.
void D2DAdvancedColorImagesRenderer::UpdateImageColorContext()
{
    auto context = m_deviceResources->GetD2DDeviceContext();

    ComPtr<ID2D1ColorContext> sourceColorContext;

    if (m_imageColorSpace == DXGI_COLOR_SPACE_CUSTOM)
    {
        // Derive the color context from the image.
        if (m_imageInfo.numProfiles >= 1)
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
            // Since no embedded color profile/metadata exists, select a default
            // based on the pixel format: floating point == scRGB, others == sRGB.
            DX::ThrowIfFailed(
                context->CreateColorContext(
                    m_imageInfo.isFloat ? D2D1_COLOR_SPACE_SCRGB : D2D1_COLOR_SPACE_SRGB,
                    nullptr,
                    0,
                    &sourceColorContext
                    )
                );
        }
    }
    else
    {
        // Use the user's explicitly defined colorspace.
        ComPtr<ID2D1ColorContext1> colorContext1;
        DX::ThrowIfFailed(
            context->CreateColorContextFromDxgiColorSpace(m_imageColorSpace, &colorContext1)
            );

        DX::ThrowIfFailed(colorContext1.As(&sourceColorContext));
    }

    DX::ThrowIfFailed(
        m_colorManagementEffect->SetValue(
            D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT,
            sourceColorContext.Get()
        )
    );
}

void D2DAdvancedColorImagesRenderer::UpdateWhiteLevelScale()
{
    // If we are viewing an HDR10-encoded image and are using an HDR display,
    // we need to map the [0, 1] range of the image pixels to [0, 10000] nits
    // of the SMPTE.2084 EOTF. We render in scRGB, which defines 1.0 reference white
    // as 80 nits; therefore, we must scale up by 125x (10000 / 80).
    switch (m_imageColorSpace)
    {
    case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
    case DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020:
        if (GetDisplayACKind() == DisplayACKind::AdvancedColor_HighDynamicRange)
        {
            m_whiteLevelScale = 125.0f;
        }
        break;
    }
    
    D2D1_MATRIX_5X4_F matrix = D2D1::Matrix5x4F(
        m_whiteLevelScale, 0, 0, 0,  // [R] Multiply each color channel
        0, m_whiteLevelScale, 0, 0,  // [G] by the scale factor in 
        0, 0, m_whiteLevelScale, 0,  // [B] linear gamma space.
        0, 0, 0                , 1,  // [A] Preserve alpha values.
        0, 0, 0                , 0); //     No offset.

    DX::ThrowIfFailed(m_whiteScaleEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, matrix));
}

// Determines what class of advanced color capabilities the current display has.
DisplayACKind D2DAdvancedColorImagesRenderer::GetDisplayACKind()
{
    switch (m_outputDesc.ColorSpace)
    {
    case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709:
        return DisplayACKind::NotAdvancedColor;
        break;

    case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
        // 300 nits is considered the cutoff between an HDR and LDR display.
        // HDR displays support overbright, i.e. colors brighter than reference white.
        // LDR displays are limited to the [0, 1] luminance range.
        if (m_outputDesc.MaxLuminance < 300.0f)
        {
            return DisplayACKind::AdvancedColor_LowDynamicRange;
        }
        else
        {
            return DisplayACKind::AdvancedColor_HighDynamicRange;
        }
        break;

    default:
        // DXGI_OUTPUT_DESC1 should only contain one of the above values.
        return DisplayACKind::NotAdvancedColor;
        break;
    }
}

// Renders the loaded image with user-specified options.
void D2DAdvancedColorImagesRenderer::Draw()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->BeginDraw();

    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    d2dContext->SetTransform(m_deviceResources->GetOrientationTransform2D());

    if (m_scaledImage)
    {
        if (m_useTonemapping)
        {
            // Draw the full effect pipeline including the HDR tonemapper.
            ComPtr<ID2D1Effect> tonemapper;
            switch (m_tonemapperKind)
            {
            case TonemapperKind::Reinhard:
                tonemapper = m_reinhardEffect.Get();
                break;

            case TonemapperKind::Filmic:
            default:
                tonemapper = m_filmicEffect.Get();
                break;
            }

            d2dContext->DrawImage(tonemapper.Get(), m_offset);
        }
        else
        {
            // Omit the tone mapping effect and just draw the color-managed, scaled image.
            d2dContext->DrawImage(m_whiteScaleEffect.Get(), m_offset);
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
void D2DAdvancedColorImagesRenderer::OnDeviceLost()
{
    ReleaseImageDependentResources();
    ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void D2DAdvancedColorImagesRenderer::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateImageDependentResources();
    CreateWindowSizeDependentResources();

    Draw();
}
