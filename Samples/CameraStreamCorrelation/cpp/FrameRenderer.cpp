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
#include <MemoryBuffer.h>
#include "FrameRenderer.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Media::Devices::Core;
using namespace Windows::Perception::Spatial;
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

FrameRenderer::FrameRenderer(Image^ imageElement)
{
    m_imageElement = imageElement;
    m_imageElement->Source = ref new SoftwareBitmapSource();
}

task<void> FrameRenderer::DrainBackBufferAsync()
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

void FrameRenderer::ProcessColorFrame(MediaFrameReference^ colorFrame)
{
    if (colorFrame == nullptr)
    {
        return;
    }

    SoftwareBitmap^ inputBitmap = colorFrame->VideoMediaFrame->SoftwareBitmap;

    if (inputBitmap == nullptr)
    {
        return;
    }

    // If the input bitmap is in the correct format, copy it and then buffer it for rendering.
    if ((inputBitmap->BitmapPixelFormat == BitmapPixelFormat::Bgra8) &&
        (inputBitmap->BitmapAlphaMode == BitmapAlphaMode::Premultiplied))
    {
        BufferBitmapForRendering(SoftwareBitmap::Copy(inputBitmap));
    }
    // Otherwise, convert the bitmap to the correct format before buffering it for rendering.
    else
    {
        BufferBitmapForRendering(SoftwareBitmap::Convert(inputBitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied));
    }
}

void FrameRenderer::ProcessDepthAndColorFrames(MediaFrameReference^ colorFrame, MediaFrameReference^ depthFrame)
{
    if (colorFrame == nullptr || depthFrame == nullptr)
    {
        return;
    }

    // Create the coordinate mapper used to map depth pixels from depth space to color space.
    DepthCorrelatedCoordinateMapper^ coordinateMapper = depthFrame->VideoMediaFrame->DepthMediaFrame->TryCreateCoordinateMapper(
        colorFrame->VideoMediaFrame->CameraIntrinsics, colorFrame->CoordinateSystem);

    if (coordinateMapper == nullptr)
    {
        return;
    }

    // Map the depth image to color space and buffer the result for rendering.
    SoftwareBitmap^ softwareBitmap = MapDepthToColor(
        colorFrame->VideoMediaFrame,
        depthFrame->VideoMediaFrame,
        colorFrame->VideoMediaFrame->CameraIntrinsics,
        colorFrame->CoordinateSystem,
        coordinateMapper);

    if (softwareBitmap)
    {
        BufferBitmapForRendering(softwareBitmap);
    }
}

