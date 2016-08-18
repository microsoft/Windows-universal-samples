//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include <cmath>
#include <MemoryBuffer.h>
#include "FrameRenderer.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;

#pragma region Low-level operations on reference pointers

// InterlockedExchange for reference pointer types.
template<typename T, typename U>
T^ InterlockedExchangeRefPointer(T^* target, U value)
{
    static_assert(sizeof(T^) == sizeof(void*), "InterlockedExchangePointer is the wrong size");
    T^ exchange = value;
    void** rawExchange = reinterpret_cast<void**>(&exchange);
    void** rawTarget = reinterpret_cast<void**>(target);
    *rawExchange = static_cast<IInspectable*>(InterlockedExchangePointer(rawTarget, *rawExchange));
    return exchange;
}

// Convert a reference pointer to a specific ComPtr.
template<typename T>
Microsoft::WRL::ComPtr<T> AsComPtr(Platform::Object^ object)
{
    Microsoft::WRL::ComPtr<T> p;
    reinterpret_cast<IUnknown*>(object)->QueryInterface(IID_PPV_ARGS(&p));
    return p;
}
#pragma endregion

// Structure used to access colors stored in 8-bit BGRA format.
struct ColorBGRA
{
    byte B, G, R, A;
};

// Colors to map values to based on intensity.
static constexpr std::array<ColorBGRA, 9> colorRamp = {
    ColorBGRA{ 0xFF, 0x7F, 0x00, 0x00 },
    ColorBGRA{ 0xFF, 0xFF, 0x00, 0x00 },
    ColorBGRA{ 0xFF, 0xFF, 0x7F, 0x00 },
    ColorBGRA{ 0xFF, 0xFF, 0xFF, 0x00 },
    ColorBGRA{ 0xFF, 0x7F, 0xFF, 0x7F },
    ColorBGRA{ 0xFF, 0x00, 0xFF, 0xFF },
    ColorBGRA{ 0xFF, 0x00, 0x7F, 0xFF },
    ColorBGRA{ 0xFF, 0x00, 0x00, 0xFF },
    ColorBGRA{ 0xFF, 0x00, 0x00, 0x7F }
};

static ColorBGRA ColorRampInterpolation(float value)
{
    static_assert(colorRamp.size() >= 2, "colorRamp table is too small");

    // Map value to surrounding indexes on the color ramp.
    size_t rampSteps = colorRamp.size() - 1;
    float scaled = value * rampSteps;
    int integer = static_cast<int>(scaled);
    int index = min(static_cast<size_t>(max(0, integer)), rampSteps - 1);
    const ColorBGRA& prev = colorRamp[index];
    const ColorBGRA& next = colorRamp[index + 1];

    // Set color based on a ratio of how closely it matches the surrounding colors.
    UINT32 alpha = static_cast<UINT32>((scaled - integer) * 255);
    UINT32 beta = 255 - alpha;
    return {
        static_cast<byte>((prev.A * beta + next.A * alpha) / 255), // Alpha
        static_cast<byte>((prev.R * beta + next.R * alpha) / 255), // Red
        static_cast<byte>((prev.G * beta + next.G * alpha) / 255), // Green
        static_cast<byte>((prev.B * beta + next.B * alpha) / 255)  // Blue
    };
}

// Initializes pseudo-color look up table for depth pixels
static ColorBGRA GeneratePseudoColorLookupTable(UINT32 index, UINT32 size)
{
    return ColorRampInterpolation(static_cast<float>(index) / static_cast<float>(size));
}

// Initializes the pseudo-color look up table for infrared pixels
static ColorBGRA GenerateInfraredRampLookupTable(UINT32 index, UINT32 size)
{
    const float value = static_cast<float>(index) / static_cast<float>(size);

    // Adjust to increase color change between lower values in infrared images.
    const float alpha = powf(1 - value, 12);

    return ColorRampInterpolation(alpha);
}

static LookupTable<ColorBGRA, 1024> colorLookupTable(GeneratePseudoColorLookupTable);
static LookupTable<ColorBGRA, 1024> infraredLookupTable(GenerateInfraredRampLookupTable);

static ColorBGRA PseudoColor(float value)
{
    return colorLookupTable.GetValue(value);
}

static ColorBGRA InfraredColor(float value)
{
    return infraredLookupTable.GetValue(value);
}

