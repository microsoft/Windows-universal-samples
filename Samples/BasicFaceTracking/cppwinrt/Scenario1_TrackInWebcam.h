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

#include "Scenario1_TrackInWebcam.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_TrackInWebcam : Scenario1_TrackInWebcamT<Scenario1_TrackInWebcam>
    {
        Scenario1_TrackInWebcam();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget CameraStreamingButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        // Values for identifying and controlling scenario states.
        enum class ScenarioState
        {
            /// Display is blank - default state.
            Idle,

            /// Webcam is actively engaged and a live video stream is displayed.
            Streaming,
        };

    private:
        /// Reference back to the "root" page of the app.
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        /// Holds the current scenario state value.
        ScenarioState currentState = ScenarioState::Idle;

        // References a MediaCapture instance; is null when not in Streaming state.
        agile_ref<Windows::Media::Capture::MediaCapture> agileMediaCapture{ nullptr };

        // Cache of properties from the current MediaCapture device which is used for capturing the preview frame.
        Windows::Media::MediaProperties::VideoEncodingProperties videoProperties{ nullptr };

        // References a FaceTracker instance.
        Windows::Media::FaceAnalysis::FaceTracker faceTracker{ nullptr };

        // A periodic timer to execute FaceTracker on preview frames
        Windows::System::Threading::ThreadPoolTimer frameProcessingTimer{ nullptr };

        // Flag to ensure FaceTracking logic only executes one at a time
        std::atomic<bool> busy{ false };

        // Event registration tokens.
        event_token suspendingEventToken;

    private:
        fire_and_forget OnSuspending(Windows::Foundation::IInspectable const& sender, Windows::ApplicationModel::SuspendingEventArgs const& e);
        Windows::Foundation::IAsyncOperation<bool> StartWebcamStreamingAsync();
        Windows::Foundation::IAsyncAction ShutdownWebcamAsync();
        Windows::Foundation::IAsyncAction ProcessCurrentVideoFrameAsync();
        fire_and_forget ProcessCurrentVideoFrame(Windows::System::Threading::ThreadPoolTimer const& timer);
        void SetupVisualization(Windows::Foundation::Size framePixelSize, Windows::Foundation::Collections::IVector<Windows::Media::FaceAnalysis::DetectedFace> const& foundFaces);
        Windows::Foundation::IAsyncAction ChangeScenarioStateAsync(ScenarioState newState);
        fire_and_forget AbandonStreaming();
        void MediaCapture_CameraStreamFailed(Windows::Media::Capture::MediaCapture const& sender, Windows::Foundation::IInspectable const& args);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_TrackInWebcam : Scenario1_TrackInWebcamT<Scenario1_TrackInWebcam, implementation::Scenario1_TrackInWebcam>
    {
    };
}
