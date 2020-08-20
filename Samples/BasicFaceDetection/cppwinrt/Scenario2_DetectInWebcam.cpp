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
#include "Scenario2_DetectInWebcam.h"
#include "Scenario2_DetectInWebcam.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::FaceAnalysis;
using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_DetectInWebcam::Scenario2_DetectInWebcam()
    {
        InitializeComponent();
    }

    void Scenario2_DetectInWebcam::OnNavigatedTo(NavigationEventArgs const&)
    {
        suspendingEventToken = Application::Current().Suspending({ get_weak(), &Scenario2_DetectInWebcam::OnSuspending });
    }

    void Scenario2_DetectInWebcam::OnNavigatedFrom(NavigationEventArgs const&)
    {
        Application::Current().Suspending(suspendingEventToken);
    }

    // Responds to App Suspend event to stop/release MediaCapture object if it's running and return to Idle state.
    fire_and_forget Scenario2_DetectInWebcam::OnSuspending(IInspectable const&, SuspendingEventArgs const& e)
    {
        auto lifetime = get_strong();

        if (currentState == ScenarioState::Streaming)
        {
            auto deferral = e.SuspendingOperation().GetDeferral();
            co_await ChangeScenarioStateAsync(ScenarioState::Idle);
            deferral.Complete();
        }
    }

    // Creates the FaceDetector object which we will use for face detection.
    // Initializes a new MediaCapture instance and starts the Preview streaming to the CamPreview UI element.
    // Completes with true if initialization and streaming were successful and false if an exception occurred.
    IAsyncOperation<bool> Scenario2_DetectInWebcam::StartWebcamStreamingAsync()
    {
        auto lifetime = get_strong();

        bool successful;

        faceDetector = co_await FaceDetector::CreateAsync();
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
            mediaCapture.CameraStreamStateChanged({ get_weak(), &Scenario2_DetectInWebcam::MediaCapture_CameraStreamStateChanged });

            // Cache the media properties as we'll need them later.
            auto deviceController = mediaCapture.VideoDeviceController();
            videoProperties = deviceController.GetMediaStreamProperties(MediaStreamType::VideoPreview).as<VideoEncodingProperties>();

            // Immediately start streaming to our CaptureElement UI.
            // NOTE: CaptureElement's Source must be set before streaming is started.
            CamPreview().Source(mediaCapture);
            co_await mediaCapture.StartPreviewAsync();

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
    IAsyncAction Scenario2_DetectInWebcam::ShutdownWebcamAsync()
    {
        auto lifetime = get_strong();

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

        CamPreview().Source(nullptr);
        agileMediaCapture = nullptr;
    }

    // Captures a single frame from the running webcam stream and executes the FaceDetector on the image. If successful calls SetupVisualization to display the results.
    IAsyncOperation<bool> Scenario2_DetectInWebcam::TakeSnapshotAndFindFacesAsync()
    {
        auto lifetime = get_strong();

        if (currentState != ScenarioState::Streaming)
        {
            co_return false;
        }

        bool successful = false;

        try
        {
            // Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
            // GetPreviewFrame will convert the native webcam frame into this format.
            static constexpr BitmapPixelFormat InputPixelFormat = BitmapPixelFormat::Nv12;
            VideoFrame previewFrame(InputPixelFormat, (int)videoProperties.Width(), (int)videoProperties.Height());

            co_await agileMediaCapture.get().GetPreviewFrameAsync(previewFrame);

            // The returned VideoFrame should be in the supported NV12 format but we need to verify 
            if (FaceDetector::IsBitmapPixelFormatSupported(previewFrame.SoftwareBitmap().BitmapPixelFormat()))
            {
                IVector<DetectedFace> faces = co_await faceDetector.DetectFacesAsync(previewFrame.SoftwareBitmap());

                // Create a WritableBitmap for our visualization display; copy the original bitmap pixels to wb's buffer.
                // Note that WriteableBitmap doesn't support NV12 and we have to convert it to 32-bit BGRA.
                SoftwareBitmap convertedSource = SoftwareBitmap::Convert(previewFrame.SoftwareBitmap(), BitmapPixelFormat::Bgra8);
                WriteableBitmap displaySource(convertedSource.PixelWidth(), convertedSource.PixelHeight());
                convertedSource.CopyToBuffer(displaySource.PixelBuffer());

                // Create our display using the available image and face results.
                SetupVisualization(displaySource, faces);

                successful = true;
            }
            else
            {
                rootPage.NotifyUser(L"PixelFormat 'NV12' is not supported by FaceDetector", NotifyType::ErrorMessage);
            }
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
        }

        co_return successful;
    }

    // Takes the webcam image and FaceTracker results and assembles the visualization onto the Canvas.
    void Scenario2_DetectInWebcam::SetupVisualization(WriteableBitmap const& displaySource, IVector<DetectedFace> const& foundFaces)
    {
        ImageBrush brush;
        brush.ImageSource(displaySource);
        brush.Stretch(Stretch::Fill);
        SnapshotCanvas().Background(brush);

        SampleHelpers::HighlightFaces(displaySource, foundFaces, SnapshotCanvas(), HighlightedFaceBox());
    }

    // Manages the scenario's internal state. Invokes the internal methods and updates the UI according to the
    // passed in state value. Handles failures and resets the state if necessary.
    IAsyncAction Scenario2_DetectInWebcam::ChangeScenarioStateAsync(ScenarioState newState)
    {
        auto lifetime = get_strong();

        switch (newState)
        {
        case ScenarioState::Idle:

            CameraSnapshotButton().IsEnabled(false);
            currentState = newState;
            co_await ShutdownWebcamAsync();

            SnapshotCanvas().Background(nullptr);
            SnapshotCanvas().Children().Clear();
            CameraStreamingButton().Content(box_value(L"Start Streaming"));
            CameraSnapshotButton().Content(box_value(L"Take Snapshot"));
            break;

        case ScenarioState::Streaming:

            if (!co_await StartWebcamStreamingAsync())
            {
                co_await ChangeScenarioStateAsync(ScenarioState::Idle);
                break;
            }

            SnapshotCanvas().Background(nullptr);
            SnapshotCanvas().Children().Clear();
            CameraSnapshotButton().IsEnabled(true);
            CameraStreamingButton().Content(box_value(L"Stop Streaming"));
            CameraSnapshotButton().Content(box_value(L"Take Snapshot"));
            currentState = newState;
            break;

        case ScenarioState::Snapshot:

            if (!co_await TakeSnapshotAndFindFacesAsync())
            {
                co_await ChangeScenarioStateAsync(ScenarioState::Idle);
                break;
            }

            co_await ShutdownWebcamAsync();

            CameraSnapshotButton().IsEnabled(true);
            CameraStreamingButton().Content(box_value(L"Start Streaming"));
            CameraSnapshotButton().Content(box_value(L"Clear Display"));
            currentState = newState;
            break;
        }
    }

    // Handles MediaCapture changes by shutting down streaming and returning to Idle state.
    fire_and_forget Scenario2_DetectInWebcam::MediaCapture_CameraStreamStateChanged(MediaCapture const&, IInspectable const& args)
    {
        auto lifetime = get_strong();

        // MediaCapture is not Agile and so we cannot invoke it's methods on this caller's thread
        // and instead need to schedule the state change on the UI thread.
        co_await resume_foreground(Dispatcher());
        co_await ChangeScenarioStateAsync(ScenarioState::Idle);
    }

    // Handles "streaming" button clicks to start/stop webcam streaming.
    fire_and_forget Scenario2_DetectInWebcam::CameraStreamingButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
        if (currentState == ScenarioState::Streaming)
        {
            co_await ChangeScenarioStateAsync(ScenarioState::Idle);
        }
        else
        {
            co_await ChangeScenarioStateAsync(ScenarioState::Streaming);
        }
    }

    // Handles "snapshot" button clicks to take a snapshot or clear the current display.
    fire_and_forget Scenario2_DetectInWebcam::CameraSnapshotButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
        if (currentState == ScenarioState::Streaming)
        {
            co_await ChangeScenarioStateAsync(ScenarioState::Snapshot);
        }
        else
        {
            co_await ChangeScenarioStateAsync(ScenarioState::Idle);
        }
    }

    // Updates any existing face bounding boxes in response to changes in the size of the Canvas.
    void Scenario2_DetectInWebcam::SnapshotCanvas_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        // If the Canvas is resized we must recompute a new scaling factor and
        // apply it to each face box.
        ImageBrush brush = SnapshotCanvas().Background().try_as<ImageBrush>();
        if (brush != nullptr)
        {
            WriteableBitmap displaySource = brush.ImageSource().as<WriteableBitmap>();
            SampleHelpers::RepositionFaces(displaySource, SnapshotCanvas());
        }
    }
}
