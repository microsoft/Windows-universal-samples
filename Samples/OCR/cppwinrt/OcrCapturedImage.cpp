#include "pch.h"
#include "OcrCapturedImage.h"
#include "OcrCapturedImage.g.cpp"

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Ocr;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace SDKTemplate;

namespace winrt::SDKTemplate::implementation
{
    OcrCapturedImage::OcrCapturedImage()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Invoked when this page is about to be displayed in a Frame.
    /// Ckecks if English language is avaiable for OCR on device and starts camera preview..
    /// </summary>
    /// <param name="e"></param>
    fire_and_forget OcrCapturedImage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        auto lifetime = get_strong();

        orientationChangedEventToken = displayInformation.OrientationChanged({ get_weak(), &OcrCapturedImage::DisplayInformation_OrientationChanged });

        // Useful to know when to initialize/clean up the camera
        applicationSuspendingToken = Application::Current().Suspending({ get_weak(), &OcrCapturedImage::Application_Suspending });
        applicationResumingToken = Application::Current().Resuming({ get_weak(), &OcrCapturedImage::Application_Resuming });

        if (!OcrEngine::IsLanguageSupported(ocrLanguage))
        {
            rootPage.NotifyUser(ocrLanguage.DisplayName() + L" is not supported.", NotifyType::ErrorMessage);

            return;
        }

        co_await StartCameraAsync();

