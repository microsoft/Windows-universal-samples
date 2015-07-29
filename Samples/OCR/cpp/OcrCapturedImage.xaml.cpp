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
#include "OcrCapturedImage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media;
using namespace Windows::Media::MediaProperties;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

using namespace std;

// Creates a task that completes with a void result.
concurrency::task<void> create_empty_task()
{
    return concurrency::create_task([] {});
}

OcrCapturedImage::OcrCapturedImage() : rootPage(MainPage::Current)
{
    InitializeComponent();

    ocrLanguage = ref new Windows::Globalization::Language("en");

    // Useful to know when to initialize/clean up the camera.
    Application::Current->Suspending += ref new Windows::UI::Xaml::SuspendingEventHandler(this, &OcrCapturedImage::Application_Suspending); ;
    Application::Current->Resuming += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &OcrCapturedImage::Application_Resuming);;
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// Ckecks if English language is avaiable for OCR on device and starts camera preview.
/// </summary>
/// <param name="e"></param>
void OcrCapturedImage::OnNavigatedTo(NavigationEventArgs^ e)
{
    orientationChangedEventToken = displayInformation->OrientationChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation ^, Platform::Object ^>(this, &SDKTemplate ::OcrCapturedImage::DisplayInformation_OrientationChanged);

    if (!OcrEngine::IsLanguageSupported(ocrLanguage))
    {
        rootPage->NotifyUser( ocrLanguage->DisplayName + " language ia not supported.", NotifyType::ErrorMessage);

        return;
    }

    create_task(StartCameraAsync()).then([this](task<void> prevTask)
    {
        Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this]()
        {
            // Update buttons visibility.
            ExtractButton->Visibility = isInitialized ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
            CameraButton->Visibility = isInitialized ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
        }));
    });
}

/// <summary>
/// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
/// Stops camera if initialized.
/// </summary>
/// <param name="e"></param>
void OcrCapturedImage::OnNavigatedFrom(NavigationEventArgs^ e)
{
    displayInformation->OrientationChanged -= orientationChangedEventToken;

    create_task(CleanupCameraAsync());
}

/// <summary>
/// Occures on app suspending. Stops camera if initialized.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrCapturedImage::Application_Suspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e)
{
    // Handle global application events only if this page is active.
    if (Frame->CurrentSourcePageType.Name == Windows::UI::Xaml::Interop::TypeName(MainPage::typeid).Name)
    {
        auto deferral = e->SuspendingOperation->GetDeferral();

        create_task(CleanupCameraAsync()).then([this, deferral]()
        {
            displayInformation->OrientationChanged -= orientationChangedEventToken;

            deferral->Complete();
        });;
    }
}

/// <summary>
/// Occures on app resuming. Initializes camera if available.
/// </summary>
/// <param name="sender"></param>
/// <param name="o"></param>
void OcrCapturedImage::Application_Resuming(Platform::Object^ sender, Platform::Object^ o)
{
    // Handle global application events only if this page is active
    if (Frame->CurrentSourcePageType.Name == Windows::UI::Xaml::Interop::TypeName(MainPage::typeid).Name)
    {
        orientationChangedEventToken = displayInformation->OrientationChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation ^, Platform::Object ^>(this, &SDKTemplate ::OcrCapturedImage::DisplayInformation_OrientationChanged);

        create_task(StartCameraAsync());
    }
}

