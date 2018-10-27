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

static const float sc_MaxZoom = 1.0f; // Restrict max zoom to 1:1 scale.
static const unsigned int sc_MaxBytesPerPixel = 16; // Covers all supported image formats.
static const float sc_nominalRefWhite = 80.0f; // Nominal white nits for sRGB and scRGB.

// 400 bins with gamma of 10 lets us measure luminance to within 10% error for any
// luminance above ~1.5 nits, up to 1 million nits.
static const unsigned int sc_histNumBins = 400;
static const float        sc_histGamma = 0.1f;
static const unsigned int sc_histMaxNits = 1000000;

D2DAdvancedColorImagesRenderer::D2DAdvancedColorImagesRenderer(
    const std::shared_ptr<DX::DeviceResources>& deviceResources
    ) :
    m_deviceResources(deviceResources),
    m_renderEffectKind(RenderEffectKind::None),
    m_zoom(1.0f),
    m_minZoom(1.0f), // Dynamically calculated on window size.
    m_imageOffset(),
    m_pointerPos(),
    m_maxCLL(-1.0f),
    m_brightnessAdjust(1.0f),
    m_imageInfo{},
    m_isComputeSupported(false)
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
    // Register the custom render effects.
    DX::ThrowIfFailed(
        ReinhardEffect::Register(m_deviceResources->GetD2DFactory())
        );

    DX::ThrowIfFailed(
        FilmicEffect::Register(m_deviceResources->GetD2DFactory())
        );

    DX::ThrowIfFailed(
        SdrOverlayEffect::Register(m_deviceResources->GetD2DFactory())
        );

    DX::ThrowIfFailed(
        LuminanceHeatmapEffect::Register(m_deviceResources->GetD2DFactory())
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
}

// White level scale is used to multiply the color values in the image; allows the user to
// adjust the brightness of the image on an HDR display.
void D2DAdvancedColorImagesRenderer::SetRenderOptions(
    RenderEffectKind effect,
    float brightnessAdjustment,
    AdvancedColorInfo^ acInfo
    )
{
    m_dispInfo = acInfo;
    m_renderEffectKind = effect;
    m_brightnessAdjust = brightnessAdjustment;

    auto sdrWhite = m_dispInfo ? m_dispInfo->SdrWhiteLevelInNits : sc_nominalRefWhite;

    UpdateWhiteLevelScale(m_brightnessAdjust, sdrWhite);

    // Adjust the Direct2D effect graph based on RenderEffectKind.
    // Some RenderEffectKind values require us to apply brightness adjustment
    // after the effect as their numerical output is affected by any luminance boost.
    switch (m_renderEffectKind)
    {
    // Effect graph: ImageSource > ColorManagement > WhiteScale > Reinhard
    case RenderEffectKind::ReinhardTonemap:
        m_finalOutput = m_reinhardEffect.Get();
        m_whiteScaleEffect->SetInputEffect(0, m_colorManagementEffect.Get());
        break;

    // Effect graph: ImageSource > ColorManagement > WhiteScale > FilmicTonemap
    case RenderEffectKind::FilmicTonemap:
        m_finalOutput = m_filmicEffect.Get();
        m_whiteScaleEffect->SetInputEffect(0, m_colorManagementEffect.Get());
        break;

    // Effect graph: ImageSource > ColorManagement > WhiteScale
    case RenderEffectKind::None:
        m_finalOutput = m_whiteScaleEffect.Get();
        m_whiteScaleEffect->SetInputEffect(0, m_colorManagementEffect.Get());
        break;

    // Effect graph: ImageSource > ColorManagement > Heatmap > WhiteScale
    case RenderEffectKind::LuminanceHeatmap:
        m_finalOutput = m_whiteScaleEffect.Get();
        m_whiteScaleEffect->SetInputEffect(0, m_heatmapEffect.Get());
        break;

    // Effect graph: ImageSource > ColorManagement > SdrOverlay > WhiteScale
    case RenderEffectKind::SdrOverlay:
        m_finalOutput = m_whiteScaleEffect.Get();
        m_whiteScaleEffect->SetInputEffect(0, m_sdrOverlayEffect.Get());
        break;

    default:
        throw ref new NotImplementedException();
        break;
    }

    Draw();
}

