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
#include "PhotoAdjustmentRenderer.h"
#include "DirectXPage.xaml.h"
#include "Common\DirectXHelper.h"

using namespace D2DPhotoAdjustment;

using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Input;

PhotoAdjustmentRenderer::PhotoAdjustmentRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    m_maxZoom(1.0f), // Never allow upscaling.
    m_imageSize(),
    m_panelSize(),
    m_isWindowClosed(false),
    m_zoom()
{
    // Register to be notified if the GPU device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

PhotoAdjustmentRenderer::~PhotoAdjustmentRenderer()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

void PhotoAdjustmentRenderer::CreateDeviceIndependentResources()
{
    auto wicFactory = m_deviceResources->GetWicImagingFactory();
    auto d2dFactory = m_deviceResources->GetD2DFactory();

    // Use WIC to decode the JPEG image and load it into Direct2D.
    ComPtr<IWICBitmapDecoder> decoder;
    DX::ThrowIfFailed(wicFactory->CreateDecoderFromFilename(
        L"Assets\\rainier.jpg",
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        &decoder
        ));

    ComPtr<IWICBitmapFrameDecode> frame;
    DX::ThrowIfFailed(decoder->GetFrame(0, &frame));

    // Convert the image to a pixel format supported by Direct2D. 32bppPBGRA is
    // guaranteed to be supported on all hardware.
    DX::ThrowIfFailed(wicFactory->CreateFormatConverter(&m_formatConvert));
    DX::ThrowIfFailed(m_formatConvert->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeCustom
        ));

    UINT width;
    UINT height;
    DX::ThrowIfFailed(m_formatConvert->GetSize(&width, &height));
    m_imageSize = Size(static_cast<float>(width), static_cast<float>(height));

    // Attempt to read the embedded color profile from the image; use sRGB if the data doesn't exist.
    UINT actualCount;
    DX::ThrowIfFailed(wicFactory->CreateColorContext(&m_wicColorContext));
    DX::ThrowIfFailed(frame->GetColorContexts(1, m_wicColorContext.GetAddressOf(), &actualCount));

    if (actualCount == 0)
    {
        m_wicColorContext->InitializeFromExifColorSpace(1); // 1 = sRGB.
    }
}

void PhotoAdjustmentRenderer::CreateDeviceDependentResources()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    auto d2dDevice = m_deviceResources->GetD2DDevice();

    // Load the image from WIC using ID2D1ImageSource. The image source selects optimal image loading
    // and caching behavior for the scenario and also supports greater than max texture size images.
    // Scaling is accomplished using ID2D1TransformedImageSource in UpdateZoomState().
    DX::ThrowIfFailed(d2dContext->CreateImageSourceFromWic(m_formatConvert.Get(), &m_imageSource));

    // Create the photo adjustment pipeline.
    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1ColorManagement, &m_colorManagement));
    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1Straighten, &m_straighten));
    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1TemperatureTint, &m_temperatureTint));
    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1Saturation, &m_saturation));
    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1Contrast, &m_contrast));
    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1HighlightsShadows, &m_highlightsShadows));

    // Chain the individual photo adjustment effects together.
    // The input to the first effect (color management) is set during UpdateZoomState.
    m_straighten->SetInputEffect(0, m_colorManagement.Get());
    m_temperatureTint->SetInputEffect(0, m_straighten.Get());
    m_saturation->SetInputEffect(0, m_temperatureTint.Get());
    m_contrast->SetInputEffect(0, m_saturation.Get());
    m_highlightsShadows->SetInputEffect(0, m_contrast.Get());

    // m_outputEffect always points to the photo pipeline output.
    DX::ThrowIfFailed(m_highlightsShadows.CopyTo(&m_outputEffect));

    // Do one-time configuration of the photo pipeline.
    m_straighten->SetValue(D2D1_STRAIGHTEN_PROP_MAINTAIN_SIZE, TRUE);

    // Consider using a higher quality mode for offline processing, like saving the image to disk.
    m_colorManagement->SetValue(D2D1_COLORMANAGEMENT_PROP_QUALITY, D2D1_COLORMANAGEMENT_QUALITY_PROOF);

    ComPtr<ID2D1ColorContext> sourceContext;
    DX::ThrowIfFailed(d2dContext->CreateColorContextFromWicColorContext(m_wicColorContext.Get(), &sourceContext));
    m_colorManagement->SetValue(D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT, sourceContext.Get());

    // Force loading the current display color context.
    OnColorProfileChanged(DisplayInformation::GetForCurrentView());
}