        // Update buttons visibility.
        ExtractButton().Visibility(isInitialized ? Visibility::Visible : Visibility::Collapsed);
        CameraButton().Visibility(isInitialized ? Visibility::Collapsed : Visibility::Visible);
    }

    /// <summary>
    /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
    /// Stops camera if initialized.
    /// </summary>
    /// <param name="e"></param>
    fire_and_forget OcrCapturedImage::OnNavigatingFrom(NavigatingCancelEventArgs const&)
    {
        auto lifetime = get_strong();

        displayInformation.OrientationChanged(orientationChangedEventToken);
        Application::Current().Suspending(applicationSuspendingToken);
        Application::Current().Resuming(applicationResumingToken);

        co_await CleanupCameraAsync();
    }

    /// <summary>
    /// Occures on app suspending. Stops camera if initialized.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    fire_and_forget OcrCapturedImage::Application_Suspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const& e)
    {
        auto lifetime = get_strong();

        auto deferral = e.SuspendingOperation().GetDeferral();

        co_await CleanupCameraAsync();

        displayInformation.OrientationChanged(orientationChangedEventToken);

        deferral.Complete();
    }

    /// <summary>
    /// Occurs on app resuming. Initializes camera if available.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="o"></param>
    fire_and_forget OcrCapturedImage::Application_Resuming(IInspectable const&, IInspectable const&)
    {
        auto lifetime = get_strong();

        orientationChangedEventToken = displayInformation.OrientationChanged({ get_weak(), &OcrCapturedImage::DisplayInformation_OrientationChanged });

        co_await StartCameraAsync();
    }

    /// <summary>
    /// Occurs when display orientation changes.
    /// Sets camera rotation preview.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OcrCapturedImage::DisplayInformation_OrientationChanged(IInspectable const&, IInspectable const&)
    {
        if (isPreviewing)
        {
            SetPreviewRotationAsync();
        }
    }

    /// <summary>
    /// This is event handler for 'Extract' button.
    /// Captures image from camera ,recognizes text and displays it.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    fire_and_forget OcrCapturedImage::ExtractButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        auto lifetime = get_strong();

        //Get information about the preview.
        auto previewProperties = mediaCapture.VideoDeviceController().GetMediaStreamProperties(MediaStreamType::VideoPreview).as<VideoEncodingProperties>();
        int videoFrameWidth = static_cast<int>(previewProperties.Width());
        int videoFrameHeight = static_cast<int>(previewProperties.Height());

        // In portrait modes, the width and height must be swapped for the VideoFrame to have the correct aspect ratio and avoid letterboxing / black bars.
        if (!externalCamera && (displayInformation.CurrentOrientation() == DisplayOrientations::Portrait || displayInformation.CurrentOrientation() == DisplayOrientations::PortraitFlipped))
        {
            videoFrameWidth = static_cast<int>(previewProperties.Height());
            videoFrameHeight = static_cast<int>(previewProperties.Width());
        }

        // Create the video frame to request a SoftwareBitmap preview frame.
        VideoFrame videoFrame(BitmapPixelFormat::Bgra8, videoFrameWidth, videoFrameHeight);

        // Capture the preview frame.

        auto currentFrame{ co_await mediaCapture.GetPreviewFrameAsync(videoFrame) };

        // Collect the resulting frame.
        SoftwareBitmap bitmap = currentFrame.SoftwareBitmap();

        OcrEngine ocrEngine = OcrEngine::TryCreateFromLanguage(ocrLanguage);

        if (ocrEngine == nullptr)
        {
            rootPage.NotifyUser(ocrLanguage.DisplayName() + L" is not supported.", NotifyType::ErrorMessage);

            co_return;
        }

        WriteableBitmap imgSource(bitmap.PixelWidth(), bitmap.PixelHeight());
        bitmap.CopyToBuffer(imgSource.PixelBuffer());
        PreviewImage().Source(imgSource);

        auto ocrResult = co_await ocrEngine.RecognizeAsync(bitmap);

        // Used for text overlay.
        // Prepare scale transform for words since image is not displayed in original format.
        ScaleTransform scaleTransform;

        scaleTransform.CenterX(0);
        scaleTransform.CenterY(0);
        scaleTransform.ScaleX(PreviewControl().ActualWidth() / bitmap.PixelWidth());
        scaleTransform.ScaleY(PreviewControl().ActualHeight() / bitmap.PixelHeight());

        auto angle = unbox_value_or<double>(ocrResult.TextAngle(), 0.0);
        if (angle != 0.0)
        {
            // If text is detected under some angle in this sample scenario we want to
            // overlay word boxes over original image, so we rotate overlay boxes.
            RotateTransform rtTemp;
            rtTemp.Angle(angle);
            rtTemp.CenterX(PreviewImage().ActualWidth() / 2);
            rtTemp.CenterY(PreviewImage().ActualHeight() / 2);

            TextOverlay().RenderTransform() = rtTemp;
        }

        // Iterate over recognized lines of text.
        for (auto&& line : ocrResult.Lines())
        {
            // Iterate over words in line.
            for (auto&& word : line.Words())
            {
                com_ptr<WordOverlay> wordBoxOverlay = make_self<WordOverlay>(word);

                // Keep references to word boxes.
                wordBoxes.push_back(wordBoxOverlay);

                // Create a box with the word inside it.
                TextBlock wordTextBlock;
                wordTextBlock.Text(word.Text());
                wordTextBlock.Style(ExtractedWordTextStyle());
                TextOverlay().Children().Append(wordBoxOverlay->CreateBorder(HighlightedWordBoxHorizontalLineStyle(), wordTextBlock));
            }
        }

        rootPage.NotifyUser(L"Image processed using " + ocrEngine.RecognizerLanguage().DisplayName() + L" language.", NotifyType::StatusMessage);

        currentFrame.Close();

        UpdateWordBoxTransform();

        PreviewControl().Visibility(Visibility::Collapsed);
        Image().Visibility(Visibility::Visible);
        ExtractButton().Visibility(Visibility::Collapsed);
        CameraButton().Visibility(Visibility::Visible);
    }

    /// <summary>
    /// This is event handler for 'Camera' button.
    /// Clears previous OCR results and starts camera.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    fire_and_forget OcrCapturedImage::CameraButton_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e)
    {
        auto lifetime = get_strong();

        co_await StartCameraAsync();
    }

    /// <summary>
    ///  Update word box transform to match current UI size.
    /// </summary>
    void OcrCapturedImage::UpdateWordBoxTransform()
    {
        WriteableBitmap bitmap = PreviewImage().Source().as<WriteableBitmap>();

        if (bitmap != nullptr)
        {
            // Used for text overlay.
            // Prepare scale transform for words since image is not displayed in original size.
            ScaleTransform scaleTransform;
            scaleTransform.CenterX(0);
            scaleTransform.CenterY(0);
            scaleTransform.ScaleX(PreviewImage().ActualWidth() / bitmap.PixelWidth());
            scaleTransform.ScaleY(PreviewImage().ActualHeight() / bitmap.PixelHeight());

            for (auto&& item : wordBoxes)
            {
                item->Transform(scaleTransform);
            }
        }
    }

    /// <summary>
    /// Occurs when displayed image size changes.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OcrCapturedImage::PreviewImage_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        UpdateWordBoxTransform();

        // Update image rotation center.
        auto rotate = TextOverlay().RenderTransform().try_as<RotateTransform>();

        if (rotate != nullptr)
        {
            rotate.CenterX(PreviewImage().ActualWidth() / 2);
            rotate.CenterY(PreviewImage().ActualHeight() / 2);
        }
    }


