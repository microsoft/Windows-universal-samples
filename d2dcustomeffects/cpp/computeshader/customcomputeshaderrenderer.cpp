#include "pch.h"
#include "CustomComputeShaderRenderer.h"
#include "DirectXHelper.h"

using namespace CustomComputeShader;

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;

CustomComputeShaderRenderer::CustomComputeShaderRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void CustomComputeShaderRenderer::CreateDeviceIndependentResources()
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

    // Register the custom discrete Fourier transform effect.
    DX::ThrowIfFailed(DftEffect::Register(d2dFactory));
}

void CustomComputeShaderRenderer::CreateDeviceDependentResources()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    // Load the image from WIC using ImageSource. The image is scaled using
    // ID2D1TransformedImageSource which is dependent on the scale factor.
    DX::ThrowIfFailed(d2dContext->CreateImageSourceFromWic(m_formatConvert.Get(), &m_imageSource));

    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_CustomDftEffect, &m_dftEffect));
}

void CustomComputeShaderRenderer::ReleaseDeviceDependentResources()
{
    m_imageSource.Reset();
    m_transformedImage.Reset();
    m_dftEffect.Reset();
}

void CustomComputeShaderRenderer::CreateWindowSizeDependentResources()
{
    // Scale using pixel (not DIPs) because layout calculations are pixel-dependent.
    auto size = m_deviceResources->GetOutputSize();

    m_scale = min(
        (size.Width / m_imageSize.width) / 2.0f,
        (size.Height / m_imageSize.height) / 2.0f
        );

    UpdateImageScale(m_scale);
}

// Sets up the image effect pipeline to reflect the new scale factor.
void CustomComputeShaderRenderer::UpdateImageScale(float scale)
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
        D2D1_TRANSFORMED_IMAGE_SOURCE_OPTIONS_DISABLE_DPI_SCALE // Does not apply the context's DPI.
    };

    DX::ThrowIfFailed(d2dContext->CreateTransformedImageSource(
        m_imageSource.Get(),
        &props,
        &m_transformedImage));

    m_dftEffect->SetInput(0, m_transformedImage.Get());
}

void CustomComputeShaderRenderer::Render()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    auto renderTargetSize = m_deviceResources->GetOutputSize();

    d2dContext->BeginDraw();
    m_deviceResources->GetD2DDeviceContext()->SetTransform(m_deviceResources->GetOrientationTransform2D());
    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

    // Calculate display positioning for the scaled image and DFT effect.
    float scaledWidth = m_imageSize.width * m_scale;
    float scaledHeight = m_imageSize.height * m_scale;
    float horizontalMargin = ((renderTargetSize.Width / 2 - scaledWidth) / 2.0f);

    // Clip values nearest whole pixel to prevent effect output from being anti-aliased.
    scaledWidth = floor(scaledWidth);
    scaledHeight = floor(scaledHeight);
    horizontalMargin = floor(horizontalMargin);

    // Convert values to DIPs to preserve layout at different DPIs.
    scaledHeight = DX::ConvertPixelsToDips(scaledHeight);
    scaledWidth = DX::ConvertPixelsToDips(scaledWidth);
    horizontalMargin = DX::ConvertPixelsToDips(horizontalMargin);

    // Calculate whole pixel offset of screen midpoint before converting to DIPs to
    // prevent effect output from being antialiased.
    float screenMidpoint = floor(DX::ConvertPixelsToDips(renderTargetSize.Width / 2.0f));

    // Draw the original image on the left side of the window.
    d2dContext->DrawImage(m_transformedImage.Get(), D2D1::Point2F(horizontalMargin, 0.0f));

    // Draw the DFT of the image on the right side of the window.
    // Because image output is fully opaque, it should be blended with BOUNDED_SOURCE_COPY, which allows
    // the compute shader output to be written directly to the render target instead of blended.
    d2dContext->DrawImage(
        m_dftEffect.Get(),
        D2D1::Point2F(screenMidpoint + horizontalMargin, 0.0f),
        D2D1::RectF(0.0f, 0.0f, scaledWidth, scaledHeight),
        D2D1_INTERPOLATION_MODE_LINEAR,
        D2D1_COMPOSITE_MODE_BOUNDED_SOURCE_COPY
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }
}
