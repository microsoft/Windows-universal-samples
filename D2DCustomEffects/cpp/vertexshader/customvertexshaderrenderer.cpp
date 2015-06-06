#include "pch.h"
#include "CustomVertexShaderRenderer.h"
#include "DirectXHelper.h"

using namespace CustomVertexShader;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

CustomVertexShaderRenderer::CustomVertexShaderRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    m_timer = ref new BasicTimer();

    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void CustomVertexShaderRenderer::CreateDeviceIndependentResources()
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

    // Register the custom wave effect.
    DX::ThrowIfFailed(WaveEffect::Register(d2dFactory));
}

void CustomVertexShaderRenderer::CreateDeviceDependentResources()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    // Load the image from WIC using ImageSource. The image is scaled using
    // ID2D1TransformedImageSource which is dependent on the scale factor.
    DX::ThrowIfFailed(d2dContext->CreateImageSourceFromWic(m_formatConvert.Get(), &m_imageSource));

    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_CustomWaveEffect, &m_waveEffect));
}

void CustomVertexShaderRenderer::ReleaseDeviceDependentResources()
{
    m_imageSource.Reset();
    m_waveEffect.Reset();
    m_transformedImage.Reset();
}

void CustomVertexShaderRenderer::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetLogicalSize();
    float scale = size.Width / m_imageSize.width;

    UpdateImageScale(scale);
}

// Sets up the image effect pipeline to reflect the new scale factor.
void CustomVertexShaderRenderer::UpdateImageScale(float scale)
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

    m_waveEffect->SetInput(0, m_transformedImage.Get());
}

void CustomVertexShader::CustomVertexShaderRenderer::UpdateManipulationState(Windows::UI::Input::ManipulationUpdatedEventArgs ^ args)
{
    m_skewX += args->Delta.Translation.X * 0.01f;
    m_skewY += args->Delta.Translation.Y * 0.01f;

    DX::ThrowIfFailed(m_waveEffect->SetValue(WAVE_PROP_SKEW_X, m_skewX));
    DX::ThrowIfFailed(m_waveEffect->SetValue(WAVE_PROP_SKEW_Y, m_skewY));
}

void CustomVertexShaderRenderer::Update()
{
    m_timer->Update();

    // The wave ripples over time.
    DX::ThrowIfFailed(
        m_waveEffect->SetValue(WAVE_PROP_OFFSET, m_timer->Total)
        );
}

void CustomVertexShaderRenderer::Render()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->BeginDraw();
    m_deviceResources->GetD2DDeviceContext()->SetTransform(m_deviceResources->GetOrientationTransform2D());
    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

    d2dContext->DrawImage(m_waveEffect.Get());

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }
}

// Saves the current state of the app for suspend and terminate events.
void _In_ CustomVertexShaderRenderer::SaveInternalState(_In_ IPropertySet ^ state)
{
    // Check to ensure each key is not already in the collection. If it is present, remove
    // it, before storing in the new value.

    if (state->HasKey("SkewX"))
    {
        state->Remove("SkewX");
    }
    state->Insert("SkewX", PropertyValue::CreateSingle(m_skewX));

    if (state->HasKey("SkewY"))
    {
        state->Remove("SkewY");
    }
    state->Insert("SkewY", PropertyValue::CreateSingle(m_skewY));
}

// Loads the current state of the app for resume events.
void CustomVertexShaderRenderer::LoadInternalState(_In_ IPropertySet ^ state)
{
    if (state->HasKey("skewX"))
    {
        m_skewX = safe_cast<IPropertyValue^>(state->Lookup("skewX"))->GetSingle();
    }

    if (state->HasKey("skewY"))
    {
        m_skewY = safe_cast<IPropertyValue^>(state->Lookup("skewY"))->GetSingle();
    }
}
