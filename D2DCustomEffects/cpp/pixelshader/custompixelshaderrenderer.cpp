#include "pch.h"
#include "CustomPixelShaderRenderer.h"
#include "DirectXHelper.h"

using namespace CustomPixelShader;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;

CustomPixelShaderRenderer::CustomPixelShaderRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    m_isRippleAnimating(false)
{
    m_timer = ref new BasicTimer();

    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void CustomPixelShaderRenderer::CreateDeviceIndependentResources()
{
    auto wicFactory = m_deviceResources->GetWicImagingFactory();
    auto d2dFactory = m_deviceResources->GetD2DFactory();

    // Create WIC Decoder to read JPG file.
    ComPtr<IWICBitmapDecoder> decoder;
    DX::ThrowIfFailed(
        wicFactory->CreateDecoderFromFilename(
        L"Assets\\mammoth.jpg",
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        &decoder
        ));

    ComPtr<IWICBitmapFrameDecode> frame;
    DX::ThrowIfFailed(decoder->GetFrame(0, &frame));

    DX::ThrowIfFailed(wicFactory->CreateFormatConverter(&m_formatConvert));
    DX::ThrowIfFailed(m_formatConvert->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppPBGRA, // 32bppPBGRA is used by Direct2D.
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeCustom
        ));

    UINT width;
    UINT height;
    DX::ThrowIfFailed(m_formatConvert->GetSize(&width, &height));
    m_imageSize = D2D1::SizeU(width, height);

    // Register the custom ripple effect.
    DX::ThrowIfFailed(RippleEffect::Register(d2dFactory));
}

void CustomPixelShaderRenderer::CreateDeviceDependentResources()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    // Load the image from WIC using ImageSource. The image is scaled using
    // ID2D1TransformedImageSource which is dependent on the scale factor.
    DX::ThrowIfFailed(d2dContext->CreateImageSourceFromWic(m_formatConvert.Get(), &m_imageSource));

    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_CustomRippleEffect, &m_rippleEffect));
}

void CustomPixelShaderRenderer::ReleaseDeviceDependentResources()
{
    m_imageSource.Reset();
    m_rippleEffect.Reset();
    m_transformedImage.Reset();
}

void CustomPixelShaderRenderer::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetLogicalSize();
    float scale = size.Width / m_imageSize.width;

    UpdateImageScale(scale);
}

// Sets up the image effect pipeline to reflect the new scale factor.
void CustomPixelShaderRenderer::UpdateImageScale(float scale)
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    // When using ID2D1ImageSource, the recommend method of applying a transform
    // is to use ID2D1TransformedImageSource. It is inexpensive to recreate this object.
    D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES props =
    {
        D2D1_ORIENTATION_DEFAULT,
        scale,
        scale,
        D2D1_INTERPOLATION_MODE_LINEAR, // This is ignored when using DrawImage.
        D2D1_TRANSFORMED_IMAGE_SOURCE_OPTIONS_NONE
    };

    DX::ThrowIfFailed(d2dContext->CreateTransformedImageSource(
        m_imageSource.Get(),
        &props,
        &m_transformedImage));

    m_rippleEffect->SetInput(0, m_transformedImage.Get());
}

void CustomPixelShaderRenderer::UpdatePointer(_In_ PointerEventArgs^ args)
{
    // Reset the timer to the start of the animation.
    m_timer->Reset();

    // Since the custom ripple effect expects positioning in pixels, convert the event position from DIPs to pixels.
    float dpi = DisplayInformation::GetForCurrentView()->LogicalDpi;
    D2D1_POINT_2F position = D2D1::Point2F(
        DX::ConvertDipsToPixels(args->CurrentPoint->Position.X, dpi),
        DX::ConvertDipsToPixels(args->CurrentPoint->Position.Y, dpi)
        );

    DX::ThrowIfFailed(m_rippleEffect->SetValue(RIPPLE_PROP_CENTER, position));

    // While ripple is animating, the program needs to both continuously render
    // and continuously poll for inputs.
    m_isRippleAnimating = true;
}

void CustomPixelShaderRenderer::Update()
{
    if (m_isRippleAnimating)
    {
        m_timer->Update();
        float delta = m_timer->Total;

        // Stop animating after four seconds.
        if (delta >= 4)
        {
            delta = 4;
            m_isRippleAnimating = false;
        }

        // Increase the spread over time to make the visible area of the waves spread out.
        DX::ThrowIfFailed(m_rippleEffect->SetValue(RIPPLE_PROP_SPREAD, 0.01f + delta / 10.0f));

        // Reduce the amplitude over time to make the waves decay in intensity.
        DX::ThrowIfFailed(m_rippleEffect->SetValue(RIPPLE_PROP_AMPLITUDE, 60.0f - delta * 15.0f));

        // Reduce the frequency over time to make each individual wave spread out.
        DX::ThrowIfFailed(m_rippleEffect->SetValue(RIPPLE_PROP_FREQUENCY, 140.0f - delta * 30.0f));

        // Change the phase over time to make each individual wave travel away from the center.
        DX::ThrowIfFailed(m_rippleEffect->SetValue(RIPPLE_PROP_PHASE, -delta * 20.0f));
    }
}

void CustomPixelShaderRenderer::Render()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->BeginDraw();
    m_deviceResources->GetD2DDeviceContext()->SetTransform(m_deviceResources->GetOrientationTransform2D());
    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

    d2dContext->DrawImage(m_rippleEffect.Get());

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }
}
