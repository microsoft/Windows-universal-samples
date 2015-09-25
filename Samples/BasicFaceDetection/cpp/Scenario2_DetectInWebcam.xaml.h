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
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Values for identifying and controlling scenario states.
    /// </summary>
    public enum class ScenarioState
    {
        /// <summary>
        /// Display is blank - default state.
        /// </summary>
        Idle,

        /// <summary>
        /// Webcam is actively engaged and a live video stream is displayed.
        /// </summary>
        Streaming,

        /// <summary>
        /// Snapshot image has been captured and is being displayed along with detected faces; webcam is not active.
        /// </summary>
        Snapshot
    };

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DetectFacesInWebcam sealed
    {
    public:
        DetectFacesInWebcam();

    protected:

        /// <summary>
        /// Responds when we navigate to this page.
        /// </summary>
        /// <param name="e">Event data</param>
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:

        /// <summary>
        /// Brush for drawing the bounding box around each detected face.
        /// </summary>
        Windows::UI::Xaml::Media::SolidColorBrush^ lineBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Yellow);

        /// <summary>
        /// Thickness of the face bounding box lines.
        /// </summary>
        const double lineThickness = 2.0;

        /// <summary>
        /// Transparent fill for the bounding box.
        /// </summary>
        Windows::UI::Xaml::Media::SolidColorBrush^ fillBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Transparent);

        /// <summary>
        /// Reference back to the "root" page of the app.
        /// </summary>
        SDKTemplate::MainPage^ rootPage;

        /// <summary>
        /// Holds the current scenario state value.
        /// </summary>
        ScenarioState currentState;

        /// <summary>
        /// References a MediaCapture instance; is null when not in Streaming state.
        /// </summary>
        Platform::Agile<Windows::Media::Capture::MediaCapture^> mediaCapture;

        /// <summary>
        /// Cache of properties from the current MediaCapture device which is used for capturing the preview frame.
        /// </summary>
        Windows::Media::MediaProperties::VideoEncodingProperties^ videoProperties;

        /// References a FaceDetector instance.
        /// </summary>
        Windows::Media::FaceAnalysis::FaceDetector^ faceDetector;

        /// <summary>
        /// Responds to App Suspend event to stop/release MediaCapture object if it's running and return to Idle state.
        /// </summary>
        /// <param name="sender">The source of the Suspending event</param>
        /// <param name="e">Event data</param>
        void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);

        /// <summary>
        /// Initializes a new MediaCapture instance and starts the Preview streaming to the CamPreview UI element.
        /// </summary>
        /// <returns>Async Task object returning true if initialization and streaming were successful and false if an exception occurred.</returns>
        Concurrency::task<bool> StartWebcamStreaming();

        /// <summary>
        /// Safely stops webcam streaming (if running) and releases MediaCapture object.
        /// </summary>
        void ShutdownWebCam();

        /// <summary>
        /// Captures a single frame from the running webcam stream and executes the FaceDetector on the image. If successful calls SetupVisualization to display the results.
        /// </summary>
        /// <returns>Async Task object returning true if the capture was successful and false if an exception occurred.</returns>
        Concurrency::task<bool> TakeSnapshotAndFindFaces();

        /// <summary>
        /// Takes the webcam image and FaceDetector results and assembles the visualization onto the Canvas.
        /// </summary>
        /// <param name="displaySource">Bitmap object holding the image we're going to display</param>
        /// <param name="foundFaces">List of detected faces; output from FaceDetector</param>
        void SetupVisualization(
            Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ displaySource,
            Windows::Foundation::Collections::IVector<Windows::Media::FaceAnalysis::DetectedFace^>^ foundFaces);

        /// <summary>
        /// Manages the scenario's internal state. Invokes the internal methods and updates the UI according to the
        /// passed in state value. Handles failures and resets the state if necessary.
        /// </summary>
        /// <param name="newState">State to switch to</param>
        void ChangeScenarioState(ScenarioState newState);

        /// <summary>
        /// Handles MediaCapture changes by shutting down streaming and returning to Idle state.
        /// </summary>
        /// <param name="sender">The source of the event, i.e. our MediaCapture object</param>
        /// <param name="args">Event data</param>
        void MediaCapture_CameraStreamStateChanged(Windows::Media::Capture::MediaCapture^ sender, Platform::Object^ args);

        /// <summary>
        /// Handles "streaming" button clicks to start/stop webcam streaming.
        /// </summary>
        /// <param name="sender">Button user clicked</param>
        /// <param name="e">Event data</param>
        void CameraStreamingButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /// <summary>
        /// Handles "snapshot" button clicks to take a snapshot or clear the current display.
        /// </summary>
        /// <param name="sender">Button user clicked</param>
        /// <param name="e">Event data</param>
        void CameraSnapshotButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /// <summary>
        /// Updates any existing face bounding boxes in response to changes in the size of the Canvas.
        /// </summary>
        /// <param name="sender">Canvas whose size has changed</param>
        /// <param name="e">Event data</param>
        void SnapshotCanvas_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
