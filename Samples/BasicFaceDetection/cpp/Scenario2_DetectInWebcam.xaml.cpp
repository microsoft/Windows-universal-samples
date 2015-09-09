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
#include "Scenario2_DetectInWebcam.xaml.h"

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


DetectFacesInWebcam::DetectFacesInWebcam() :
    rootPage(MainPage::Current),
    currentState(ScenarioState::Idle)
{
    InitializeComponent();

    App::Current->Suspending += ref new SuspendingEventHandler(this, &DetectFacesInWebcam::OnSuspending);

    concurrency::create_task(FaceDetector::CreateAsync())
        .then([this](FaceDetector^ detector)
    {
        this->faceDetector = detector;
    });
}

void DetectFacesInWebcam::OnNavigatedTo(NavigationEventArgs^ e)
{
    this->rootPage = MainPage::Current;
}

void DetectFacesInWebcam::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ e)
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

Concurrency::task<bool> DetectFacesInWebcam::StartWebcamStreaming()
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
        this->mediaCapture->CameraStreamStateChanged += ref new TypedEventHandler<MediaCapture^, Platform::Object^>(this, &DetectFacesInWebcam::MediaCapture_CameraStreamStateChanged);

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

void DetectFacesInWebcam::ShutdownWebCam()
{
    if (this->mediaCapture.Get() != nullptr)
    {
        if (this->mediaCapture->CameraStreamState == Windows::Media::Devices::CameraStreamState::Streaming)
        {
            // Attempt to stop preview screen and clean up MediaCapture object once Shutdown has completed
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
            });
        }
        else
        {
            this->mediaCapture = nullptr;
            this->CamPreview->Source = nullptr;
        }
    }
    else
    {
        this->CamPreview->Source = nullptr;
    }
}

Concurrency::task<bool> DetectFacesInWebcam::TakeSnapshotAndFindFaces()
{
    // If we're not currently streaming, return false, but it must be
    // in the form of a task object.
    if (this->mediaCapture == nullptr || this->currentState != ScenarioState::Streaming)
    {
        return concurrency::create_task([]() { return false; });
    }

    // Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
    // GetPreviewFrame will convert the native webcam frame into this format.
    const BitmapPixelFormat InputPixelFormat = BitmapPixelFormat::Nv12;
    VideoFrame^ previewFrame = ref new VideoFrame(InputPixelFormat, this->videoProperties->Width, this->videoProperties->Height);

    return concurrency::create_task(this->mediaCapture->GetPreviewFrameAsync(previewFrame))
        .then([this](VideoFrame^ previewFrame)
    {
        // The returned VideoFrame should be in the supported NV12 format but we need to verify this.
        if (FaceDetector::IsBitmapPixelFormatSupported(previewFrame->SoftwareBitmap->BitmapPixelFormat))
        {
            return concurrency::create_task(this->faceDetector->DetectFacesAsync(previewFrame->SoftwareBitmap))
                .then([this, previewFrame](IVector<DetectedFace^>^ faces)
            {
                // Create a WritableBitmap for our visualization display; copy the original bitmap pixels to wb's buffer.
                // Note that WriteableBitmap doesn't support NV12 and we have to convert it to 32-bit BGRA.
                SoftwareBitmap^ convertedSource = SoftwareBitmap::Convert(previewFrame->SoftwareBitmap, BitmapPixelFormat::Bgra8);

                WriteableBitmap^ displaySource = ref new WriteableBitmap(convertedSource->PixelWidth, convertedSource->PixelHeight);
                convertedSource->CopyToBuffer(displaySource->PixelBuffer);

                // Create our display using the available image and face results.
                SetupVisualization(displaySource, faces);
            });
        }
        else
        {
            // We need to return a task<void> object from this lambda to continue the task chain,
            // and also the call to BitmapPixelFormat::ToString() must occur on the UI thread.
            return concurrency::create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
                ref new Windows::UI::Core::DispatchedHandler([this, previewFrame]()
            {
                this->rootPage->NotifyUser("PixelFormat '" + previewFrame->SoftwareBitmap->BitmapPixelFormat.ToString() + "' is not supported by FaceDetector", NotifyType::ErrorMessage);
            })));
        }

    }).then([this](concurrency::task<void> finalTask)
    {
        // Use a final continuation task to handle any exceptions that may have been thrown
        // during the task sequence.
        bool successful;
        try
        {
            finalTask.get();
            successful = true;
        }
        catch (Platform::Exception^ ex)
        {
            this->rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
            successful = false;
        }
        return successful;
    });
}