// Reads the provided data stream and decodes an image from it using WIC. These resources are device-
// independent.
ImageInfo D2DAdvancedColorImagesRenderer::LoadImageFromWic(_In_ IStream* imageStream)
{
    auto wicFactory = m_deviceResources->GetWicImagingFactory();

    // Decode the image using WIC.
    ComPtr<IWICBitmapDecoder> decoder;
    DX::ThrowIfFailed(
        wicFactory->CreateDecoderFromStream(
            imageStream,
            nullptr,
            WICDecodeMetadataCacheOnDemand,
            &decoder
            ));

    ComPtr<IWICBitmapFrameDecode> frame;
    DX::ThrowIfFailed(
        decoder->GetFrame(0, &frame)
        );

    return LoadImageCommon(frame.Get());
}

// Simplified heuristic to determine what advanced color kind the image is.
// Requires that all fields other than imageKind are populated.
void D2DAdvancedColorImagesRenderer::PopulateImageInfoACKind(_Inout_ ImageInfo* info)
{
    if (info->bitsPerPixel == 0 ||
        info->bitsPerChannel == 0 ||
        info->size.Width == 0 ||
        info->size.Height == 0)
    {
        DX::ThrowIfFailed(E_INVALIDARG);
    }

    info->imageKind = AdvancedColorKind::StandardDynamicRange;

    // Bit depth > 8bpc or color gamut > sRGB signifies a WCG image.
    // The presence of a color profile is used as an approximation for wide gamut.
    if (info->bitsPerChannel > 8 || info->numProfiles >= 1)
    {
        info->imageKind = AdvancedColorKind::WideColorGamut;
    }

    // This application currently only natively supports HDR images with floating point.
    // An image encoded using the HDR10 colorspace is also HDR, but this
    // is not automatically detected by the application.
    if (info->isFloat == true)
    {
        info->imageKind = AdvancedColorKind::HighDynamicRange;
    }
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

    // Input to white level scale may be modified in SetRenderOptions.
    m_whiteScaleEffect->SetInputEffect(0, m_colorManagementEffect.Get());

    // Set the actual matrix in SetRenderOptions.

    // Instantiate and cache all of the tonemapping/render effects.
    // Each effect is implemented as a Direct2D custom effect; see the RenderEffects filter in the
    // Solution Explorer.
    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_CustomReinhardEffect, &m_reinhardEffect)
        );

    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_CustomFilmicEffect, &m_filmicEffect)
        );

    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_CustomSdrOverlayEffect, &m_sdrOverlayEffect)
        );

    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_CustomLuminanceHeatmapEffect, &m_heatmapEffect)
        );

    m_reinhardEffect->SetInputEffect(0, m_whiteScaleEffect.Get());
    m_filmicEffect->SetInputEffect(0, m_whiteScaleEffect.Get());

    // For the following effects, we want white level scale to be applied after
    // tonemapping (otherwise brightness adjustments will affect numerical values).
    m_heatmapEffect->SetInputEffect(0, m_colorManagementEffect.Get());
    m_sdrOverlayEffect->SetInputEffect(0, m_colorManagementEffect.Get());

    // The remainder of the Direct2D effect graph is constructed in SetRenderOptions based on the
    // selected RenderEffectKind.

    CreateHistogramResources();
}

