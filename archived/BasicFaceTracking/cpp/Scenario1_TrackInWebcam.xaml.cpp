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
#include "Scenario1_TrackInWebcam.xaml.h"

using namespace SDKTemplate;

using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media;
using namespace Windows::Media::Capture;
using namespace Windows::Media::FaceAnalysis;
using namespace Windows::Media::MediaProperties;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;


TrackFacesInWebcam::TrackFacesInWebcam() :
    rootPage(MainPage::Current),
    currentState(ScenarioState::Idle),
    frameProcessingSemaphore(nullptr)
{
    InitializeComponent();

    App::Current->Suspending += ref new SuspendingEventHandler(this, &TrackFacesInWebcam::OnSuspending);

    frameProcessingSemaphore = CreateSemaphoreEx(nullptr, 1, 1, nullptr, 0, SYNCHRONIZE | SEMAPHORE_MODIFY_STATE);
    if (frameProcessingSemaphore == nullptr)
    {
        throw ref new Platform::Exception(HRESULT_FROM_WIN32(GetLastError()), "Failed to create frameProcessingSemaphore");
    }

    concurrency::create_task(FaceTracker::CreateAsync())
        .then([this](FaceTracker^ tracker)
    {
        this->faceTracker = tracker;
    });
}

TrackFacesInWebcam::~TrackFacesInWebcam()
{
    if (frameProcessingSemaphore != nullptr)
    {
        CloseHandle(frameProcessingSemaphore);
        frameProcessingSemaphore = nullptr;
    }
}

void TrackFacesInWebcam::OnNavigatedTo(NavigationEventArgs^ e)
{
    this->rootPage = MainPage::Current;
}

void TrackFacesInWebcam::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ e)
{
    if (this->currentState == ScenarioState::Streaming)
    {
        auto deferral = e->SuspendingOperation->GetDeferral();
        try
        {
            ChangeScenarioState(ScenarioState::Idle);
        }
        catch (...)
        {
            deferral->Complete();
            throw;
        }
    }
}

Concurrency::task<bool> TrackFacesInWebcam::StartWebcamStreaming()
{
    this->mediaCapture = ref new MediaCapture();

    // For this scenario, we only need Video (not microphone) so specify this in the initializer.
    // NOTE: the appxmanifest only declares "webcam" under capabilities and if this is changed to include
    // microphone (default constructor) you must add "microphone" to the manifest or initialization will fail.
    MediaCaptureInitializationSettings^ settings = ref new MediaCaptureInitializationSettings();
    settings->StreamingCaptureMode = StreamingCaptureMode::Video;

    // Start a task chain to initialize MediaCapture and start streaming. A "final" task is
    // linked in to handle exceptions and return success/failure back to the caller.
    return concurrency::create_task(this->mediaCapture->InitializeAsync(settings))
        .then([this]()
    {
        this->mediaCapture->Failed += ref new Windows::Media::Capture::MediaCaptureFailedEventHandler(this, &TrackFacesInWebcam::MediaCapture_CameraStreamFailed);

        // Cache the media properties as we'll need them later.
        auto deviceController = this->mediaCapture->VideoDeviceController;
        this->videoProperties = dynamic_cast<VideoEncodingProperties^>(deviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview));

        // Immediately start streaming to our CaptureElement UI.
        // NOTE: CaptureElement's Source must be set before streaming is started.
        this->CamPreview->Source = this->mediaCapture.Get();
        return concurrency::create_task(this->mediaCapture->StartPreviewAsync());

    }).then([this](concurrency::task<void> finalTask)
    {
        bool successful;

        try
        {
            finalTask.get();

            // Use a 66 millisecond interval for our timer, i.e. 15 frames per second.
            // NOTE: Timespan is measured in 100 nanosecond units
            TimeSpan timerInterval;
            timerInterval.Duration = 66 * 10000;
            frameProcessingTimer = Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(ref new Windows::System::Threading::TimerElapsedHandler(this, &TrackFacesInWebcam::ProcessCurrentVideoFrame), timerInterval);

            successful = true;
        }
        catch (Platform::AccessDeniedException^ ex)
        {
            // If the user has disabled their webcam this exception is thrown; provide a descriptive message to inform the user of this fact.
            this->rootPage->NotifyUser("Webcam is disabled or access to the webcam is disabled for this app.\nEnsure Privacy Settings allow webcam usage.", NotifyType::ErrorMessage);
            successful = false;
        }
        catch (Platform::Exception^ ex)
        {
            this->rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
            successful = false;
        }

        return successful;
    });
}