void DetectFacesInWebcam::SetupVisualization(
    WriteableBitmap^ displaySource,
    IVector<DetectedFace^>^ foundFaces)
{
    ImageBrush^ brush = ref new ImageBrush();
    brush->ImageSource = displaySource;
    brush->Stretch = Stretch::Fill;
    this->SnapshotCanvas->Background = brush;

    double actualWidth = this->SnapshotCanvas->ActualWidth;
    double actualHeight = this->SnapshotCanvas->ActualHeight;

    if (foundFaces != nullptr && actualWidth != 0 && actualHeight != 0)
    {
        double widthScale = displaySource->PixelWidth / actualWidth;
        double heightScale = displaySource->PixelHeight / actualHeight;

        for (DetectedFace^ face : foundFaces)
        {
            // Create a rectangle element for displaying the face box but since we're using a Canvas
            // we must scale the rectangles according to the image's actual size.
            // The original FaceBox values are saved in the Rectangle's Tag field so we can update the
            // boxes when the Canvas is resized.
            Rectangle^ box = ref new Rectangle();
            box->Tag = face->FaceBox;
            box->Width = face->FaceBox.Width / widthScale;
            box->Height = face->FaceBox.Height / heightScale;
            box->Fill = this->fillBrush;
            box->Stroke = this->lineBrush;
            box->StrokeThickness = this->lineThickness;
            box->Margin = Thickness(face->FaceBox.X / widthScale, face->FaceBox.Y / heightScale, 0, 0);

            this->SnapshotCanvas->Children->Append(box);
        }
    }

    Platform::String^ message;
    if (foundFaces == nullptr || foundFaces->Size == 0)
    {
        message = "Didn't find any human faces in the image";
    }
    else if (foundFaces->Size == 1)
    {
        message = "Found a human face in the image";
    }
    else
    {
        message = "Found " + foundFaces->Size + " human faces in the image";
    }

    this->rootPage->NotifyUser(message, NotifyType::StatusMessage);
}

void DetectFacesInWebcam::ChangeScenarioState(ScenarioState newState)
{
    switch (newState)
    {
    case ScenarioState::Idle:

        ShutdownWebCam();

        this->SnapshotCanvas->Background = nullptr;
        this->SnapshotCanvas->Children->Clear();
        this->CameraSnapshotButton->IsEnabled = false;
        this->CameraStreamingButton->Content = "Start Streaming";
        this->CameraSnapshotButton->Content = "Take Snapshot";
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
                this->SnapshotCanvas->Background = nullptr;
                this->SnapshotCanvas->Children->Clear();
                this->CameraSnapshotButton->IsEnabled = true;
                this->CameraStreamingButton->Content = "Stop Streaming";
                this->CameraSnapshotButton->Content = "Take Snapshot";
                this->currentState = newState;
            }
        });
        break;

    case ScenarioState::Snapshot:

        concurrency::create_task(this->TakeSnapshotAndFindFaces())
            .then([this, newState](concurrency::task<bool> task)
        {
            if (!task.get())
            {
                ChangeScenarioState(ScenarioState::Idle);
            }
            else
            {
                ShutdownWebCam();

                this->CameraSnapshotButton->IsEnabled = true;
                this->CameraStreamingButton->Content = "Start Streaming";
                this->CameraSnapshotButton->Content = "Clear Display";
                this->currentState = newState;
            }
        });
        break;
    }
}

void DetectFacesInWebcam::MediaCapture_CameraStreamStateChanged(MediaCapture^ sender, Platform::Object^ args)
{
    // MediaCapture is not Agile and so we cannot invoke it's methods on this caller's thread
    // and instead need to schedule the state change on the UI thread.
    concurrency::create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        ChangeScenarioState(ScenarioState::Idle);
    })));
}

void DetectFacesInWebcam::CameraStreamingButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
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

void DetectFacesInWebcam::CameraSnapshotButton_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (this->currentState == ScenarioState::Streaming)
    {
        this->rootPage->NotifyUser("", NotifyType::StatusMessage);
        ChangeScenarioState(ScenarioState::Snapshot);
    }
    else
    {
        this->rootPage->NotifyUser("", NotifyType::StatusMessage);
        ChangeScenarioState(ScenarioState::Idle);
    }
}

void DetectFacesInWebcam::SnapshotCanvas_SizeChanged(Platform::Object^ sender, RoutedEventArgs^ e)
{
    try
    {
        double actualWidth = this->SnapshotCanvas->ActualWidth;
        double actualHeight = this->SnapshotCanvas->ActualHeight;

        // If the Canvas is resized we must recompute a new scaling factor and
        // apply it to each face box.
        if (this->currentState == ScenarioState::Snapshot && this->SnapshotCanvas->Background != nullptr && actualWidth != 0 && actualHeight != 0)
        {
            WriteableBitmap^ displaySource = dynamic_cast<WriteableBitmap^>(dynamic_cast<ImageBrush^>(this->SnapshotCanvas->Background)->ImageSource);

            double widthScale = displaySource->PixelWidth / actualWidth;
            double heightScale = displaySource->PixelHeight / actualHeight;

            for (UIElement^ item : this->SnapshotCanvas->Children)
            {
                Rectangle^ box = dynamic_cast<Rectangle^>(item);
                if (box == nullptr)
                {
                    continue;
                }

                // We saved the original size of the face box in the rectangles Tag field.
                BitmapBounds faceBounds = static_cast<BitmapBounds>(box->Tag);
                box->Width = faceBounds.Width / widthScale;
                box->Height = faceBounds.Height / heightScale;

                box->Margin = Thickness(faceBounds.X / widthScale, faceBounds.Y / heightScale, 0, 0);
            }
        }
    }
    catch (Platform::Exception^ ex)
    {
        this->rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
    }
}