// Perform histogram pipeline setup; this should occur as part of image resource creation.
// Histogram results in no visual output but is used to calculate HDR metadata for the image.
void D2DAdvancedColorImagesRenderer::CreateHistogramResources()
{
    auto context = m_deviceResources->GetD2DDeviceContext();

    // We need to preprocess the image data before running the histogram.
    // 1. Spatial downscale to reduce the amount of processing needed.
    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_D2D1Scale, &m_histogramPrescale)
        );

    DX::ThrowIfFailed(
        m_histogramPrescale->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(0.5f, 0.5f))
        );

    // The right place to compute HDR metadata is after color management to the
    // image's native colorspace but before any tonemapping or adjustments for the display.
    m_histogramPrescale->SetInputEffect(0, m_colorManagementEffect.Get());

    // 2. Convert scRGB data into luminance (nits).
    // 3. Normalize color values. Histogram operates on [0-1] numeric range,
    //    while FP16 can go up to 65504 (5+ million nits).
    // Both steps are performed in the same color matrix.
    ComPtr<ID2D1Effect> histogramMatrix;
    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_D2D1ColorMatrix, &histogramMatrix)
        );

    histogramMatrix->SetInputEffect(0, m_histogramPrescale.Get());

    float scale = sc_histMaxNits / sc_nominalRefWhite;

    D2D1_MATRIX_5X4_F rgbtoYnorm = D2D1::Matrix5x4F(
        0.2126f / scale, 0, 0, 0,
        0.7152f / scale, 0, 0, 0,
        0.0722f / scale, 0, 0, 0,
        0              , 0, 0, 1,
        0              , 0, 0, 0);
    // 1st column: [R] output, contains normalized Y (CIEXYZ).
    // 2nd column: [G] output, unused.
    // 3rd column: [B] output, unused.
    // 4th column: [A] output, alpha passthrough.
    // We explicitly calculate Y; this deviates from the CEA 861.3 definition of MaxCLL
    // which approximates luminance with max(R, G, B).

    DX::ThrowIfFailed(histogramMatrix->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, rgbtoYnorm));

    // 4. Apply a gamma to allocate more histogram bins to lower luminance levels.
    ComPtr<ID2D1Effect> histogramGamma;
    DX::ThrowIfFailed(
        context->CreateEffect(CLSID_D2D1GammaTransfer, &histogramGamma)
        );

    histogramGamma->SetInputEffect(0, histogramMatrix.Get());

    // Gamma function offers an acceptable tradeoff between simplicity and efficient bin allocation.
    // A more sophisticated pipeline would use a more perceptually linear function than gamma.
    DX::ThrowIfFailed(histogramGamma->SetValue(D2D1_GAMMATRANSFER_PROP_RED_EXPONENT, sc_histGamma));
    // All other channels are passthrough.
    DX::ThrowIfFailed(histogramGamma->SetValue(D2D1_GAMMATRANSFER_PROP_GREEN_DISABLE, TRUE));
    DX::ThrowIfFailed(histogramGamma->SetValue(D2D1_GAMMATRANSFER_PROP_BLUE_DISABLE, TRUE));
    DX::ThrowIfFailed(histogramGamma->SetValue(D2D1_GAMMATRANSFER_PROP_ALPHA_DISABLE, TRUE));

    // 5. Finally, the histogram itself.
    HRESULT hr = context->CreateEffect(CLSID_D2D1Histogram, &m_histogramEffect);
    
    if (hr == D2DERR_INSUFFICIENT_DEVICE_CAPABILITIES)
    {
        // The GPU doesn't support compute shaders and we can't run histogram on it.
        m_isComputeSupported = false;
    }
    else
    {
        DX::ThrowIfFailed(hr);
        m_isComputeSupported = true;

        DX::ThrowIfFailed(m_histogramEffect->SetValue(D2D1_HISTOGRAM_PROP_NUM_BINS, sc_histNumBins));

        m_histogramEffect->SetInputEffect(0, histogramGamma.Get());
    }
}

void D2DAdvancedColorImagesRenderer::ReleaseImageDependentResources()
{
    m_imageSource.Reset();
    m_scaledImage.Reset();
    m_colorManagementEffect.Reset();
    m_whiteScaleEffect.Reset();
    m_reinhardEffect.Reset();
    m_filmicEffect.Reset();
    m_sdrOverlayEffect.Reset();
    m_heatmapEffect.Reset();
    m_histogramPrescale.Reset();
    m_histogramEffect.Reset();
    m_finalOutput.Reset();
}