/// <summary>
/// Occures when display orientation changes.
/// Sets camera rotation preview.
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void OcrCapturedImage::DisplayInformation_OrientationChanged(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
{
    if (isPreviewing)
    {
        create_task(SetPreviewRotationAsync());
    }
}

/// <summary>
/// This is event handler for 'Extract' button.
/// Captures image from camera ,recognizes text and displays it.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrCapturedImage::ExtractButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
    // Get information about the preview.
    auto previewProperties = safe_cast<VideoEncodingProperties^>(mediaCapture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview));
    int videoFrameWidth = (int)previewProperties->Width;
    int videoFrameHeight = (int)previewProperties->Height;

    // In portrait modes, the width and height must be swapped for the VideoFrame to have the correct aspect ratio and avoid letterboxing / black bars.
    if (!externalCamera && (displayInformation->CurrentOrientation == DisplayOrientations::Portrait || displayInformation->CurrentOrientation == DisplayOrientations::PortraitFlipped))
    {
        videoFrameWidth = (int)previewProperties->Height;
        videoFrameHeight = (int)previewProperties->Width;
    }

    // Create the video frame to request a SoftwareBitmap preview frame.
    auto videoFrame = ref new VideoFrame(BitmapPixelFormat::Bgra8, videoFrameWidth, videoFrameHeight);

    OcrEngine^ ocrEngine = OcrEngine::TryCreateFromLanguage(ocrLanguage);

    // Capture the preview frame.
    auto bitmap = make_shared<SoftwareBitmap^>(nullptr);
    create_task(mediaCapture->GetPreviewFrameAsync(videoFrame)).then([this, bitmap, ocrEngine](VideoFrame^ currentFrame)
    {
        // Collect the resulting frame.
        *bitmap = currentFrame->SoftwareBitmap;

        auto imgSource = ref new WriteableBitmap((*bitmap)->PixelWidth, (*bitmap)->PixelHeight);
        (*bitmap)->CopyToBuffer(imgSource->PixelBuffer);
        PreviewImage->Source = imgSource;

        return ocrEngine->RecognizeAsync(*bitmap);
    }).then([this, bitmap, ocrEngine](OcrResult^ ocrResult)
    {
        // Used for text overlay.
        // Prepare scale transform for words since image is not displayed in original format.
        auto scaleTrasform = ref new ScaleTransform();
        scaleTrasform->CenterX = 0;
        scaleTrasform->CenterY = 0;
        scaleTrasform->ScaleX = PreviewControl->ActualWidth / (*bitmap)->PixelWidth;
        scaleTrasform->ScaleY = PreviewControl->ActualHeight / (*bitmap)->PixelHeight;

        if (ocrResult->TextAngle != nullptr)
        {
            // If text is detected under some angle in this sample scenario we want to
            // overlay word boxes over original image, so we rotate overlay boxes.
            auto rotateTransform = ref new RotateTransform();
            rotateTransform->Angle = ocrResult->TextAngle->Value;
            rotateTransform->CenterX = PreviewImage->ActualWidth / 2;
            rotateTransform->CenterY = PreviewImage->ActualHeight / 2;
            TextOverlay->RenderTransform = rotateTransform;

            isAngleDetected = true;
        }

        // Iterate over recognized lines of text.
        for (auto line : ocrResult->Lines)
        {
            // Iterate over words in line.
            for (auto word : line->Words)
            {
                // Define the TextBlock.
                auto wordTextBlock = ref new TextBlock();
                wordTextBlock->Text = word->Text;
                wordTextBlock->Style = (Windows::UI::Xaml::Style^) this->Resources->Lookup("ExtractedWordTextStyle");

                WordOverlay^ wordBoxOverlay = ref new WordOverlay(word);

                // Keep references to word boxes.
                wordBoxes.push_back(wordBoxOverlay);

                // Define position, background, etc.
                auto overlay = ref new Border();
                overlay->Child = wordTextBlock;
                overlay->Style = (Windows::UI::Xaml::Style^) this->Resources->Lookup("HighlightedWordBoxHorizontalLine");

                // Bind word boxes to UI.
                overlay->SetBinding(FrameworkElement::MarginProperty, wordBoxOverlay->CreateWordPositionBinding());
                overlay->SetBinding(FrameworkElement::WidthProperty, wordBoxOverlay->CreateWordWidthBinding());
                overlay->SetBinding(FrameworkElement::HeightProperty, wordBoxOverlay->CreateWordHeightBinding());

                // Put the filled textblock in the results grid.
                TextOverlay->Children->Append(overlay);
            }
        }

        UpdateWordBoxTransform();

        PreviewControl->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        ImageGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
        ExtractButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        CameraButton->Visibility = Windows::UI::Xaml::Visibility::Visible;

        Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this, ocrEngine]()
        {
            rootPage->NotifyUser("Image processed using " + ocrEngine->RecognizerLanguage->DisplayName + " language.", NotifyType::StatusMessage);
        }));
    });
}

