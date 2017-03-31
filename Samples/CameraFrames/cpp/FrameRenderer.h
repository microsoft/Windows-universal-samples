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

#pragma once

#include "LookupTable.h"

namespace SDKTemplate
{
    // Function type used to map a scanline of pixels to an alternate pixel format.
    typedef std::function<void(int, byte*, byte*)> TransformScanline;

    private ref class FrameRenderer sealed
    {
    public:
        FrameRenderer(Windows::UI::Xaml::Controls::Image^ image);

    public: // Public methods.
        /// <summary>
        /// Buffer and render frames.
        /// </summary>
        void ProcessFrame(Windows::Media::Capture::Frames::MediaFrameReference^ frame);

    private: // Private static methods.
        /// <summary>
        /// Converts the input frame to BGRA8 premultiplied alpha format and returns the result.
        /// Returns nullptr if the input frame cannot be converted BGRA8 premultiplied alpha.
        /// </summary>
        static Windows::Graphics::Imaging::SoftwareBitmap^ ConvertToDisplayableImage(
            Windows::Media::Capture::Frames::VideoMediaFrame^ inputFrame);
            
        /// <summary>
        /// Transforms pixels of inputBitmap to an output bitmap using the supplied pixel transformation method.
        /// Returns nullptr if translation fails.
        /// </summary>
        static Windows::Graphics::Imaging::SoftwareBitmap^ TransformBitmap(
            Windows::Graphics::Imaging::SoftwareBitmap^ inputBitmap,
            TransformScanline pixelTransformation);

    private: // Private instance methods.
        /// <summary>
        /// Keep presenting the m_backBuffer until there are no more.
        /// </summary>
        Concurrency::task<void> DrainBackBufferAsync();

    private: // Private data.
        Windows::UI::Xaml::Controls::Image^ m_imageElement;
        Windows::Graphics::Imaging::SoftwareBitmap^ m_backBuffer;
        bool m_taskRunning = false;

    };
} // SDKTemplate