void PhotoAdjustmentRenderer::ReleaseDeviceDependentResources()
{
    m_imageSource.Reset();
    m_scaledImage.Reset();
    m_colorManagement.Reset();
    m_straighten.Reset();
    m_temperatureTint.Reset();
    m_saturation.Reset();
    m_contrast.Reset();
    m_highlightsShadows.Reset();
    m_outputEffect.Reset();
}

void PhotoAdjustmentRenderer::CreateWindowSizeDependentResources()
{
    m_panelSize = m_deviceResources->GetLogicalSize();

    // Scale the image to fill the window, but never exceed max scale.
    m_zoom = min(
        max(
            m_panelSize.Width / m_imageSize.Width,
            m_panelSize.Height / m_imageSize.Height),
        m_maxZoom);

    UpdateZoomState();
}

// Update render pipeline to reflect the new zoom state.
void PhotoAdjustmentRenderer::UpdateZoomState()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

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

    DX::ThrowIfFailed(d2dContext->CreateTransformedImageSource(
        m_imageSource.Get(),
        &props,
        &m_scaledImage
        ));

    m_colorManagement->SetInput(0, m_scaledImage.Get());

    // Adjust DIP properties for the new zoom and/or DPI.
    m_highlightsShadows->SetValue(
        D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS,
        ConvertDipProperty(m_hsMaskRadiusDips));
}

void PhotoAdjustmentRenderer::Draw()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->BeginDraw();

    // DeviceResources explicitly handles screen rotation by setting the rotation on the swap chain.
    // In order for Direct2D content to appear correctly on this swap chain, the app is responsible for
    // explicitly "prerotating" by setting the appropriate transform before drawing.
    m_deviceResources->GetD2DDeviceContext()->SetTransform(m_deviceResources->GetOrientationTransform2D());
    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

    d2dContext->DrawImage(m_outputEffect.Get());

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
void PhotoAdjustmentRenderer::OnDeviceLost()
{
    ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void PhotoAdjustmentRenderer::OnDeviceRestored()
{
    CreateDeviceDependentResources();
}

void PhotoAdjustmentRenderer::OnColorProfileChanged(_In_ DisplayInformation^ sender)
{
    if (m_colorManagement != nullptr)
    {
        // The color profile is stored in the DisplayInformation object.
        // We need to kick off an asynchronous operation to load the information
        // before passing it to the renderer to update the effect pipeline.
        shared_ptr<DataReader^> profileReaderPtr = make_shared<DataReader^>();

        create_task(sender->GetColorProfileAsync()).then([=](IRandomAccessStream^ profileStream)
        {
            // Extract the profile bytes from the IRandomAccessStream to initialize the Direct2D color context.
            *profileReaderPtr = ref new DataReader(profileStream);

            return (*profileReaderPtr)->LoadAsync(static_cast<unsigned int>(profileStream->Size));
        }).then([=](unsigned int readBytes)
        {
            // Direct2D was initialized using D2D1_FACTORY_TYPE_SINGLE_THREADED. Therefore, we must
            // ensure that async continuations are executed in the UI thread context. Here, this is done
            // for us automatically because create_task wraps a Windows Runtime async operation.
            UpdateDisplayColorContext(*profileReaderPtr);
            Draw();
        }).then([=](task<void> previousTask)
        {
            try
            {
                // DisplayInformation::GetColorProfileAsync() fails if there isn't a valid ICC color
                // profile associated with the display. This is expected and handled below.
                previousTask.get();
            }
            catch (Platform::COMException^ e)
            {
                if (e->HResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) ||
                    e->HResult == HRESULT_FROM_WIN32(ERROR_INVALID_PROFILE))
                {
                    UpdateDisplayColorContext(nullptr);
                    Draw();
                }
                else
                {
                    throw e;
                }
            }
        });
    }
}

