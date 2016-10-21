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

#include "Scenario1_DisplayDepthColorIR.g.h"
#include "MainPage.xaml.h"
#include "SimpleLogger.h"
#include "FrameRenderer.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_DisplayDepthColorIR sealed
    {
    public:
        Scenario1_DisplayDepthColorIR();

    protected: // Protected UI overrides.
        /// <summary>
        /// Called when user navigates to this Scenario.
        /// Immediately start streaming from first available source group.
        /// </summary>
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        /// <summary>
        /// Called when user navigates away from this Scenario.
        /// Stops streaming and disposes of all objects.
        /// </summary>
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        // Private UI methods.
        /// <summary>
        /// Select next available source group and start streaming from it.
        /// </summary>
        void NextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    private:
        // Private methods.
        /// <summary>
        /// Disable the Next button while we switch to the next eligible media source.
        /// </summary>
        concurrency::task<void> PickNextMediaSourceAsync();

        /// <summary>
        /// Switch to the next eligible media source.
        /// </summary>
        concurrency::task<void> PickNextMediaSourceWorkerAsync();

        /// <summary>
        /// Stop streaming from all readers and dispose all readers and media capture object.
        /// </summary>
        concurrency::task<void> CleanupMediaCaptureAsync();

        /// <summary>
        /// Creates a reader for the frame source described by the MediaFrameSourceInfo.
        /// On frame arrived will render the output to the supplied software bitmap source.
        /// </summary>
        concurrency::task<void> CreateReaderAsync(
            Windows::Media::Capture::Frames::MediaFrameSourceGroup^ group,
            Windows::Media::Capture::Frames::MediaFrameSourceInfo^ info);

        /// <summary>
        /// Returns the first MediaFrameSource of matching kind in the group.
        /// </summary>
        Windows::Media::Capture::Frames::MediaFrameSourceInfo^ GetFirstSourceInfoOfKind(
            Windows::Media::Capture::Frames::MediaFrameSourceGroup^ group,
            Windows::Media::Capture::Frames::MediaFrameSourceKind kind);

        /// <summary>
        /// Initialize the media capture object.
        /// Must be called from the UI thread.
        /// </summary>
        concurrency::task<bool> TryInitializeMediaCaptureAsync(
            Windows::Media::Capture::Frames::MediaFrameSourceGroup^ group);

        /// <summary>
        /// Handler for frames which arrive from the MediaFrameReader.
        /// </summary>
        void FrameReader_FrameArrived(
            Windows::Media::Capture::Frames::MediaFrameReader^ sender,
            Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^ args);

    private:
        // Private data.
        MainPage^ rootPage = MainPage::Current;
        
        // The currently selected source group.
        int m_selectedSourceGroupIndex = 0;

        Platform::Agile<Windows::Media::Capture::MediaCapture> m_mediaCapture;

        std::vector<std::pair<Windows::Media::Capture::Frames::MediaFrameReader^, Windows::Foundation::EventRegistrationToken>> m_readers;

        std::map<Windows::Media::Capture::Frames::MediaFrameSourceKind, FrameRenderer^> m_frameRenderers;
        
        SimpleLogger^ m_logger;
    };
} // SDKTemplate