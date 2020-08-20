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

#include "Scenario2_DetectInWebcam.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_DetectInWebcam : Scenario2_DetectInWebcamT<Scenario2_DetectInWebcam>
    {
        Scenario2_DetectInWebcam();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget CameraStreamingButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget CameraSnapshotButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SnapshotCanvas_SizeChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::SizeChangedEventArgs const& e);

    private:
        // Values for identifying and controlling scenario states.
        enum ScenarioState
        {
            // Display is blank - default state.
            Idle,

            // Webcam is actively engaged and a live video stream is displayed.
            Streaming,

            // Snapshot image has been captured and is being displayed along with detected faces; webcam is not active.
            Snapshot,
        };

    private:
        // Reference back to the "root" page of the app.
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        // Holds the current scenario state value.
        ScenarioState currentState = ScenarioState::Idle;

        // References a MediaCapture instance; is null when not in Streaming state.
        agile_ref<Windows::Media::Capture::MediaCapture> agileMediaCapture{ nullptr };

        // Cache of properties from the current MediaCapture device which is used for capturing the preview frame.
        Windows::Media::MediaProperties::VideoEncodingProperties videoProperties{ nullptr };

        // References a FaceDetector instance.
        Windows::Media::FaceAnalysis::FaceDetector faceDetector{ nullptr };

        // Event registration tokens.
        event_token suspendingEventToken;

    private:
        fire_and_forget OnSuspending(Windows::Foundation::IInspectable const& sender, Windows::ApplicationModel::SuspendingEventArgs const& e);
        Windows::Foundation::IAsyncOperation<bool> StartWebcamStreamingAsync();
        Windows::Foundation::IAsyncAction ShutdownWebcamAsync();
        Windows::Foundation::IAsyncOperation<bool> TakeSnapshotAndFindFacesAsync();
        void SetupVisualization(Windows::UI::Xaml::Media::Imaging::WriteableBitmap const& displaySource, Windows::Foundation::Collections::IVector<Windows::Media::FaceAnalysis::DetectedFace> const& foundFaces);
        Windows::Foundation::IAsyncAction ChangeScenarioStateAsync(ScenarioState newState);
        fire_and_forget AbandonStreaming();
        fire_and_forget MediaCapture_CameraStreamStateChanged(Windows::Media::Capture::MediaCapture const& sender, Windows::Foundation::IInspectable const& args);

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_DetectInWebcam : Scenario2_DetectInWebcamT<Scenario2_DetectInWebcam, implementation::Scenario2_DetectInWebcam>
    {
    };
}