void TrackFacesInWebcam::ShutdownWebCam()
{
    if (frameProcessingTimer != nullptr)
    {
        frameProcessingTimer->Cancel();
    }
    frameProcessingTimer = nullptr;

    if (this->mediaCapture.Get() != nullptr)
    {
        if (this->mediaCapture->CameraStreamState == Windows::Media::Devices::CameraStreamState::Streaming)
        {

            // Attempt to stop preview screen and clean up MediaCapture object once Shutdown has completed.
            concurrency::create_task(this->mediaCapture->StopPreviewAsync())
                .then([this](concurrency::task<void> task)
            {
                try
                {
                    task.get();
                }
                catch (Platform::Exception^)
                {
                    ;   // Since we're going to destroy the MediaCapture object there's nothing to do here
                }

                this->mediaCapture = nullptr;
                this->CamPreview->Source = nullptr;
                this->CameraStreamingButton->IsEnabled = true;
            });
        }
        else
        {
            this->mediaCapture = nullptr;
            this->CamPreview->Source = nullptr;
            this->CameraStreamingButton->IsEnabled = true;
        }
    }
    else
    {
        this->CamPreview->Source = nullptr;
        this->CameraStreamingButton->IsEnabled = true;
    }
}

void TrackFacesInWebcam::ProcessCurrentVideoFrame(Windows::System::Threading::ThreadPoolTimer^ timer)
{
    if (this->currentState != ScenarioState::Streaming)
    {
        return;
    }

    // If the semaphore is nonsignaled it means we're still waiting for processing work on the previous frame to complete.
    // In this situation, don't wait on the semaphore but exit immediately.
    if(WaitForSingleObject(frameProcessingSemaphore, 0) != WAIT_OBJECT_0)
    {
        return;
    }

    // Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
    // GetPreviewFrame will convert the native webcam frame into this format.
    const BitmapPixelFormat InputPixelFormat = BitmapPixelFormat::Nv12;
    VideoFrame^ previewFrame = ref new VideoFrame(InputPixelFormat, this->videoProperties->Width, this->videoProperties->Height);

    concurrency::create_task(this->mediaCapture->GetPreviewFrameAsync(previewFrame))
        .then([this](VideoFrame^ previewFrame)
    {
        // The returned VideoFrame should be in the supported NV12 format but we need to verify this.
        if (FaceTracker::IsBitmapPixelFormatSupported(previewFrame->SoftwareBitmap->BitmapPixelFormat))
        {
            return concurrency::create_task(this->faceTracker->ProcessNextFrameAsync(previewFrame))
                .then([this, previewFrame](IVector<DetectedFace^>^ faces)
            {

                // Create our visualization using the frame dimensions and face results but run it on the UI thread.
                auto previewFrameSize = Windows::Foundation::Size(static_cast<float>(previewFrame->SoftwareBitmap->PixelWidth), static_cast<float>(previewFrame->SoftwareBitmap->PixelHeight));
                
                concurrency::create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                    ref new Windows::UI::Core::DispatchedHandler([this, previewFrameSize, faces]()
                {
                    SetupVisualization(previewFrameSize, faces);
                })));
            });
        }
        else
        {
            // We need to return a task<void> object from this lambda to continue the task chain,
            // and also the call to BitmapPixelFormat::ToString() must occur on the UI thread.
            return concurrency::create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
                ref new Windows::UI::Core::DispatchedHandler([this, previewFrame]()
            {
                this->rootPage->NotifyUser("PixelFormat '" + previewFrame->SoftwareBitmap->BitmapPixelFormat.ToString() + "' is not supported by FaceTracker", NotifyType::ErrorMessage);
            })));
        }

    }).then([this](concurrency::task<void> finalTask)
    {
        // Use a final continuation task to handle any exceptions that may have been thrown
        // during the task sequence.
        try
        {
            finalTask.get();
        }
        catch (Platform::Exception^ ex)
        {
            concurrency::create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
                ref new Windows::UI::Core::DispatchedHandler([this, ex]()
            {
                this->rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
            })));
        }

        // Finally release the semaphore.
        ReleaseSemaphore(frameProcessingSemaphore, 1, nullptr);
    });
}