void D2DAdvancedColorImagesRenderer::UpdateManipulationState(_In_ ManipulationUpdatedEventArgs^ args)
{
    Point position = args->Position;
    Point positionDelta = args->Delta.Translation;
    float zoomDelta = args->Delta.Scale;

    m_imageOffset.x += positionDelta.X;
    m_imageOffset.y += positionDelta.Y;

    // We want to have any zoom operation be "centered" around the pointer position, which
    // requires recalculating the view position based on the new zoom and pointer position.
    // Step 1: Calculate the absolute pointer position (image position).
    D2D1_POINT_2F pointerAbsolutePosition = D2D1::Point2F(
        (m_imageOffset.x - position.X) / m_zoom,
        (m_imageOffset.y - position.Y) / m_zoom
        );

    // Step 2: Apply the zoom; do not allow user to go beyond max zoom level.
    m_zoom *= zoomDelta;
    m_zoom = min(m_zoom, sc_MaxZoom);

    // Step 3: Adjust the view position based on the new m_zoom value.
    m_imageOffset.x = pointerAbsolutePosition.x * m_zoom + position.X;
    m_imageOffset.y = pointerAbsolutePosition.y * m_zoom + position.Y;

    // Step 4: Clamp the translation to the window bounds.
    Size panelSize = m_deviceResources->GetLogicalSize();
    m_imageOffset.x = Clamp(m_imageOffset.x, panelSize.Width - m_imageInfo.size.Width * m_zoom, 0);
    m_imageOffset.y = Clamp(m_imageOffset.y, panelSize.Height - m_imageInfo.size.Height * m_zoom, 0);

    UpdateImageTransformState();
    Draw();
}

// Overrides any pan/zoom state set by the user to fit image to the window size.
// Returns the computed MaxCLL of the image in nits.
float D2DAdvancedColorImagesRenderer::FitImageToWindow()
{
    if (m_imageSource)
    {
        Size panelSize = m_deviceResources->GetLogicalSize();

        // Set image to be letterboxed in the window, up to the max allowed scale factor.
        float letterboxZoom = min(
            panelSize.Width / m_imageInfo.size.Width,
            panelSize.Height / m_imageInfo.size.Height);

        m_zoom = min(sc_MaxZoom, letterboxZoom);

        // Center the image.
        m_imageOffset = D2D1::Point2F(
            (panelSize.Width - (m_imageInfo.size.Width * m_zoom)) / 2.0f,
            (panelSize.Height - (m_imageInfo.size.Height * m_zoom)) / 2.0f
            );

        UpdateImageTransformState();

        // HDR metadata is supposed to be independent of any rendering options, but
        // we can't compute it until the full effect graph is hooked up, which is here.
        ComputeHdrMetadata();
    }

    return m_maxCLL;
}