// Maps each pixel in a scanline from a 16 bit depth value to a pseudo-color pixel.
static void PseudoColorForDepth(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes, float depthScale)
{
    // Visualize space in front of your desktop, in meters.
    constexpr float min = 0.5f;   // 0.5 meters
    constexpr float max = 4.0f;  // 4 meters
    constexpr float one_min = 1.0f / min;
    constexpr float range = 1.0f / max - one_min;

    UINT16* inputRow = reinterpret_cast<UINT16*>(inputRowBytes);
    ColorBGRA* outputRow = reinterpret_cast<ColorBGRA*>(outputRowBytes);
    for (int x = 0; x < pixelWidth; x++)
    {
        float depth = static_cast<float>(inputRow[x]) * depthScale;

        // Map invalid depth values to transparent pixels.
        // This happens when depth information cannot be calculated, e.g. when objects are too close.
        if (depth == 0)
        {
            outputRow[x] = { 0 };
        }
        else
        {
            float alpha = (1.0f / depth - one_min) / range;
            outputRow[x] = PseudoColor(alpha * alpha);
        }
    }
}

// Maps each pixel in a scanline from a 16 bit infrared value to a pseudo-color pixel.
static void PseudoColorFor16BitInfrared(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes)
{
    UINT16* inputRow = reinterpret_cast<UINT16*>(inputRowBytes);
    ColorBGRA* outputRow = reinterpret_cast<ColorBGRA*>(outputRowBytes);
    for (int x = 0; x < pixelWidth; x++)
    {
        outputRow[x] = InfraredColor(inputRow[x] / static_cast<float>(UINT16_MAX));
    }
}

// Maps each pixel in a scanline from a 8 bit infrared value to a pseudo-color pixel.
static void PseudoColorFor8BitInfrared(int pixelWidth, byte* inputRowBytes, byte* outputRowBytes)
{
    ColorBGRA* outputRow = reinterpret_cast<ColorBGRA*>(outputRowBytes);
    for (int x = 0; x < pixelWidth; x++)
    {
        outputRow[x] = InfraredColor(inputRowBytes[x] / static_cast<float>(UINT8_MAX));
    }
}

FrameRenderer::FrameRenderer(Image^ imageElement)
{
    m_imageElement = imageElement;
    m_imageElement->Source = ref new SoftwareBitmapSource();
}

Concurrency::task<void> FrameRenderer::DrainBackBufferAsync()
{
    // Keep draining frames from the backbuffer until the backbuffer is empty.
    SoftwareBitmap^ latestBitmap = InterlockedExchangeRefPointer(&m_backBuffer, nullptr);
    if (latestBitmap != nullptr)
    {
        if (SoftwareBitmapSource^ imageSource = dynamic_cast<SoftwareBitmapSource^>(m_imageElement->Source))
        {
            return create_task(imageSource->SetBitmapAsync(latestBitmap))
                .then([this]()
            {
                return DrainBackBufferAsync();
            }, task_continuation_context::use_current());
        }
    }

    // To avoid a race condition against ProcessFrame, we cannot let any other
    // tasks run on the UI thread between point that the InterlockedExchangeRefPointer
    // reports that there is no more work, and we clear the m_taskRunning flag on
    // the UI thread.
    m_taskRunning = false;

    return task_from_result();
}

void FrameRenderer::ProcessFrame(Windows::Media::Capture::Frames::MediaFrameReference^ frame)
{
    if (frame == nullptr)
    {
        return;
    }

    SoftwareBitmap^ softwareBitmap = ConvertToDisplayableImage(frame->VideoMediaFrame);
    if (softwareBitmap != nullptr)
    {
        // Swap the processed frame to _backBuffer, and trigger the UI thread to render it.
        softwareBitmap = InterlockedExchangeRefPointer(&m_backBuffer, softwareBitmap);

        // UI thread always resets m_backBuffer before using it. Unused bitmap should be disposed.
        delete softwareBitmap;

        // Changes to the XAML ImageElement must happen in the UI thread, via the CoreDispatcher.
        m_imageElement->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            // Don't let two copies of this task run at the same time.
            if (m_taskRunning)
            {
                return;
            }

            m_taskRunning = true;

            // Keep draining frames from the backbuffer until the backbuffer is empty.
            DrainBackBufferAsync();
        }));
    }
}