/// <summary>
/// This is event handler for 'Camera' button.
/// Clears previous OCR results and starts camera.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrCapturedImage::CameraButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
    create_task(StartCameraAsync());
}

/// <summary>
///  Update word box transform to match current UI size.
/// </summary>
void OcrCapturedImage::UpdateWordBoxTransform()
{
    if (PreviewImage->Source != nullptr)
    {
        WriteableBitmap^ bitmap = safe_cast<WriteableBitmap^>(PreviewImage->Source);

        // Used for text overlay.
        // Prepare scale transform for words since image is not displayed in original size.
        ScaleTransform^ scaleTrasform = ref new ScaleTransform();
        scaleTrasform->CenterX = 0;
        scaleTrasform->CenterY = 0;
        scaleTrasform->ScaleX = PreviewImage->ActualWidth / bitmap->PixelWidth;
        scaleTrasform->ScaleY = PreviewImage->ActualHeight / bitmap->PixelHeight;

        for (auto& box : wordBoxes)
        {
            box->Transform(scaleTrasform);
        }
    }
}

/// <summary>
/// Occures when displayed image size changes.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void OcrCapturedImage::PreviewImage_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
    UpdateWordBoxTransform();

    // Update image rotation center.
    if (isAngleDetected)
    {
        const auto& rotate = static_cast<RotateTransform^>(TextOverlay->RenderTransform);

        rotate->CenterX = PreviewImage->ActualWidth / 2;
        rotate->CenterY = PreviewImage->ActualHeight / 2;
    }
}

#pragma region MediaCapture

/// <summary>
/// Starts the camera. Initializes resources and starts preview.
/// </summary>
task<void> OcrCapturedImage::StartCameraAsync()
{
    auto cameraTask = create_empty_task();

    if (!isInitialized)
    {
        cameraTask = cameraTask.then([this]()
        {
            return InitializeCameraAsync();
        });
    }

    cameraTask.then([this](task<void> initTask)
    {
        try
        {
            initTask.get();
        }
        catch (Exception^ e)
        {
            // Use the dispatcher because this method is sometimes called from non-UI threads.
            auto msg = make_shared<String^>(e->Message);
            Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::High,
                ref new Windows::UI::Core::DispatchedHandler([this, msg]()
            {
                rootPage->NotifyUser(*msg, NotifyType::ErrorMessage);
            }));
        }

        if (isInitialized)
        {
            // Use the dispatcher because this method is sometimes called from non-UI threads.
            Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::High,
                ref new Windows::UI::Core::DispatchedHandler([this]()
            {
                TextOverlay->Children->Clear();
                wordBoxes.clear();

                PreviewImage->Source = nullptr;

                PreviewControl->Visibility = Windows::UI::Xaml::Visibility::Visible;
                ImageGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

                ExtractButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
                CameraButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

                rootPage->NotifyUser("Camera started.", NotifyType::StatusMessage);
            }));
        }
    });

    return cameraTask;
}