// After initial decode, obtain image information and do common setup.
// Populates all members of ImageInfo.
ImageInfo D2DAdvancedColorImagesRenderer::LoadImageCommon(_In_ IWICBitmapSource* source)
{
    auto wicFactory = m_deviceResources->GetWicImagingFactory();
    m_imageInfo = {};

    // Attempt to read the embedded color profile from the image; only valid for WIC images.
    ComPtr<IWICBitmapFrameDecode> frame;
    if (SUCCEEDED(source->QueryInterface(IID_PPV_ARGS(&frame))))
    {
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
    }

    // Check whether the image data is natively stored in a floating-point format, and
    // decode to the appropriate WIC pixel format.

    WICPixelFormatGUID pixelFormat;
    DX::ThrowIfFailed(
        source->GetPixelFormat(&pixelFormat)
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

    // Calculate the bits per channel (bit depth) using GetChannelMask.
    // This accounts for nonstandard color channel packing and padding, e.g. 32bppRGB.
    unsigned char channelMaskBytes[sc_MaxBytesPerPixel];
    ZeroMemory(channelMaskBytes, ARRAYSIZE(channelMaskBytes));
    unsigned int maskSize;

    DX::ThrowIfFailed(
        pixelFormatInfo->GetChannelMask(
            0,  // Read the first color channel.
            ARRAYSIZE(channelMaskBytes),
            channelMaskBytes,
            &maskSize)
        );

    // Count up the number of bits set in the mask for the first color channel.
    for (unsigned int i = 0; i < maskSize * 8; i++)
    {
        unsigned int byte = i / 8;
        unsigned int bit = i % 8;
        if ((channelMaskBytes[byte] & (1 << bit)) != 0)
        {
            m_imageInfo.bitsPerChannel += 1;
        }
    }

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
            source,
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

    PopulateImageInfoACKind(&m_imageInfo);

    return m_imageInfo;
}

// Derive the source color context from the image (embedded ICC profile or metadata).
void D2DAdvancedColorImagesRenderer::UpdateImageColorContext()
{
    auto context = m_deviceResources->GetD2DDeviceContext();

    ComPtr<ID2D1ColorContext> sourceColorContext;

    // For most image types, automatically derive the color context from the image.
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

    DX::ThrowIfFailed(
        m_colorManagementEffect->SetValue(
            D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT,
            sourceColorContext.Get()
        )
    );
}

// When connected to an HDR display, the OS renders SDR content (e.g. 8888 UNORM) at
// a user configurable white level; this typically is around 200-300 nits. It is the responsibility
// of an advanced color app (e.g. FP16 scRGB) to emulate the OS-implemented SDR white level adjustment,
// BUT only for non-HDR content (SDR or WCG).
void D2DAdvancedColorImagesRenderer::UpdateWhiteLevelScale(float brightnessAdjustment, float sdrWhiteLevel)
{
    float scale = 1.0f;

    switch (m_imageInfo.imageKind)
    {
    case AdvancedColorKind::HighDynamicRange:
        // HDR content should not be compensated by the SdrWhiteLevel parameter.
        scale = 1.0f;
        break;

    case AdvancedColorKind::StandardDynamicRange:
    case AdvancedColorKind::WideColorGamut:
    default:
        scale = sdrWhiteLevel / sc_nominalRefWhite;
        break;
    }

    // The user may want to manually adjust brightness specifically for this image, on top of any
    // white level adjustment for SDR/WCG content. Brightness adjustment using a linear gamma scale
    // is mainly useful for HDR displays, but can be useful for HDR content tonemapped to an SDR/WCG display.
    scale *= brightnessAdjustment;

    // SDR white level scaling is performing by multiplying RGB color values in linear gamma.
    // We implement this with a Direct2D matrix effect.
    D2D1_MATRIX_5X4_F matrix = D2D1::Matrix5x4F(
        scale, 0, 0, 0,  // [R] Multiply each color channel
        0, scale, 0, 0,  // [G] by the scale factor in 
        0, 0, scale, 0,  // [B] linear gamma space.
        0, 0, 0    , 1,  // [A] Preserve alpha values.
        0, 0, 0    , 0); //     No offset.

    DX::ThrowIfFailed(m_whiteScaleEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, matrix));
}