SoftwareBitmap^ FrameRenderer::ConvertToDisplayableImage(VideoMediaFrame^ inputFrame)
{
    if (inputFrame == nullptr)
    {
        return nullptr;
    }

    SoftwareBitmap^ inputBitmap = inputFrame->SoftwareBitmap;
    if ((inputBitmap->BitmapPixelFormat == BitmapPixelFormat::Bgra8) &&
        (inputBitmap->BitmapAlphaMode == BitmapAlphaMode::Premultiplied))
    {
        // SoftwareBitmap is already in the correct format for an Image control, so just return a copy.
        return SoftwareBitmap::Copy(inputBitmap);
    }
    else if ((inputBitmap->BitmapPixelFormat == BitmapPixelFormat::Gray16) && (inputFrame->FrameReference->SourceKind == MediaFrameSourceKind::Depth))
    {
        using namespace std::placeholders;

        // Use a special pseudo color to render 16 bits depth frame.
        // Since we must scale the output appropriately we use std::bind to
        // create a function that takes the depth scale as input but also matches
        // the required signature.
        double depthScale = inputFrame->DepthMediaFrame->DepthFormat->DepthScaleInMeters;
        return TransformBitmap(inputBitmap, std::bind(&PseudoColorForDepth, _1, _2, _3, static_cast<float>(depthScale)));
    }
    else if (inputBitmap->BitmapPixelFormat == BitmapPixelFormat::Gray16)
    {
        // Use pseudo color to render 16 bits frames.
        return TransformBitmap(inputBitmap, PseudoColorFor16BitInfrared);
    }
    else if (inputBitmap->BitmapPixelFormat == BitmapPixelFormat::Gray8)
    {
        // Use pseudo color to render 8 bits frames.
        return TransformBitmap(inputBitmap, PseudoColorFor8BitInfrared);
    }
    else
    {
        try
        {
            // Convert to Bgra8 Premultiplied SoftwareBitmap, so xaml can display in UI.
            return SoftwareBitmap::Convert(inputBitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied);
        }
        catch (InvalidArgumentException^ exception)
        {
            // Conversion of software bitmap format is not supported.  Drop this frame.
            OutputDebugStringW(exception->Message->Data());
            OutputDebugStringW(L"\r\n");
            return nullptr;
        }
    }

    return nullptr;
}

SoftwareBitmap^ FrameRenderer::TransformBitmap(SoftwareBitmap^ inputBitmap, TransformScanline pixelTransformation)
{
    // XAML Image control only supports premultiplied Bgra8 format.
    SoftwareBitmap^ outputBitmap = ref new SoftwareBitmap(
        BitmapPixelFormat::Bgra8,
        inputBitmap->PixelWidth,
        inputBitmap->PixelHeight,
        BitmapAlphaMode::Premultiplied);

    BitmapBuffer^ input = inputBitmap->LockBuffer(BitmapBufferAccessMode::Read);
    BitmapBuffer^ output = outputBitmap->LockBuffer(BitmapBufferAccessMode::Write);

    // Get stride values to calculate buffer position for a given pixel x and y position.
    int inputStride = input->GetPlaneDescription(0).Stride;
    int outputStride = output->GetPlaneDescription(0).Stride;

    int pixelWidth = inputBitmap->PixelWidth;
    int pixelHeight = inputBitmap->PixelHeight;

    IMemoryBufferReference^ inputReference = input->CreateReference();
    IMemoryBufferReference^ outputReference = output->CreateReference();

    // Get input and output byte access buffers.
    byte* inputBytes;
    UINT32 inputCapacity;
    AsComPtr<IMemoryBufferByteAccess>(inputReference)->GetBuffer(&inputBytes, &inputCapacity);

    byte* outputBytes;
    UINT32 outputCapacity;
    AsComPtr<IMemoryBufferByteAccess>(outputReference)->GetBuffer(&outputBytes, &outputCapacity);

    // Iterate over all pixels, and store the converted value.
    for (int y = 0; y < pixelHeight; y++)
    {
        byte* inputRowBytes = inputBytes + y * inputStride;
        byte* outputRowBytes = outputBytes + y * outputStride;

        pixelTransformation(pixelWidth, inputRowBytes, outputRowBytes);
    }

    // Close objects that need closing.
    delete outputReference;
    delete inputReference;
    delete output;
    delete input;

    return outputBitmap;
}