void TrackFacesInWebcam::SetupVisualization(
    Windows::Foundation::Size framePizelSize,
    IVector<DetectedFace^>^ foundFaces)
{
    this->VisualizationCanvas->Children->Clear();

    double actualWidth = this->VisualizationCanvas->ActualWidth;
    double actualHeight = this->VisualizationCanvas->ActualHeight;

    if (this->currentState == ScenarioState::Streaming && foundFaces != nullptr && actualWidth != 0 && actualHeight != 0)
    {
        double widthScale = framePizelSize.Width / actualWidth;
        double heightScale = framePizelSize.Height / actualHeight;

        for each(DetectedFace^ face in foundFaces)
        {
            // Create a rectangle element for displaying the face box but since we're using a Canvas
            // we must scale the rectangles according to the frames's actual size.
            Rectangle^ box = ref new Rectangle();
            box->Tag = face->FaceBox;
            box->Width = face->FaceBox.Width / widthScale;
            box->Height = face->FaceBox.Height / heightScale;
            box->Fill = this->fillBrush;
            box->Stroke = this->lineBrush;
            box->StrokeThickness = this->lineThickness;
            box->Margin = Thickness(face->FaceBox.X / widthScale, face->FaceBox.Y / heightScale, 0, 0);

            this->VisualizationCanvas->Children->Append(box);
        }
    }
}

void TrackFacesInWebcam::ChangeScenarioState(ScenarioState newState)
{
    // Disable UI while state change is in progress
    this->CameraStreamingButton->IsEnabled = false;

    switch (newState)
    {
    case ScenarioState::Idle:

        ShutdownWebCam();

        this->VisualizationCanvas->Children->Clear();
        this->CameraStreamingButton->Content = "Start Streaming";
        this->currentState = newState;
        break;

    case ScenarioState::Streaming:

        concurrency::create_task(this->StartWebcamStreaming())
            .then([this, newState](concurrency::task<bool> task)
        {
            if (!task.get())
            {
                ChangeScenarioState(ScenarioState::Idle);
            }
            else
            {
                this->VisualizationCanvas->Children->Clear();
                this->CameraStreamingButton->Content = "Stop Streaming";
                this->currentState = newState;
            }

            this->CameraStreamingButton->IsEnabled = true;
        });
        break;
    }
}

void TrackFacesInWebcam::MediaCapture_CameraStreamFailed(MediaCapture^ sender, MediaCaptureFailedEventArgs^ errorEventArgs)
{
    // MediaCapture is not Agile and so we cannot invoke its methods on this caller's thread
    // and instead need to schedule the state change on the UI thread.
    concurrency::create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        ChangeScenarioState(ScenarioState::Idle);
    })));
}

void TrackFacesInWebcam::CameraStreamingButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (this->currentState == ScenarioState::Streaming)
    {
        this->rootPage->NotifyUser("", NotifyType::StatusMessage);
        ChangeScenarioState(ScenarioState::Idle);
    }
    else
    {
        this->rootPage->NotifyUser("", NotifyType::StatusMessage);
        ChangeScenarioState(ScenarioState::Streaming);
    }
}