// Call this after updating any spatial transform state to regenerate the effect graph.
void D2DAdvancedColorImagesRenderer::UpdateImageTransformState()
{
    if (m_imageSource)
    {
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

// Uses a histogram to compute a modified version of MaxCLL (ST.2086 max content light level).
// Performs Begin/EndDraw on the D2D context.
void D2DAdvancedColorImagesRenderer::ComputeHdrMetadata()
{
    // Initialize with a sentinel value.
    m_maxCLL = -1.0f;

    // MaxCLL is not meaningful for SDR or WCG images.
    if ((!m_isComputeSupported) ||
        (m_imageInfo.imageKind != AdvancedColorKind::HighDynamicRange))
    {
        return;
    }

    // MaxCLL is nominally calculated for the single brightest pixel in a frame.
    // But we take a slightly more conservative definition that takes the 99.99th percentile
    // to account for extreme outliers in the image.
    float maxCLLPercent = 0.9999f;

    auto ctx = m_deviceResources->GetD2DDeviceContext();

    ctx->BeginDraw();

    ctx->DrawImage(m_histogramEffect.Get());

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = ctx->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    float *histogramData = new float[sc_histNumBins];
    DX::ThrowIfFailed(
        m_histogramEffect->GetValue(D2D1_HISTOGRAM_PROP_HISTOGRAM_OUTPUT,
            reinterpret_cast<BYTE*>(histogramData),
            sc_histNumBins * sizeof(float)
            )
        );

    unsigned int maxCLLbin = 0;
    float runningSum = 0.0f; // Cumulative sum of values in histogram is 1.0.
    for (int i = sc_histNumBins - 1; i >= 0; i--)
    {
        runningSum += histogramData[i];
        maxCLLbin = i;

        if (runningSum >= 1.0f - maxCLLPercent)
        {
            break;
        }
    }

    float binNorm = static_cast<float>(maxCLLbin) / static_cast<float>(sc_histNumBins);
    m_maxCLL = powf(binNorm, 1 / sc_histGamma) * sc_histMaxNits;

    // Some drivers have a bug where histogram will always return 0. Treat this as unknown.
    m_maxCLL = (m_maxCLL == 0.0f) ? -1.0f : m_maxCLL;
}

// Set HDR10 metadata to allow HDR displays to optimize behavior based on our content.
void D2DAdvancedColorImagesRenderer::EmitHdrMetadata()
{
    auto acKind = m_dispInfo ? m_dispInfo->CurrentAdvancedColorKind : AdvancedColorKind::StandardDynamicRange;

    if (acKind == AdvancedColorKind::HighDynamicRange)
    {
        DXGI_HDR_METADATA_HDR10 metadata = {};

        // This sample doesn't do any chrominance (e.g. xy) gamut mapping, so just use default
        // color primaries values; a more sophisticated app will explicitly set these.
        // DXGI_HDR_METADATA_HDR10 defines primaries as 1/50000 of a unit in xy space.
        metadata.RedPrimary[0]   = static_cast<UINT16>(m_dispInfo->RedPrimary.X   * 50000.0f);
        metadata.RedPrimary[1]   = static_cast<UINT16>(m_dispInfo->RedPrimary.Y   * 50000.0f);
        metadata.GreenPrimary[0] = static_cast<UINT16>(m_dispInfo->GreenPrimary.X * 50000.0f);
        metadata.GreenPrimary[1] = static_cast<UINT16>(m_dispInfo->GreenPrimary.Y * 50000.0f);
        metadata.BluePrimary[0]  = static_cast<UINT16>(m_dispInfo->BluePrimary.X  * 50000.0f);
        metadata.BluePrimary[1]  = static_cast<UINT16>(m_dispInfo->BluePrimary.Y  * 50000.0f);
        metadata.WhitePoint[0]   = static_cast<UINT16>(m_dispInfo->WhitePoint.X   * 50000.0f);
        metadata.WhitePoint[1]   = static_cast<UINT16>(m_dispInfo->WhitePoint.Y   * 50000.0f);

        float effectiveMaxCLL = 0;

        switch (m_renderEffectKind)
        {
        // Currently only the "None" render effect results in pixel values that exceed
        // the OS-specified SDR white level, as it just passes through HDR color values.
        case RenderEffectKind::None:
            effectiveMaxCLL = max(m_maxCLL, 0.0f) * m_brightnessAdjust;
            break;

        default:
            effectiveMaxCLL = m_dispInfo->SdrWhiteLevelInNits * m_brightnessAdjust;
            break;
        }

        // DXGI_HDR_METADATA_HDR10 defines MaxCLL in integer nits.
        metadata.MaxContentLightLevel = static_cast<UINT16>(effectiveMaxCLL);

        // The luminance analysis doesn't calculate MaxFrameAverageLightLevel. We also don't have mastering
        // information (i.e. reference display in a studio), so Min/MaxMasteringLuminance is not relevant.
        // Leave these values as 0.

        auto sc = m_deviceResources->GetSwapChain();

        ComPtr<IDXGISwapChain4> sc4;
        DX::ThrowIfFailed(sc->QueryInterface(IID_PPV_ARGS(&sc4)));
        DX::ThrowIfFailed(sc4->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_HDR10, sizeof(metadata), &metadata));
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
        d2dContext->DrawImage(m_finalOutput.Get(), m_imageOffset);

        EmitHdrMetadata();
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