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
#include "Scenario1_TrackInWebcam.h"
#include "Scenario1_TrackInWebcam.g.cpp"
#include "SampleConfiguration.h"

using namespace std::literals::chrono_literals;
using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media;
using namespace Windows::Media::Capture;
using namespace Windows::Media::FaceAnalysis;
using namespace Windows::Media::MediaProperties;
using namespace Windows::System::Threading;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_TrackInWebcam::Scenario1_TrackInWebcam()
    {
        InitializeComponent();
    }

    void Scenario1_TrackInWebcam::OnNavigatedTo(NavigationEventArgs const&)
    {
        suspendingEventToken = Application::Current().Suspending({ get_weak(), &Scenario1_TrackInWebcam::OnSuspending });
    }

    void Scenario1_TrackInWebcam::OnNavigatedFrom(NavigationEventArgs const&)
    {
        Application::Current().Suspending(suspendingEventToken);
    }

    // Responds to App Suspend event to stop/release MediaCapture object if it's running and return to Idle state.
    fire_and_forget Scenario1_TrackInWebcam::OnSuspending(IInspectable const&, SuspendingEventArgs const& e)
    {
        auto lifetime = get_strong();

        if (currentState == ScenarioState::Streaming)
        {
            auto deferral = e.SuspendingOperation().GetDeferral();
            co_await ChangeScenarioStateAsync(ScenarioState::Idle);
            deferral.Complete();
        }
    }

    // Creates the FaceTracker object which we will use for face detection and tracking.
    // Initializes a new MediaCapture instance and starts the Preview streaming to the CamPreview UI element.
    // Completes with true if initialization and streaming were successful and false if an exception occurred.
    IAsyncOperation<bool> Scenario1_TrackInWebcam::StartWebcamStreamingAsync()
    {
        auto lifetime = get_strong();

        bool successful;

        faceTracker = co_await FaceTracker::CreateAsync();
        try
        {
            MediaCapture mediaCapture;
            agileMediaCapture = mediaCapture;
            MediaCaptureInitializationSettings settings;

            // For this scenario, we only need Video (not microphone) so specify this in the initializer.
            // NOTE: the appxmanifest only declares "webcam" under capabilities and if this is changed to include
            // microphone (default constructor) you must add "microphone" to the manifest or initialization will fail.
            settings.StreamingCaptureMode(StreamingCaptureMode::Video);

            co_await mediaCapture.InitializeAsync(settings);
            mediaCapture.Failed({ get_weak(), &Scenario1_TrackInWebcam::MediaCapture_CameraStreamFailed });

            // Cache the media properties as we'll need them later.
            auto deviceController = mediaCapture.VideoDeviceController();
            videoProperties = deviceController.GetMediaStreamProperties(MediaStreamType::VideoPreview).as<VideoEncodingProperties>();

            // Immediately start streaming to our CaptureElement UI.
            // NOTE: CaptureElement's Source must be set before streaming is started.
            CamPreview().Source(mediaCapture);
            co_await mediaCapture.StartPreviewAsync();

            // Run the timer at 15 frames per second.
            TimeSpan timerInterval = TimeSpan{ 1s } / 15;
            frameProcessingTimer = ThreadPoolTimer::CreatePeriodicTimer({ get_weak(), &Scenario1_TrackInWebcam::ProcessCurrentVideoFrame }, timerInterval);

            successful = true;
        }
        catch (hresult_access_denied const&)
        {
            // If the user has disabled their webcam this exception is thrown; provide a descriptive message to inform the user of this fact.
            rootPage.NotifyUser(L"Webcam is disabled or access to the webcam is disabled for this app.\nEnsure Privacy Settings allow webcam usage.", NotifyType::ErrorMessage);
            successful = false;
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
            successful = false;
        }

        co_return successful;
    }

    // Safely stops webcam streaming (if running) and releases MediaCapture object.
    IAsyncAction Scenario1_TrackInWebcam::ShutdownWebcamAsync()
    {
        auto lifetime = get_strong();

        if (frameProcessingTimer != nullptr)
        {
            frameProcessingTimer.Cancel();
        }

        if (agileMediaCapture)
        {
            auto mediaCapture = agileMediaCapture.get();
            if (mediaCapture.CameraStreamState() == Windows::Media::Devices::CameraStreamState::Streaming)
            {
                try
                {
                    co_await mediaCapture.StopPreviewAsync();
                }
                catch (hresult_error const&)
                {
                    // Since we're going to destroy the MediaCapture object there's nothing to do here
                }
            }
            mediaCapture.Close();
        }

        frameProcessingTimer = nullptr;
        CamPreview().Source(nullptr);
        agileMediaCapture = nullptr;
        CameraStreamingButton().IsEnabled(true);
    }

    // This method is invoked by a ThreadPoolTimer to execute the FaceTracker and Visualization logic at approximately 15 frames per second.
    // Keep in mind this method is called from a Timer and not synchronized with the camera stream. Also, the processing time of FaceTracker
    // will vary depending on the size of each frame and the number of faces being tracked. That is, a large image with several tracked faces may
    // take longer to process.
    fire_and_forget Scenario1_TrackInWebcam::ProcessCurrentVideoFrame(ThreadPoolTimer const&)
    {
        auto lifetime = get_strong();

        if (currentState != ScenarioState::Streaming)
        {
            return;
        }

        // If already busy, it means we're still waiting for processing work on the previous frame to complete.
        // In this situation, exit immediately.
        bool expected = false;
        if (!busy.compare_exchange_strong(expected, true))
        {
            return;
        }

        co_await ProcessCurrentVideoFrameAsync();
        busy.store(false);
    }

    IAsyncAction Scenario1_TrackInWebcam::ProcessCurrentVideoFrameAsync()
    {
        auto lifetime = get_strong();

        // Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
        // GetPreviewFrame will convert the native webcam frame into this format.
        static constexpr BitmapPixelFormat InputPixelFormat = BitmapPixelFormat::Nv12;
        VideoFrame previewFrame(InputPixelFormat, (int)videoProperties.Width(), (int)videoProperties.Height());

        try
        {
            co_await agileMediaCapture.get().GetPreviewFrameAsync(previewFrame);
        }
        catch (hresult_access_denied const&)
        {
            // Lost access to the camera.
            AbandonStreaming();
            co_return;
        }
        catch (hresult_error const&)
        {
            rootPage.NotifyUser(L"PreviewFrame with format '" + to_hstring(InputPixelFormat) + L"' is not supported by your Webcam", NotifyType::ErrorMessage);
            co_return;
        }

        // The returned VideoFrame should be in the supported NV12 format but we need to verify this.
        if (!FaceDetector::IsBitmapPixelFormatSupported(previewFrame.SoftwareBitmap().BitmapPixelFormat()))
        {
            rootPage.NotifyUser(L"PixelFormat '" + to_hstring(previewFrame.SoftwareBitmap().BitmapPixelFormat()) + L"' is not supported by FaceDetector", NotifyType::ErrorMessage);
        }

        IVector<DetectedFace> faces;
        try
        {
            faces = co_await faceTracker.ProcessNextFrameAsync(previewFrame);
        }
        catch (hresult_error const& e)
        {
            rootPage.NotifyUser(e.message(), NotifyType::ErrorMessage);
            co_return;
        }

        // Create our visualization using the frame dimensions and face results but run it on the UI thread.
        Size previewFrameSize(
            static_cast<float>(previewFrame.SoftwareBitmap().PixelWidth()),
            static_cast<float>(previewFrame.SoftwareBitmap().PixelHeight()));
        co_await resume_foreground(Dispatcher());
        SetupVisualization(previewFrameSize, faces);
    }

    // Takes the webcam image and FaceTracker results and assembles the visualization onto the Canvas.
    void Scenario1_TrackInWebcam::SetupVisualization(Size framePixelSize, IVector<DetectedFace> const& foundFaces)
    {
        VisualizationCanvas().Children().Clear();

        if (currentState == ScenarioState::Streaming &&
            framePixelSize.Width != 0.0 && framePixelSize.Height != 0.0)
        {
            double widthScale = VisualizationCanvas().ActualWidth() / framePixelSize.Width;
            double heightScale = VisualizationCanvas().ActualHeight() / framePixelSize.Height;

            for (DetectedFace face : foundFaces)
            {
                // Create a rectangle element for displaying the face box but since we're using a Canvas
                // we must scale the rectangles according to the frames's actual size.
                BitmapBounds faceBox = face.FaceBox();
                Rectangle box;
                box.Width(faceBox.Width * widthScale);
                box.Height(faceBox.Height * heightScale);
                box.Margin({ faceBox.X * widthScale, faceBox.Y * heightScale, 0, 0 });
                box.Style(HighlightedFaceBoxStyle());
                VisualizationCanvas().Children().Append(box);
            }
        }
    }

    IAsyncAction Scenario1_TrackInWebcam::ChangeScenarioStateAsync(ScenarioState newState)
    {
        auto lifetime = get_strong();

        // Disable UI while state change is in progress
        CameraStreamingButton().IsEnabled(false);

        switch (newState)
        {
        case ScenarioState::Idle:

            currentState = newState;
            co_await ShutdownWebcamAsync();

            VisualizationCanvas().Children().Clear();
            CameraStreamingButton().Content(box_value(L"Start Streaming"));
            break;

        case ScenarioState::Streaming:

            if (!co_await StartWebcamStreamingAsync())
            {
                co_await ChangeScenarioStateAsync(ScenarioState::Idle);
                break;
            }

            VisualizationCanvas().Children().Clear();
            CameraStreamingButton().Content(box_value(L"Stop Streaming"));
            currentState = newState;
            CameraStreamingButton().IsEnabled(true);
            break;
        }
    }

    fire_and_forget Scenario1_TrackInWebcam::AbandonStreaming()
    {
        auto lifetime = get_strong();

        // MediaCapture is not Agile and so we cannot invoke its methods on this caller's thread
        // and instead need to schedule the state change on the UI thread.
        co_await resume_foreground(Dispatcher());
        co_await ChangeScenarioStateAsync(ScenarioState::Idle);
    }

    void Scenario1_TrackInWebcam::MediaCapture_CameraStreamFailed(MediaCapture const&, IInspectable const&)
    {
        AbandonStreaming();
    }

    // Handles "streaming" button clicks to start/stop webcam streaming.
    fire_and_forget Scenario1_TrackInWebcam::CameraStreamingButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser({}, NotifyType::StatusMessage);
        if (currentState == ScenarioState::Streaming)
        {
            co_await ChangeScenarioStateAsync(ScenarioState::Idle);
        }
        else
        {
            co_await ChangeScenarioStateAsync(ScenarioState::Streaming);
        }
    }
}