/// <summary>
/// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, and starts preview.
/// </summary>
task<void> OcrCapturedImage::InitializeCameraAsync()
{
    if (mediaCapture.Get() != nullptr)
    {
        return create_empty_task();
    }

    // Attempt to get the back camera if one is available, but use any camera device if not.
    return create_task(FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel::Back))
        .then([this](DeviceInformation^ cameraDevice)
    {
        if (cameraDevice == nullptr)
        {
            throw ref new Platform::Exception(E_ABORT, L"No camera device.");
        }

        // Figure out where the camera is located.
        if (cameraDevice->EnclosureLocation == nullptr || cameraDevice->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Unknown)
        {
            // No information on the location of the camera, assume it's an external camera, not integrated on the device.
            externalCamera = true;
        }
        else
        {
            // Camera is fixed on the device.
            externalCamera = false;

            // Only mirror the preview if the camera is on the front panel.
            mirroringPreview = (cameraDevice->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Front);
        }

        auto settings = ref new MediaCaptureInitializationSettings();
        settings->VideoDeviceId = cameraDevice->Id;

        // Create MediaCapture and its settings.
        mediaCapture = ref new MediaCapture();

        mediaCaptureFailedEventToken = mediaCapture->Failed += ref new Windows::Media::Capture::MediaCaptureFailedEventHandler(this, &SDKTemplate ::OcrCapturedImage::MediaCapture_Failed);

        // Initialize MediaCapture.
        return mediaCapture->InitializeAsync(settings);
    }).then([this]()
    {
        isInitialized = true;

        // If initialization succeeded, start the preview.
        return StartPreviewAsync();
    });
}

/// <summary>
/// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on and unlocks the UI.
/// </summary>
task<void> OcrCapturedImage::StartPreviewAsync()
{
    // Prevent the device from sleeping while the preview is running.
    displayRequest->RequestActive();

    return create_task([this]()
    {
        // Return task to make sure PreviewControl->Source is set before StartPreviewAsync.
        return Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this]()
        {
            // Set the preview source in the UI and mirror it if necessary.
            PreviewControl->Source = mediaCapture.Get();
            PreviewControl->FlowDirection = mirroringPreview ? ::FlowDirection::RightToLeft : ::FlowDirection::LeftToRight;
        }));
    }).then([this]()
    {
        // Start the preview.
        return mediaCapture->StartPreviewAsync();
    }).then([this]()
    {
        isPreviewing = true;

        // Initialize the preview to the current orientation.
        return SetPreviewRotationAsync();
    });
}

/// <summary>
/// Gets the current orientation of the UI in relation to the device and applies a corrective rotation to the preview.
/// </summary>
task<void> OcrCapturedImage::SetPreviewRotationAsync()
{
    // Only need to update the orientation if the camera is mounted on the device.
    if (externalCamera)
    {
        return create_empty_task();
    }

    // Calculate which way and how far to rotate the preview.
    return create_task(CalculatePreviewRotation()).then([this]()
    {
        // Set preview rotation in the preview source.
        mediaCapture->SetPreviewRotation(sourceRotation);

        // Rotation metadata to apply to the preview stream (MF_MT_VIDEO_ROTATION).
        // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
        static const GUID RotationKey = { 0xC380465D, 0x2271, 0x428C,{ 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1 } };

        // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames.
        const auto& props = mediaCapture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview);
        props->Properties->Insert(RotationKey, rotationDegrees);

        return mediaCapture->SetEncodingPropertiesAsync(MediaStreamType::VideoPreview, props, nullptr);
    });;
}

/// <summary>
/// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes, and locks the UI
/// </summary>
/// <returns></returns>
task<void> OcrCapturedImage::StopPreviewAsync()
{
    isPreviewing = false;
    return create_task(mediaCapture->StopPreviewAsync()).then([this]()
    {
        // Use the dispatcher because this method is sometimes called from non-UI threads
        return Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::High,
            ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            PreviewControl->Source = nullptr;

            // Allow the device to sleep now that the preview is stopped
            displayRequest->RequestRelease();
        }));
    }).then([this](task<void> t)
    {
        try
        {
            t.get();
        }
        catch (Exception^ e)
        {
            auto msg = make_shared<String^>(e->Message);
            Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::High,
                ref new Windows::UI::Core::DispatchedHandler([this, msg]()
            {
                rootPage->NotifyUser("Exception stopping preview. " + (*msg), NotifyType::ErrorMessage);
            }));
        }
    });
}