// Pass in nullptr to use the standard sRGB color space.
void PhotoAdjustmentRenderer::UpdateDisplayColorContext(
    _In_opt_ DataReader^ colorProfileDataReader
    )
{
    ComPtr<ID2D1ColorContext> displayColorContext;
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    if (colorProfileDataReader == nullptr)
    {
        DX::ThrowIfFailed(
            d2dContext->CreateColorContext(D2D1_COLOR_SPACE_SRGB, nullptr, 0, &displayColorContext)
            );
    }
    else
    {
        Array<byte>^ profileBytes = ref new Array<byte>(colorProfileDataReader->UnconsumedBufferLength);
        colorProfileDataReader->ReadBytes(profileBytes);
        DX::ThrowIfFailed(d2dContext->CreateColorContext(
            D2D1_COLOR_SPACE_CUSTOM,
            profileBytes->Data,
            profileBytes->Length,
            &displayColorContext
            ));
    }

    DX::ThrowIfFailed(m_colorManagement->SetValue(
        D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT,
        displayColorContext.Get()
        ));
}

// Converts an effect property from DIPs to pixels in the unscaled image's coordinate
// space. This compensates for screen DPI and image scaling, which is needed for
// correct behavior in a photo adjustment pipeline.
float PhotoAdjustmentRenderer::ConvertDipProperty(float valueInDips)
{
    float dpiX, dpiY;
    m_deviceResources->GetD2DDeviceContext()->GetDpi(&dpiX, &dpiY);
    return valueInDips * m_zoom * 96.0f / dpiX;
}

// Invoked when one or more of the photo adjustment pipeline values have changed.
void PhotoAdjustmentRenderer::UpdatePhotoAdjustmentValues(PhotoAdjustmentProperties properties)
{
    // Testing for the validity of one of the device-dependent resources is sufficient.
    if (m_imageSource != nullptr)
    {
        DX::ThrowIfFailed(m_straighten->SetValue(
            D2D1_STRAIGHTEN_PROP_ANGLE,
            static_cast<float>(properties.StraightenValue)));

        DX::ThrowIfFailed(m_temperatureTint->SetValue(
            D2D1_TEMPERATUREANDTINT_PROP_TEMPERATURE,
            static_cast<float>(properties.TemperatureValue)));

        DX::ThrowIfFailed(m_temperatureTint->SetValue(
            D2D1_TEMPERATUREANDTINT_PROP_TINT,
            static_cast<float>(properties.TintValue)));

        DX::ThrowIfFailed(m_saturation->SetValue(
            D2D1_SATURATION_PROP_SATURATION,
            static_cast<float>(properties.SaturationValue)));

        DX::ThrowIfFailed(m_contrast->SetValue(
            D2D1_CONTRAST_PROP_CONTRAST,
            static_cast<float>(properties.ContrastValue)));

        DX::ThrowIfFailed(m_highlightsShadows->SetValue(
            D2D1_HIGHLIGHTSANDSHADOWS_PROP_SHADOWS,
            static_cast<float>(properties.ShadowsValue)));

        DX::ThrowIfFailed(m_highlightsShadows->SetValue(
            D2D1_HIGHLIGHTSANDSHADOWS_PROP_CLARITY,
            static_cast<float>(properties.ClarityValue)));

        DX::ThrowIfFailed(m_highlightsShadows->SetValue(
            D2D1_HIGHLIGHTSANDSHADOWS_PROP_HIGHLIGHTS,
            static_cast<float>(properties.HighlightsValue)));

        m_hsMaskRadiusDips = static_cast<float>(properties.HsMaskRadiusValue);
        DX::ThrowIfFailed(m_highlightsShadows->SetValue(
            D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS,
            ConvertDipProperty(m_hsMaskRadiusDips)));
    }
}