void FrameRenderer::BufferBitmapForRendering(SoftwareBitmap^ softwareBitmap)
{
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

SoftwareBitmap^ FrameRenderer::MapDepthToColor(
    VideoMediaFrame^ colorFrame,
    VideoMediaFrame^ depthFrame,
    CameraIntrinsics^ colorCameraIntrinsics,
    SpatialCoordinateSystem^ colorCoordinateSystem,
    DepthCorrelatedCoordinateMapper^ coordinateMapper)
{
    SoftwareBitmap^ inputBitmap = colorFrame->SoftwareBitmap;
    SoftwareBitmap^ outputBitmap;
    
    // Copy the color input bitmap so we may overlay the depth bitmap on top of it.
    if ((inputBitmap->BitmapPixelFormat == BitmapPixelFormat::Bgra8) &&
        (inputBitmap->BitmapAlphaMode == BitmapAlphaMode::Premultiplied))
    {
        outputBitmap = SoftwareBitmap::Copy(inputBitmap);
    }
    else
    {
        outputBitmap = SoftwareBitmap::Convert(inputBitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied);
    }

    // Create buffers used to access pixels.
    BitmapBuffer^ depthBuffer = depthFrame->SoftwareBitmap->LockBuffer(BitmapBufferAccessMode::Read);
    BitmapBuffer^ colorBuffer = colorFrame->SoftwareBitmap->LockBuffer(BitmapBufferAccessMode::Read);
    BitmapBuffer^ outputBuffer = outputBitmap->LockBuffer(BitmapBufferAccessMode::Write);

    if (depthBuffer == nullptr || colorBuffer == nullptr || outputBuffer == nullptr)
    {
        return nullptr;
    }

    BitmapPlaneDescription colorDesc = colorBuffer->GetPlaneDescription(0);
    UINT32 colorWidth = static_cast<UINT32>(colorDesc.Width);
    UINT32 colorHeight = static_cast<UINT32>(colorDesc.Height);

    IMemoryBufferReference^ depthReference = depthBuffer->CreateReference();
    IMemoryBufferReference^ outputReference = outputBuffer->CreateReference();

    byte* depthBytes = nullptr;
    UINT32 depthCapacity;

    byte* outputBytes = nullptr;
    UINT32 outputCapacity;

    AsComPtr<IMemoryBufferByteAccess>(depthReference)->GetBuffer(&depthBytes, &depthCapacity);
    AsComPtr<IMemoryBufferByteAccess>(outputReference)->GetBuffer(&outputBytes, &outputCapacity);

    if (depthBytes == nullptr || outputBytes == nullptr)
    {
        return nullptr;
    }

    ColorBGRA* outputPixels = reinterpret_cast<ColorBGRA*>(outputBytes);

    {
        // Ensure synchronous read/write access to point buffer cache.
        std::lock_guard<std::mutex> guard(m_pointBufferMutex);

        // If we don't have point arrays, or the ones we have are the wrong dimensions,
        // then create new ones.
        Array<Point>^ colorSpacePoints = m_colorSpacePoints;
        if (colorSpacePoints == nullptr ||
            m_previousBufferWidth != colorWidth ||
            m_previousBufferHeight != colorHeight)
        {
            colorSpacePoints = ref new Array<Point>(colorWidth * colorHeight);

            // Prepare array of points we want mapped.
            for (UINT y = 0; y < colorHeight; y++)
            {
                for (UINT x = 0; x < colorWidth; x++)
                {
                    colorSpacePoints[y * colorWidth + x] = Point(static_cast<float>(x), static_cast<float>(y));
                }
            }
        }

        Array<float3>^ depthSpacePoints = m_depthSpacePoints;
        if (depthSpacePoints == nullptr ||
            m_previousBufferWidth != colorWidth ||
            m_previousBufferHeight != colorHeight)
        {
            depthSpacePoints = ref new Array<float3>(colorWidth * colorHeight);
        }

        // Save the (possibly updated) values now that they are all known to be good.
        m_colorSpacePoints = colorSpacePoints;
        m_depthSpacePoints = depthSpacePoints;
        m_previousBufferWidth = colorWidth;
        m_previousBufferHeight = colorHeight;

        // Unproject depth points to color image.
        coordinateMapper->UnprojectPoints(
            m_colorSpacePoints, colorCoordinateSystem, m_depthSpacePoints);

        constexpr float depthFadeStart = 1;
        constexpr float depthFadeEnd = 1.5;

        // Using the depth values we fade the color pixels of the ouput if they are too far away.
        for (UINT y = 0; y < colorHeight; y++)
        {
            for (UINT x = 0; x < colorWidth; x++)
            {
                UINT index = y * colorWidth + x;

                // The z value of each depth space point contains the depth value of the point.
                // This value is mapped to a fade value. Fading starts at depthFadeStart meters
                // and is completely black by depthFadeEnd meters.
                float fadeValue = 1 - max(0, min(((m_depthSpacePoints[index].z - depthFadeStart) / (depthFadeEnd - depthFadeStart)), 1));

                outputPixels[index].R = static_cast<byte>(static_cast<float>(outputPixels[index].R) * fadeValue);
                outputPixels[index].G = static_cast<byte>(static_cast<float>(outputPixels[index].G) * fadeValue);
                outputPixels[index].B = static_cast<byte>(static_cast<float>(outputPixels[index].B) * fadeValue);
            }
        }
    }

    return outputBitmap;
}