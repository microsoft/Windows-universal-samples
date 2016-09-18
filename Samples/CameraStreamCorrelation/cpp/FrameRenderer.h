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

#pragma once

namespace SDKTemplate
{
    // Function type used to map scanline of pixels to an alternate format.
    typedef std::function<void(int, byte*, byte*)> TransformScanline;

    class FrameRenderer
    {
    public:
        FrameRenderer(Windows::UI::Xaml::Controls::Image^ image);

        /// <summary>
        /// Buffer and render color frame.
        /// </summary>
        void ProcessColorFrame(Windows::Media::Capture::Frames::MediaFrameReference^ colorFrame);

        /// <summary>
        /// Buffer and render correlated color and depth frames.
        /// </summary>
        void ProcessDepthAndColorFrames(
            Windows::Media::Capture::Frames::MediaFrameReference^ colorFrame,
            Windows::Media::Capture::Frames::MediaFrameReference^ depthFrame);

    private: // private methods
        /// <summary>
        /// Perform mapping of depth pixels to color pixels.
        /// </summary>
        Windows::Graphics::Imaging::SoftwareBitmap^ MapDepthToColor(
            Windows::Media::Capture::Frames::VideoMediaFrame^ colorFrame,
            Windows::Media::Capture::Frames::VideoMediaFrame^ depthFrame,
            Windows::Media::Devices::Core::CameraIntrinsics^ colorCameraIntrinsics,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ colorCoordinateSystem,
            Windows::Media::Devices::Core::DepthCorrelatedCoordinateMapper^ coordinateMapper);

        /// <summary>
        /// Buffer processed bitmap and render on UI.
        /// </summary>
        void BufferBitmapForRendering(Windows::Graphics::Imaging::SoftwareBitmap^ softwareBitmap);

        /// <summary>
        /// Keep presenting the m_backBuffer until there are no more.
        /// </summary>
        concurrency::task<void> FrameRenderer::DrainBackBufferAsync();

    private: // private data
        Windows::UI::Xaml::Controls::Image^ m_imageElement;
        Windows::Graphics::Imaging::SoftwareBitmap^ m_backBuffer;

        Platform::Array<Windows::Foundation::Point>^ m_colorSpacePoints;
        Platform::Array<Windows::Foundation::Numerics::float3>^ m_depthSpacePoints;

        UINT32 m_previousBufferWidth = 0;
        UINT32 m_previousBufferHeight = 0;

        bool m_taskRunning = false;

    private: // private synchronization
        std::mutex m_pointBufferMutex;

    };
} // CameraStreamCorrelation