#pragma region MediaCapture methods

    /// <summary>
    /// Starts the camera. Initializes resources and starts preview.
    /// </summary>
    Windows::Foundation::IAsyncAction OcrCapturedImage::StartCameraAsync()
    {
        auto lifetime = get_strong();

        if (!isInitialized)
        {
            co_await InitializeCameraAsync();
        }

        if (isInitialized)
        {
            TextOverlay().Children().Clear();
            wordBoxes.clear();

            PreviewImage().Source(nullptr);

            PreviewControl().Visibility(Visibility::Visible);
            Image().Visibility(Visibility::Collapsed);

            ExtractButton().Visibility(Visibility::Visible);
            CameraButton().Visibility(Visibility::Collapsed);

            rootPage.NotifyUser(L"Camera started.", NotifyType::StatusMessage);
        }
    }

    /// <summary>
    /// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, and starts preview.
    /// </summary>
    IAsyncAction OcrCapturedImage::InitializeCameraAsync()
    {
        auto lifetime = get_strong();

        if (mediaCapture == nullptr)
        {
            // Attempt to get the back camera if one is available, but use any camera device if not.
            auto cameraDevice = co_await FindCameraDeviceByPanelAsync(winrt::Windows::Devices::Enumeration::Panel::Back);

            if (cameraDevice == nullptr)
            {
                rootPage.NotifyUser(L"No camera device!", NotifyType::ErrorMessage);

                co_return;
            }

            // Create MediaCapture and its settings.
            mediaCapture = MediaCapture();

            // Register for a notification when something goes wrong
            mediaCaptureFailedToken = mediaCapture.Failed({ get_weak(), &OcrCapturedImage::MediaCapture_Failed });

            MediaCaptureInitializationSettings settings;
            settings.VideoDeviceId(cameraDevice.Id());

            // Initialize MediaCapture
            try
            {
                co_await mediaCapture.InitializeAsync(settings);
                isInitialized = true;
            }
            catch (winrt::hresult_access_denied const&)
            {
                rootPage.NotifyUser(L"Denied access to the camera.", NotifyType::ErrorMessage);
            }
            catch (winrt::hresult_error const& e)
            {
                rootPage.NotifyUser(L"Exception when init MediaCapture. " + e.message(), NotifyType::ErrorMessage);
            }

            // If initialization succeeded, start the preview.
            if (isInitialized)
            {
                // Figure out where the camera is located
                if (cameraDevice.EnclosureLocation() == nullptr || cameraDevice.EnclosureLocation().Panel() == winrt::Windows::Devices::Enumeration::Panel::Unknown)
                {
                    // No information on the location of the camera, assume it's an external camera, not integrated on the device.
                    externalCamera = true;
                }
                else
                {
                    // Camera is fixed on the device.
                    externalCamera = false;

                    // Only mirror the preview if the camera is on the front panel.
                    mirroringPreview = (cameraDevice.EnclosureLocation().Panel() == winrt::Windows::Devices::Enumeration::Panel::Front);
                }

                co_await StartPreviewAsync();
            }
        }
    }

    /// <summary>
    /// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on and unlocks the UI.
    /// </summary>
    IAsyncAction OcrCapturedImage::StartPreviewAsync()
    {
        auto lifetime = get_strong();

        // Prevent the device from sleeping while the preview is running.
        displayRequest.RequestActive();

        // Set the preview source in the UI and mirror it if necessary.
        PreviewControl().Source(mediaCapture);
        PreviewControl().FlowDirection(mirroringPreview ? Windows::UI::Xaml::FlowDirection::RightToLeft : Windows::UI::Xaml::FlowDirection::LeftToRight);

        // Start the preview.
        try
        {
            co_await mediaCapture.StartPreviewAsync();
            isPreviewing = true;
        }
        catch (winrt::hresult_error const& ex)
        {
            rootPage.NotifyUser(L"Exception starting preview." + ex.message(), NotifyType::ErrorMessage);
        }

        // Initialize the preview to the current orientation.
        if (isPreviewing)
        {
            co_await SetPreviewRotationAsync();
        }
    }

    /// <summary>
    /// Gets the current orientation of the UI in relation to the device and applies a corrective rotation to the preview.
    /// </summary>
    IAsyncAction OcrCapturedImage::SetPreviewRotationAsync()
    {
        auto lifetime = get_strong();

        // Only need to update the orientation if the camera is mounted on the device.
        if (externalCamera) co_return;

        // Calculate which way and how far to rotate the preview.
        auto [sourceRotation, rotationDegrees] = CalculatePreviewRotation();

        // Set preview rotation in the preview source.
        mediaCapture.SetPreviewRotation(sourceRotation);

        // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
        auto props = mediaCapture.VideoDeviceController().GetMediaStreamProperties(MediaStreamType::VideoPreview);
        props.Properties().Insert(RotationKey, box_value(rotationDegrees));
        co_await mediaCapture.SetEncodingPropertiesAsync(MediaStreamType::VideoPreview, props, nullptr);
    }

    /// <summary>
    /// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes, and locks the UI
    /// </summary>
    /// <returns></returns>
    IAsyncAction OcrCapturedImage::StopPreviewAsync()
    {
        auto lifetime = get_strong();

        try
        {
            isPreviewing = false;
            co_await mediaCapture.StopPreviewAsync();
        }
        catch (winrt::hresult_error const& ex)
        {
            rootPage.NotifyUser(L"Exception stopping preview. " + ex.message(), NotifyType::ErrorMessage);
        }

        // Use the dispatcher because this method is sometimes called from non-UI threads.
        co_await resume_foreground(Dispatcher());

        PreviewControl().Source(nullptr);

        // Allow the device to sleep now that the preview is stopped.
        displayRequest.RequestRelease();
    }

    /// <summary>
    /// Cleans up the camera resources (after stopping the preview if necessary) and unregisters from MediaCapture events.
    /// </summary>
    IAsyncAction OcrCapturedImage::CleanupCameraAsync()
    {
        auto lifetime = get_strong();

        if (isInitialized)
        {
            if (isPreviewing)
            {
                // The call to stop the preview is included here for completeness, but can be
                // safely removed if a call to MediaCapture.Dispose() is being made later,
                // as the preview will be automatically stopped at that point
                co_await StopPreviewAsync();
            }

            isInitialized = false;
        }

        if (mediaCapture != nullptr)
        {
            mediaCapture.Failed(mediaCaptureFailedToken);
            mediaCapture.Close();
            mediaCapture = nullptr;
        }
    }

    /// <summary>
    /// Queries the available video capture devices to try and find one mounted on the desired panel.
    /// </summary>
    /// <param name="desiredPanel">The panel on the device that the desired camera is mounted on.</param>
    /// <returns>A DeviceInformation instance with a reference to the camera mounted on the desired panel if available,
    ///          any other camera if not, or null if no camera is available.</returns>
    IAsyncOperation<DeviceInformation> OcrCapturedImage::FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel desiredPanel)
    {
        auto lifetime = get_strong();

        // Get available devices for capturing pictures.
        auto allVideoDevices = co_await DeviceInformation::FindAllAsync(DeviceClass::VideoCapture);

        // Get the desired camera by panel
        for (auto&& device : allVideoDevices)
        {
            if (device.EnclosureLocation() != nullptr && device.EnclosureLocation().Panel() == desiredPanel)
            {
                co_return device;
            }
        }

        // If there is no device mounted on the desired panel, return the first device found, if any.
        co_return allVideoDevices.Size() == 0 ? nullptr : allVideoDevices.GetAt(0);
    }

    /// <summary>
    /// Reads the current orientation of the app and calculates the VideoRotation necessary to ensure the preview is rendered in the correct orientation.
    /// </summary>
    /// <returns>The rotation value to use in MediaCapture.SetPreviewRotation, and the accompanying rotation metadata with which to tag the preview stream.</returns>
    std::pair<VideoRotation, int> OcrCapturedImage::CalculatePreviewRotation()
    {
        VideoRotation sourceRotation;
        int rotationDegrees;

        // Note that in some cases, the rotation direction needs to be inverted if the preview is being mirrored.
        switch (displayInformation.CurrentOrientation())
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

        return { sourceRotation, rotationDegrees };
    }

    /// <summary>
    /// Handles MediaCapture failures. Cleans up the camera resources.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="errorEventArgs"></param>
    fire_and_forget OcrCapturedImage::MediaCapture_Failed(MediaCapture const&, MediaCaptureFailedEventArgs const& errorEventArgs)
    {
        auto lifetime = get_strong();

        co_await CleanupCameraAsync();

        co_await resume_foreground(Dispatcher());

        ExtractButton().Visibility(Visibility::Collapsed);
        CameraButton().Visibility(Visibility::Visible);

        rootPage.NotifyUser(L"MediaCapture Failed. " + errorEventArgs.Message(), NotifyType::ErrorMessage);
    }

#pragma endregion MediaCapture
}