/// <summary>
/// Cleans up the camera resources (after stopping the preview if necessary) and unregisters from MediaCapture events.
/// </summary>
task<void> OcrCapturedImage::CleanupCameraAsync()
{
    auto cleanUpTask = create_empty_task();

    if (isInitialized)
    {
        if (isPreviewing)
        {
            // The call to stop the preview is included here for completeness, but can be
            // safely removed if a call to MediaCapture.Dispose() is being made later,
            // as the preview will be automatically stopped at that point
            cleanUpTask = cleanUpTask.then([this]()
            {
                return StopPreviewAsync();
            });
        }
    }

    cleanUpTask = cleanUpTask.then([this]()
    {
        isInitialized = false;

        if (mediaCapture.Get() != nullptr)
        {
            mediaCapture->Failed -= mediaCaptureFailedEventToken;
            delete(mediaCapture.Get());
            mediaCapture = nullptr;
        }
    });

    return cleanUpTask;
}

/// <summary>
/// Queries the available video capture devices to try and find one mounted on the desired panel.
/// </summary>
/// <param name="desiredPanel">The panel on the device that the desired camera is mounted on.</param>
/// <returns>A DeviceInformation instance with a reference to the camera mounted on the desired panel if available,
///          any other camera if not, or null if no camera is available.</returns>
task<DeviceInformation^> OcrCapturedImage::FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel desiredPanel)
{
    // Get available devices for capturing pictures.
    return create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
        .then([this, desiredPanel](DeviceInformationCollection^ allVideoDevices) -> DeviceInformation^
    {
        if (allVideoDevices == nullptr || allVideoDevices->Size == 0)
        {
            return nullptr;
        }

        // Get the desired camera by panel.
        for (const auto& device : allVideoDevices)
        {
            if (device->EnclosureLocation != nullptr && device->EnclosureLocation->Panel == desiredPanel)
            {
                return device;
            }
        }

        // If there is no device mounted on the desired panel, return the first device found.
        return allVideoDevices->First()->Current;
    });
}

/// <summary>
/// Reads the current orientation of the app and calculates the VideoRotation necessary to ensure the preview is rendered in the correct orientation.
/// </summary>
task<void> OcrCapturedImage::CalculatePreviewRotation()
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this]()
    {
        // Note that in some cases, the rotation direction needs to be inverted if the preview is being mirrored.
        switch (displayInformation->CurrentOrientation)
        {
        case DisplayOrientations::Portrait:
            if (mirroringPreview)
            {
                rotationDegrees = 270;
                sourceRotation = VideoRotation::Clockwise270Degrees;
            }
            else
            {
                rotationDegrees = 90;
                sourceRotation = VideoRotation::Clockwise90Degrees;
            }
            break;

        case DisplayOrientations::LandscapeFlipped:
            // No need to invert this rotation, as rotating 180 degrees is the same either way.
            rotationDegrees = 180;
            sourceRotation = VideoRotation::Clockwise180Degrees;
            break;

        case DisplayOrientations::PortraitFlipped:
            if (mirroringPreview)
            {
                rotationDegrees = 90;
                sourceRotation = VideoRotation::Clockwise90Degrees;
            }
            else
            {
                rotationDegrees = 270;
                sourceRotation = VideoRotation::Clockwise270Degrees;
            }
            break;

        case DisplayOrientations::Landscape:
        default:
            rotationDegrees = 0;
            sourceRotation = VideoRotation::None;
            break;
        }
    })));
}

void OcrCapturedImage::MediaCapture_Failed(MediaCapture^ sender, MediaCaptureFailedEventArgs^ errorEventArgs)
{
    auto msg = make_shared<String^>(errorEventArgs->Message);

    create_task(CleanupCameraAsync()).then([this, msg]
    {
        Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this, msg]()
        {
            ExtractButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            CameraButton->Visibility = Windows::UI::Xaml::Visibility::Visible;

            rootPage->NotifyUser(*msg, NotifyType::ErrorMessage);
        }));
    });
}

#pragma endregion MediaCapture
