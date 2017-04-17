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
#include "MainPage.xaml.h"
#include <sstream>

using namespace Concurrency;
using namespace FaceDetection;
using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Graphics::Display;
using namespace Windows::Media;
using namespace Windows::Media::Core;
using namespace Windows::Media::FaceAnalysis;
using namespace Windows::Phone::UI::Input;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;

MainPage::MainPage()
    : _mediaCapture(nullptr)
    , _previewProperties(nullptr)
    , _isInitialized(false)
    , _isRecording(false)
    , _externalCamera(false)
    , _mirroringPreview(false)
    , _deviceOrientation(SimpleOrientation::NotRotated)
    , _displayOrientation(DisplayOrientations::Portrait)
    , _displayRequest(ref new Windows::System::Display::DisplayRequest())
    , RotationKey({ 0xC380465D, 0x2271, 0x428C,{ 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1 } })
    , _captureFolder(nullptr)
{
    InitializeComponent();

    _orientationSensor = SimpleOrientationSensor::GetDefault();
    _displayInformation = DisplayInformation::GetForCurrentView();
    _systemMediaControls = SystemMediaTransportControls::GetForCurrentView();

    // Do not cache the state of the UI when suspending/navigating
    Page::NavigationCacheMode = Navigation::NavigationCacheMode::Disabled;

    // Useful to know when to initialize/clean up the camera
    _applicationSuspendingEventToken =
        Application::Current->Suspending += ref new SuspendingEventHandler(this, &MainPage::Application_Suspending);
    _applicationResumingEventToken =
        Application::Current->Resuming += ref new EventHandler<Object^>(this, &MainPage::Application_Resuming);
}

MainPage::~MainPage()
{
    Application::Current->Suspending -= _applicationSuspendingEventToken;
    Application::Current->Resuming -= _applicationResumingEventToken;
}

/// <summary>
/// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, starts preview and unlocks the UI
/// </summary>
/// <returns></returns>
task<void> MainPage::InitializeCameraAsync()
{
    WriteLine("InitializeCameraAsync");

    // Get available devices for capturing pictures
    return FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel::Front)
        .then([this](DeviceInformation^ camera)
    {
        if (camera == nullptr)
        {
            WriteLine("No camera device found!");
            return;
        }
        // Figure out where the camera is located
        if (camera->EnclosureLocation == nullptr || camera->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Unknown)
        {
            // No information on the location of the camera, assume it's an external camera, not integrated on the device
            _externalCamera = true;
        }
        else
        {
            // Camera is fixed on the device
            _externalCamera = false;

            // Only mirror the preview if the camera is on the front panel
            _mirroringPreview = (camera->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Front);
        }

        _mediaCapture = ref new Capture::MediaCapture();

        // Register for a notification when video recording has reached the maximum time and when something goes wrong
        _recordLimitationExceededEventToken =
            _mediaCapture->RecordLimitationExceeded += ref new Capture::RecordLimitationExceededEventHandler(this, &MainPage::MediaCapture_RecordLimitationExceeded);
        _mediaCaptureFailedEventToken =
            _mediaCapture->Failed += ref new Capture::MediaCaptureFailedEventHandler(this, &MainPage::MediaCapture_Failed);

        auto settings = ref new Capture::MediaCaptureInitializationSettings();
        settings->VideoDeviceId = camera->Id;
        settings->StreamingCaptureMode = Capture::StreamingCaptureMode::AudioAndVideo;

        // Initialize media capture and start the preview
        create_task(_mediaCapture->InitializeAsync(settings))
            .then([this]()
        {
            _isInitialized = true;

            return StartPreviewAsync()
                .then([this]()
            {
                UpdateCaptureControls();
            });
        }).then([this](task<void> previousTask)
        {
            try
            {
                previousTask.get();
            }
            catch (AccessDeniedException^ ex)
            {
                WriteLine("The app was denied access to the camera");
            }
        });
    });
}

/// <summary>
/// Cleans up the camera resources (after stopping any video recording and/or preview if necessary) and unregisters from MediaCapture events
/// </summary>
/// <returns></returns>
Concurrency::task<void> MainPage::CleanupCameraAsync()
{
    WriteLine("CleanupCameraAsync");

    std::vector<task<void>> taskList;

    if (_isInitialized)
    {
        // If a recording is in progress during cleanup, stop it to save the recording
        if (_isRecording)
        {
            auto stopRecordTask = StopRecordingAsync();
            taskList.push_back(stopRecordTask);
        }

        if (_faceDetectionEffect)
        {
            auto cleanFaceDetectionTask = CleanUpFaceDetectionEffectAsync();
            taskList.push_back(cleanFaceDetectionTask);
        }

        if (_previewProperties)
        {
            auto stopPreviewTask = StopPreviewAsync();
            taskList.push_back(stopPreviewTask);
        }

        _isInitialized = false;
    }

    // When all our tasks complete, clean up MediaCapture
    return when_all(taskList.begin(), taskList.end())
        .then([this]()
    {
        if (_mediaCapture != nullptr)
        {
            _mediaCapture->RecordLimitationExceeded -= _recordLimitationExceededEventToken;
            _mediaCapture->Failed -= _mediaCaptureFailedEventToken;
            _mediaCapture = nullptr;
        }
    });
}

/// <summary>
/// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on
/// </summary>
/// <returns></returns>
task<void> MainPage::StartPreviewAsync()
{
    // Prevent the device from sleeping while the preview is running
    _displayRequest->RequestActive();

    // Set the preview source in the UI and mirror it if necessary
    PreviewControl->Source = _mediaCapture.Get();
    PreviewControl->FlowDirection = _mirroringPreview ? Windows::UI::Xaml::FlowDirection::RightToLeft : Windows::UI::Xaml::FlowDirection::LeftToRight;

    // Start the preview
    return create_task(_mediaCapture->StartPreviewAsync())
        .then([this](task<void> previousTask)
    {
        _previewProperties = _mediaCapture->VideoDeviceController->GetMediaStreamProperties(Capture::MediaStreamType::VideoPreview);

        // Only need to update the orientation if the camera is mounted on the device
        if (!_externalCamera)
        {
            _displayOrientation = _displayInformation->CurrentOrientation;
            return SetPreviewRotationAsync();
        }

        // Not external, just return the previous task
        return previousTask;
    });
}

/// <summary>
/// Gets the current orientation of the UI in relation to the device (when AutoRotationPreferences cannot be honored) and applies a corrective rotation to the preview
/// </summary>
/// <returns></returns>
task<void> MainPage::SetPreviewRotationAsync()
{
    // Calculate which way and how far to rotate the preview
    int rotationDegrees = ConvertDisplayOrientationToDegrees(_displayOrientation);

    // The rotation direction needs to be inverted if the preview is being mirrored
    if (_mirroringPreview)
    {
        rotationDegrees = (360 - rotationDegrees) % 360;
    }

    // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
    auto props = _mediaCapture->VideoDeviceController->GetMediaStreamProperties(Capture::MediaStreamType::VideoPreview);
    props->Properties->Insert(RotationKey, rotationDegrees);
    return create_task(_mediaCapture->SetEncodingPropertiesAsync(Capture::MediaStreamType::VideoPreview, props, nullptr));
}

/// <summary>
/// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes
/// </summary>
/// <returns></returns>
task<void> MainPage::StopPreviewAsync()
{
    _previewProperties = nullptr;

    return create_task(_mediaCapture->StopPreviewAsync())
        .then([this]()
    {
        // Use the dispatcher because this method is sometimes called from non-UI threads
        return Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            // Cleanup the UI
            PreviewControl->Source = nullptr;
            // Allow the device screen to sleep now that the preview is stopped
            _displayRequest->RequestRelease();
        }));
    });
}

/// <summary>
/// Adds face detection to the preview stream, registers for its events, enables it, and gets the FaceDetectionEffect instance
/// </summary>
/// <returns></returns>
task<void> MainPage::CreateFaceDetectionEffectAsync()
{
    // Create the definition, which will contain some initialization settings
    auto definition = ref new FaceDetectionEffectDefinition();

    // To ensure preview smoothness, do not delay incoming samples
    definition->SynchronousDetectionEnabled = false;

    // In this scenario, choose detection speed over accuracy
    definition->DetectionMode = FaceDetectionMode::HighPerformance;

    // Add the effect to the preview stream
    return create_task(_mediaCapture->AddVideoEffectAsync(definition, Capture::MediaStreamType::VideoPreview))
        .then([this](IMediaExtension^ extension)
    {
        _faceDetectionEffect = static_cast<FaceDetectionEffect^>(extension);
      
        // Register for face detection events
        _faceDetectedEventToken =
            _faceDetectionEffect->FaceDetected += ref new TypedEventHandler<FaceDetectionEffect^, FaceDetectedEventArgs^>(this, &MainPage::FaceDetectionEffect_FaceDetected);

        // Choose the shortest interval between detection events
        Windows::Foundation::TimeSpan duration = Windows::Foundation::TimeSpan();
        duration.Duration = 330000; // hundreds of nanoseconds (33ms)
        _faceDetectionEffect->DesiredDetectionInterval = duration;

        // Start detecting faces
        _faceDetectionEffect->Enabled = true;
    });
}

/// <summary>
///  Disables and removes the face detection effect, and unregisters the event handler for face detection
/// </summary>
/// <returns></returns>
task<void> MainPage::CleanUpFaceDetectionEffectAsync()
{
    // Disable detection
    _faceDetectionEffect->Enabled = false;

    // Unregister the event handler
    _faceDetectionEffect->FaceDetected -= _faceDetectedEventToken;

    // Remove the effect (see ClearEffectsAsync method to remove all effects from a stream)
    return create_task(_mediaCapture->RemoveEffectAsync(_faceDetectionEffect))
        .then([this]()
    {
        // Clear the member variable that held the effect instance
        _faceDetectionEffect = nullptr;
    });
}

/// <summary>
/// Takes a photo to a StorageFile and adds rotation metadata to it
/// </summary>
/// <returns></returns>
task<void> MainPage::TakePhotoAsync()
{
    // While taking a photo, keep the video button enabled only if the camera supports simultaneously taking pictures and recording video
    VideoButton->IsEnabled = _mediaCapture->MediaCaptureSettings->ConcurrentRecordAndPhotoSupported;
    // Make the button invisible if it's disabled, so it's obvious it cannot be interacted with
    VideoButton->Opacity = VideoButton->IsEnabled ? 1 : 0;

    auto inputStream = ref new Streams::InMemoryRandomAccessStream();

    // Take the picture
    WriteLine("Taking photo...");
    return create_task(_mediaCapture->CapturePhotoToStreamAsync(Windows::Media::MediaProperties::ImageEncodingProperties::CreateJpeg(), inputStream))
        .then([this, inputStream]()
    {
        return create_task(_captureFolder->CreateFileAsync("SimplePhoto.jpg", CreationCollisionOption::GenerateUniqueName));
    }).then([this, inputStream](StorageFile^ file)
    {
        WriteLine("Photo taken! Saving to " + file->Path);

        // Done taking a photo, so re-enable the button
        VideoButton->IsEnabled = true;
        VideoButton->Opacity = 1;

        auto photoOrientation = ConvertOrientationToPhotoOrientation(GetCameraOrientation());
        return ReencodeAndSavePhotoAsync(inputStream, file, photoOrientation);
    }).then([this](task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            // File I/O errors are reported as exceptions
            WriteException(ex);
        }
    });
}

/// <summary>
/// Records an MP4 video to a StorageFile and adds rotation metadata to it
/// </summary>
/// <returns></returns>
task<void> MainPage::StartRecordingAsync()
{
    return create_task(_captureFolder->CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption::GenerateUniqueName))
        .then([this](StorageFile^ file)
    {
        // Calculate rotation angle, taking mirroring into account if necessary
        auto rotationAngle = 360 - ConvertDeviceOrientationToDegrees(GetCameraOrientation());
        auto encodingProfile = MediaProperties::MediaEncodingProfile::CreateMp4(MediaProperties::VideoEncodingQuality::Auto);
        encodingProfile->Video->Properties->Insert(RotationKey, rotationAngle);

        return create_task(_mediaCapture->StartRecordToStorageFileAsync(encodingProfile, file))
            .then([this, file]()
        {
            _isRecording = true;
            WriteLine("Started recording to " + file->Path);
        });
    }).then([this](task<void> previousTask)
    {
        try
        {
            previousTask.get();
        }
        catch (Exception^ ex)
        {
            // File I/O errors are reported as exceptions
            WriteException(ex);
        }
    });
}

/// <summary>
/// Stops recording a video
/// </summary>
/// <returns></returns>
task<void> MainPage::StopRecordingAsync()
{
    _isRecording = false;

    WriteLine("Stopping recording...");
    return create_task(_mediaCapture->StopRecordAsync())
        .then([this]()
    {
        WriteLine("Stopped recording!");
    });
}

/// <summary>
/// Attempts to find and return a device mounted on the panel specified, and on failure to find one it will return the first device listed
/// </summary>
/// <param name="panel">The desired panel on which the returned device should be mounted, if available</param>
/// <returns></returns>
task<DeviceInformation^> MainPage::FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel panel)
{
    // Get available devices for capturing pictures
    auto allVideoDevices = DeviceInformation::FindAllAsync(DeviceClass::VideoCapture);

    auto deviceEnumTask = create_task(allVideoDevices);
    return deviceEnumTask.then([panel](DeviceInformationCollection^ devices)
    {
        for (auto cameraDeviceInfo : devices)
        {
            if (cameraDeviceInfo->EnclosureLocation != nullptr && cameraDeviceInfo->EnclosureLocation->Panel == panel)
            {
                return cameraDeviceInfo;
            }
        }

        // Nothing matched, just return the first
        if (devices->Size > 0)
        {
            return devices->GetAt(0);
        }

        // We didn't find any devices, so return a null instance
        DeviceInformation^ camera = nullptr;
        return camera;
    });
}

/// <summary>
/// Applies the given orientation to a photo stream and saves it as a StorageFile
/// </summary>
/// <param name="stream">The photo stream</param>
/// <param name="file">The StorageFile in which the photo stream will be saved</param>
/// <param name="photoOrientation">The orientation metadata to apply to the photo</param>
/// <returns></returns>
task<void> MainPage::ReencodeAndSavePhotoAsync(Streams::IRandomAccessStream^ stream, StorageFile^ file, FileProperties::PhotoOrientation photoOrientation)
{
    // Using this state variable to pass multiple values through our task chain
    ReencodeState^ state = ref new ReencodeState();
    state->File = file;
    state->Orientation = photoOrientation;

    return create_task(BitmapDecoder::CreateAsync(stream))
        .then([state](BitmapDecoder^ decoder)
    {
        state->Decoder = decoder;
        return create_task(state->File->OpenAsync(FileAccessMode::ReadWrite));
    }).then([state](Streams::IRandomAccessStream^ outputStream)
    {
        return create_task(BitmapEncoder::CreateForTranscodingAsync(outputStream, state->Decoder));
    }).then([state](BitmapEncoder^ encoder)
    {
        state->Encoder = encoder;
        auto properties = ref new Windows::Graphics::Imaging::BitmapPropertySet();
        properties->Insert("System.Photo.Orientation", ref new BitmapTypedValue((unsigned short)state->Orientation, Windows::Foundation::PropertyType::UInt16));

        return create_task(state->Encoder->BitmapProperties->SetPropertiesAsync(properties));
    }).then([state]()
    {
        return state->Encoder->FlushAsync();
    });
}

/// <summary>
/// This method will update the icons, enable/disable and show/hide the photo/video buttons depending on the current state of the app and the capabilities of the device
/// </summary>
void MainPage::UpdateCaptureControls()
{
    // The buttons should only be enabled if the preview started sucessfully
    PhotoButton->IsEnabled = _previewProperties != nullptr;
    VideoButton->IsEnabled = _previewProperties != nullptr;
    FaceDetectionButton->IsEnabled = _previewProperties != nullptr;

    // Update the face detection icon depending on whether the effect exists or not
    FaceDetectionDisabledIcon->Visibility = (_faceDetectionEffect == nullptr || 
        !_faceDetectionEffect->Enabled) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
    FaceDetectionEnabledIcon->Visibility = (_faceDetectionEffect != nullptr && 
        _faceDetectionEffect->Enabled) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;

    // Hide the face detection canvas and clear it
    FacesCanvas->Visibility = (_faceDetectionEffect != nullptr && 
        _faceDetectionEffect->Enabled) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;

    // Update recording button to show "Stop" icon instead of red "Record" icon
    StartRecordingIcon->Visibility = _isRecording ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
    StopRecordingIcon->Visibility = _isRecording ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;

    // If the camera doesn't support simultaneously taking pictures and recording video, disable the photo button on record
    if (_isInitialized && !_mediaCapture->MediaCaptureSettings->ConcurrentRecordAndPhotoSupported)
    {
        PhotoButton->IsEnabled = !_isRecording;

        // Make the button invisible if it's disabled, so it's obvious it cannot be interacted with
        PhotoButton->Opacity = PhotoButton->IsEnabled ? 1 : 0;
    }
}

/// <summary>
/// Attempts to lock the page orientation, hide the StatusBar (on Phone) and registers event handlers for hardware buttons and orientation sensors
/// </summary>
/// <returns></returns>
task<void> MainPage::SetupUiAsync()
{
    // Attempt to lock page to landscape orientation to prevent the CaptureElement from rotating, as this gives a better experience
    DisplayInformation::AutoRotationPreferences = DisplayOrientations::Landscape;
    
    RegisterEventHandlers();

    // Populate orientation variables with the current state
    _displayOrientation = _displayInformation->CurrentOrientation;
    if (_orientationSensor != nullptr)
    {
        _deviceOrientation = _orientationSensor->GetCurrentOrientation();
    }

    create_task(StorageLibrary::GetLibraryAsync(KnownLibraryId::Pictures))
        .then([this](StorageLibrary^ picturesLibrary)
    {
        _captureFolder = picturesLibrary->SaveFolder;
        if (_captureFolder == nullptr)
        {
            // In this case fall back to the local app storage since the Pictures Library is not available
            _captureFolder = ApplicationData::Current->LocalFolder;
        }
    });

    // Hide the status bar
    if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
    {
        return create_task(Windows::UI::ViewManagement::StatusBar::GetForCurrentView()->HideAsync());
    }
    else
    {
        return EmptyTask();
    }
}

/// <summary>
/// Unregisters event handlers for hardware buttons and orientation sensors, allows the StatusBar (on Phone) to show, and removes the page orientation lock
/// </summary>
/// <returns></returns>
task<void> MainPage::CleanupUiAsync()
{
    UnregisterEventHandlers();

    // Revert orientation preferences
    DisplayInformation::AutoRotationPreferences = DisplayOrientations::None;

    // Show the status bar
    if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
    {
        return create_task(Windows::UI::ViewManagement::StatusBar::GetForCurrentView()->ShowAsync());
    }
    else
    {
        return EmptyTask();
    }
}

/// <summary>
/// Registers event handlers for hardware buttons and orientation sensors, and performs an initial update of the UI rotation
/// </summary>
void MainPage::RegisterEventHandlers()
{
    if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
    {
        _hardwareCameraButtonEventToken =
            HardwareButtons::CameraPressed += ref new Windows::Foundation::EventHandler<CameraEventArgs^>(this, &MainPage::HardwareButtons_CameraPress);
    }

    // If there is an orientation sensor present on the device, register for notifications
    if (_orientationSensor != nullptr)
    {
        _orientationChangedEventToken =
            _orientationSensor->OrientationChanged += ref new TypedEventHandler<SimpleOrientationSensor^, SimpleOrientationSensorOrientationChangedEventArgs^>(this, &MainPage::OrientationSensor_OrientationChanged);

        // Update orientation of buttons with the current orientation
        UpdateButtonOrientation();
    }

    _displayInformationEventToken =
        _displayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::DisplayInformation_OrientationChanged);

    _mediaControlPropChangedEventToken =
        _systemMediaControls->PropertyChanged += ref new TypedEventHandler<SystemMediaTransportControls^, SystemMediaTransportControlsPropertyChangedEventArgs^>(this, &MainPage::SystemMediaControls_PropertyChanged);
}

/// <summary>
/// Unregisters event handlers for hardware buttons and orientation sensors
/// </summary>
void MainPage::UnregisterEventHandlers()
{
    if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
    {
        HardwareButtons::CameraPressed -= _hardwareCameraButtonEventToken;
    }

    if (_orientationSensor != nullptr)
    {
        _orientationSensor->OrientationChanged -= _orientationChangedEventToken;
    }

    _displayInformation->OrientationChanged -= _displayInformationEventToken;
    _systemMediaControls->PropertyChanged -= _mediaControlPropChangedEventToken;
}

/// <summary>
/// Writes a given string to the output window
/// </summary>
/// <param name="str">String to be written</param>
void MainPage::WriteLine(String^ str)
{
    std::wstringstream wStringstream;
    wStringstream << str->Data() << "\n";
    OutputDebugString(wStringstream.str().c_str());
}

/// <summary>
/// Writes a given exception message and hresult to the output window
/// </summary>
/// <param name="ex">Exception to be written</param>
void MainPage::WriteException(Exception^ ex)
{
    std::wstringstream wStringstream;
    wStringstream << "0x" << ex->HResult << ": " << ex->Message->Data();
    OutputDebugString(wStringstream.str().c_str());
}

/// <summary>
/// Sometimes we need to conditionally return a task. If certain parameters are not met we cannot 
/// return null, but we can return a task that does nothing.
/// </summary>
/// <returns></returns>
task<void> MainPage::EmptyTask()
{
    return create_task([] {});
}

/// <summary>
/// Calculates the current camera orientation from the device orientation by taking into account whether the camera is external or facing the user
/// </summary>
/// <returns>The camera orientation in space, with an inverted rotation in the case the camera is mounted on the device and is facing the user</returns>
SimpleOrientation MainPage::GetCameraOrientation()
{
    if (_externalCamera)
    {
        // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
        return SimpleOrientation::NotRotated;
    }

    auto result = _deviceOrientation;

    // Account for the fact that, on portrait-first devices, the camera sensor is mounted at a 90 degree offset to the native orientation
    if (_displayInformation->NativeOrientation == DisplayOrientations::Portrait)
    {
        switch (result)
        {
        case SimpleOrientation::Rotated90DegreesCounterclockwise:
            result = SimpleOrientation::NotRotated;
            break;
        case SimpleOrientation::Rotated180DegreesCounterclockwise:
            result = SimpleOrientation::Rotated90DegreesCounterclockwise;
            break;
        case SimpleOrientation::Rotated270DegreesCounterclockwise:
            result = SimpleOrientation::Rotated180DegreesCounterclockwise;
            break;
        case SimpleOrientation::NotRotated:
        default:
            result = SimpleOrientation::Rotated270DegreesCounterclockwise;
            break;
        }
    }

    // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
    if (_mirroringPreview)
    {
        // This only affects the 90 and 270 degree cases, because rotating 0 and 180 degrees is the same clockwise and counter-clockwise
        switch (result)
        {
        case SimpleOrientation::Rotated90DegreesCounterclockwise:
            return SimpleOrientation::Rotated270DegreesCounterclockwise;
        case SimpleOrientation::Rotated270DegreesCounterclockwise:
            return SimpleOrientation::Rotated90DegreesCounterclockwise;
        }
    }

    return result;
}

/// <summary>
/// Converts the given orientation of the device in space to the metadata that can be added to captured photos
/// </summary>
/// <param name="orientation">The orientation of the device in space</param>
/// <returns></returns>
FileProperties::PhotoOrientation MainPage::ConvertOrientationToPhotoOrientation(SimpleOrientation orientation)
{
    switch (orientation)
    {
    case SimpleOrientation::Rotated90DegreesCounterclockwise:
        return FileProperties::PhotoOrientation::Rotate90;
    case SimpleOrientation::Rotated180DegreesCounterclockwise:
        return FileProperties::PhotoOrientation::Rotate180;
    case SimpleOrientation::Rotated270DegreesCounterclockwise:
        return FileProperties::PhotoOrientation::Rotate270;
    case SimpleOrientation::NotRotated:
    default:
        return FileProperties::PhotoOrientation::Normal;
    }
}

/// <summary>
/// Converts the given orientation of the device in space to the corresponding rotation in degrees
/// </summary>
/// <param name="orientation">The orientation of the device in space</param>
/// <returns>An orientation in degrees</returns>
int MainPage::ConvertDeviceOrientationToDegrees(SimpleOrientation orientation)
{
    switch (orientation)
    {
    case SimpleOrientation::Rotated90DegreesCounterclockwise:
        return 90;
    case SimpleOrientation::Rotated180DegreesCounterclockwise:
        return 180;
    case SimpleOrientation::Rotated270DegreesCounterclockwise:
        return 270;
    case SimpleOrientation::NotRotated:
    default:
        return 0;
    }
}

/// <summary>
/// Converts the given orientation of the app on the screen to the corresponding rotation in degrees
/// </summary>
/// <param name="orientation">The orientation of the app on the screen</param>
/// <returns>An orientation in degrees</returns>
int MainPage::ConvertDisplayOrientationToDegrees(DisplayOrientations orientation)
{
    switch (orientation)
    {
    case DisplayOrientations::Portrait:
        return 90;
    case DisplayOrientations::LandscapeFlipped:
        return 180;
    case DisplayOrientations::PortraitFlipped:
        return 270;
    case DisplayOrientations::Landscape:
    default:
        return 0;
    }
}

/// <summary>
/// Uses the current device orientation in space and page orientation on the screen to calculate the rotation
/// transformation to apply to the controls
/// </summary>
void MainPage::UpdateButtonOrientation()
{
    int device = ConvertDeviceOrientationToDegrees(_deviceOrientation);
    int display = ConvertDisplayOrientationToDegrees(_displayOrientation);

    if (_displayInformation->NativeOrientation == DisplayOrientations::Portrait)
    {
        device -= 90;
    }

    // Combine both rotations and make sure that 0 <= result < 360
    auto angle = (360 + display + device) % 360;

    // Rotate the buttons in the UI to match the rotation of the device
    auto transform = ref new RotateTransform();
    transform->Angle = angle;

    // The RenderTransform is safe to use (i.e. it won't cause layout issues) in this case, because these buttons have a 1:1 aspect ratio
    PhotoButton->RenderTransform = transform;
    VideoButton->RenderTransform = transform;
    FaceDetectionButton->RenderTransform = transform;
}

/// <summary>
/// Uses the current display orientation to calculate the rotation transformation to apply to the face detection bounding box canvas
/// and mirrors it if the preview is being mirrored
/// </summary>
void MainPage::SetFacesCanvasRotation()
{
    // Calculate how much to rotate the canvas
    int rotationDegrees = ConvertDisplayOrientationToDegrees(_displayOrientation);

    // The rotation direction needs to be inverted if the preview is being mirrored, just like in SetPreviewRotationAsync
    if (_mirroringPreview)
    {
        rotationDegrees = (360 - rotationDegrees) % 360;
    }

    // Apply the rotation
    auto transform = ref new RotateTransform();
    transform->Angle = rotationDegrees;
    FacesCanvas->RenderTransform = transform;

    auto previewArea = GetPreviewStreamRectInControl(static_cast<MediaProperties::VideoEncodingProperties^>(_previewProperties), PreviewControl);

    // For portrait mode orientations, swap the width and height of the canvas after the rotation, so the control continues to overlap the preview
    if (_displayOrientation == DisplayOrientations::Portrait || _displayOrientation == DisplayOrientations::PortraitFlipped)
    {
        FacesCanvas->Width = previewArea.Height;
        FacesCanvas->Height = previewArea.Width;

        // The position of the canvas also needs to be adjusted, as the size adjustment affects the centering of the control
        Canvas::SetLeft(FacesCanvas, previewArea.X - (previewArea.Height - previewArea.Width) / 2);
        Canvas::SetTop(FacesCanvas, previewArea.Y - (previewArea.Width - previewArea.Height) / 2);
    }
    else
    {
        FacesCanvas->Width = previewArea.Width;
        FacesCanvas->Height = previewArea.Height;

        Canvas::SetLeft(FacesCanvas, previewArea.X);
        Canvas::SetTop(FacesCanvas, previewArea.Y);
    }

    // Also mirror the canvas if the preview is being mirrored
    FacesCanvas->FlowDirection = _mirroringPreview ? Windows::UI::Xaml::FlowDirection::RightToLeft : Windows::UI::Xaml::FlowDirection::LeftToRight;
}

/// <summary>
/// Iterates over all detected faces, creating and adding Rectangles to the FacesCanvas as face bounding boxes
/// </summary>
/// <param name="faces">The list of detected faces from the FaceDetected event of the effect</param>
void MainPage::HighlightDetectedFaces(Windows::Foundation::Collections::IVectorView<DetectedFace^>^ faces)
{
    // Remove any existing rectangles from previous events
    FacesCanvas->Children->Clear();

    // For each detected face
    for (unsigned int i = 0; i < faces->Size; i++)
    {
        // Face coordinate units are preview resolution pixels, which can be a different scale from our display resolution, so a conversion may be necessary
        Rectangle^ faceBoundingBox = ConvertPreviewToUiRectangle(faces->GetAt(i)->FaceBox);

        // Set bounding box stroke properties
        faceBoundingBox->StrokeThickness = 2;

        // Highlight the first face in the set
        faceBoundingBox->Stroke = (i == 0 ? ref new SolidColorBrush(Windows::UI::Colors::Blue) : ref new SolidColorBrush(Windows::UI::Colors::DeepSkyBlue));

        // Add grid to canvas containing all face UI objects
        FacesCanvas->Children->Append(faceBoundingBox);
    }

    // Update the face detection bounding box canvas orientation
    SetFacesCanvasRotation();
}

/// <summary>
/// Takes face information defined in preview coordinates and returns one in UI coordinates, taking
/// into account the position and size of the preview control.
/// </summary>
/// <param name="faceBoxInPreviewCoordinates">Face coordinates as retried from the FaceBox property of a DetectedFace, in preview coordinates.</param>
/// <returns>Rectangle in UI (CaptureElement) coordinates, to be used in a Canvas control.</returns>
Rectangle^ MainPage::ConvertPreviewToUiRectangle(BitmapBounds faceBoxInPreviewCoordinates)
{
    auto result = ref new Rectangle();
    auto previewStream = static_cast<MediaProperties::VideoEncodingProperties^>(_previewProperties);

    // If there is no available information about the preview, return an empty rectangle, as re-scaling to the screen coordinates will be impossible
    if (previewStream == nullptr)
    {
        return result;
    }

    // Similarly, if any of the dimensions is zero (which would only happen in an error case) return an empty rectangle
    if (previewStream->Width == 0 || previewStream->Height == 0)
    {
        return result;
    }

    double streamWidth = previewStream->Width;
    double streamHeight = previewStream->Height;

    // For portrait orientations, the width and height need to be swapped
    if (_displayOrientation == DisplayOrientations::Portrait || _displayOrientation == DisplayOrientations::PortraitFlipped)
    {
        streamHeight = previewStream->Width;
        streamWidth = previewStream->Height;
    }

    auto previewInUI = GetPreviewStreamRectInControl(previewStream, PreviewControl);

    // Scale the width and height from preview stream coordinates to window coordinates
    result->Width = (faceBoxInPreviewCoordinates.Width / streamWidth) * previewInUI.Width;
    result->Height = (faceBoxInPreviewCoordinates.Height / streamHeight) * previewInUI.Height;

    // Scale the X and Y coordinates from preview stream coordinates to window coordinates
    auto x = (faceBoxInPreviewCoordinates.X / streamWidth) * previewInUI.Width;
    auto y = (faceBoxInPreviewCoordinates.Y / streamHeight) * previewInUI.Height;
    Canvas::SetLeft(result, x);
    Canvas::SetTop(result, y);

    return result;
}

/// <summary>
/// Calculates the size and location of the rectangle that contains the preview stream within the preview control, when the scaling mode is Uniform
/// </summary>
/// <param name="previewResolution">The resolution at which the preview is running</param>
/// <param name="previewControl">The control that is displaying the preview using Uniform as the scaling mode</param>
/// <returns></returns>
Windows::Foundation::Rect MainPage::GetPreviewStreamRectInControl(MediaProperties::VideoEncodingProperties^ previewResolution, Controls::CaptureElement^ previewControl)
{
    // In case this function is called before everything is initialized correctly, return an empty result
    if (previewControl == nullptr || previewControl->ActualHeight < 1 || previewControl->ActualWidth < 1 ||
        previewResolution == nullptr || previewResolution->Height == 0 || previewResolution->Width == 0)
    {
        return RectHelper::FromCoordinatesAndDimensions(0, 0, 0, 0);
    }

    auto streamWidth = previewResolution->Width;
    auto streamHeight = previewResolution->Height;

    // For portrait orientations, the width and height need to be swapped
    if (_displayOrientation == DisplayOrientations::Portrait || _displayOrientation == DisplayOrientations::PortraitFlipped)
    {
        streamWidth = previewResolution->Height;
        streamHeight = previewResolution->Width;
    }

    // Start by assuming the preview display area in the control spans the entire width and height both (this is corrected in the next if for the necessary dimension)
    auto width = previewControl->ActualWidth;
    auto height = previewControl->ActualHeight;
    double x = 0.0;
    double y = 0.0;

    // If UI is "wider" than preview, letterboxing will be on the sides
    if ((previewControl->ActualWidth / previewControl->ActualHeight > streamWidth / static_cast<double>(streamHeight)))
    {
        auto scale = previewControl->ActualHeight / streamHeight;
        auto scaledWidth = streamWidth * scale;

        x = (previewControl->ActualWidth - scaledWidth) / 2.0;
        width = scaledWidth;
    }
    else // Preview stream is "wider" than UI, so letterboxing will be on the top+bottom
    {
        auto scale = previewControl->ActualWidth / streamWidth;
        auto scaledHeight = streamHeight * scale;

        y = static_cast<float>((previewControl->ActualHeight - scaledHeight) / 2.0);
        height = scaledHeight;
    }

    return RectHelper::FromCoordinatesAndDimensions(
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(height));
}

void MainPage::Application_Suspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e)
{
    // Handle global application events only if this page is active
    if (Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPage::typeid).Name)
    {
        auto deferral = e->SuspendingOperation->GetDeferral();
        CleanupUiAsync()
            .then([this, deferral]()
        {
            CleanupCameraAsync();
        }).then([this, deferral]()
        {
            deferral->Complete();
        });
    }
}

void MainPage::Application_Resuming(Platform::Object^ sender, Platform::Object^ args)
{
    // Handle global application events only if this page is active
    if (Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPage::typeid).Name)
    {
        SetupUiAsync();
        InitializeCameraAsync();
    }
}

/// <summary>
/// This event will fire when the page is rotated, when the DisplayInformation.AutoRotationPreferences value set in the SetupUiAsync() method cannot be not honored.
/// </summary>
/// <param name="sender">The event source.</param>
/// <param name="args">The event data.</param>
void MainPage::DisplayInformation_OrientationChanged(DisplayInformation^ sender, Object^ args)
{
    _displayOrientation = sender->CurrentOrientation;

    if (_previewProperties != nullptr)
    {
        SetPreviewRotationAsync();
    }

    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        UpdateButtonOrientation();
    }));
}

void MainPage::PhotoButton_Click(Object^, Windows::UI::Xaml::RoutedEventArgs^)
{
    TakePhotoAsync();
}

void MainPage::VideoButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    task<void> taskToExecute;
    if (!_isRecording)
    {
        taskToExecute = StartRecordingAsync();
    }
    else
    {
        taskToExecute = StopRecordingAsync();
    }

    taskToExecute.then([this](task<void> previousTask)
    {
        // After starting or stopping video recording, update the UI to reflect the MediaCapture state
        UpdateCaptureControls();
    });
}

void MainPage::FaceDetectionButton_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    task<void> taskToExecute;

    if (_faceDetectionEffect == nullptr || !_faceDetectionEffect->Enabled)
    {
        // Clear any rectangles that may have been left over from a previous instance of the effect
        FacesCanvas->Children->Clear();

        taskToExecute = CreateFaceDetectionEffectAsync();
    }
    else
    {
        taskToExecute = CleanUpFaceDetectionEffectAsync();
    }

    taskToExecute
        .then([this]()
    {
        UpdateCaptureControls();
    });
}

/// <summary>
/// In the event of the app being minimized this method handles media property change events. If the app receives a mute
/// notification, it is no longer in the foregroud.
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void MainPage::SystemMediaControls_PropertyChanged(SystemMediaTransportControls^ sender, SystemMediaTransportControlsPropertyChangedEventArgs^ args)
{
    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, sender, args]()
    {
        // Only handle this event if this page is currently being displayed
        if (args->Property == SystemMediaTransportControlsProperty::SoundLevel && Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPage::typeid).Name)
        {
            // Check to see if the app is being muted. If so, it is being minimized.
            // Otherwise if it is not initialized, it is being brought into focus.
            if (sender->SoundLevel == SoundLevel::Muted)
            {
                CleanupCameraAsync();
            }
            else if (!_isInitialized)
            {
                InitializeCameraAsync();
            }
        }
    }));
}

/// <summary>
/// Occurs each time the simple orientation sensor reports a new sensor reading.
/// </summary>
/// <param name="args">The event data.</param>
void MainPage::OrientationSensor_OrientationChanged(SimpleOrientationSensor^, SimpleOrientationSensorOrientationChangedEventArgs^ args)
{
    // If the device is parallel to the ground, keep the last orientation used. This allows users to take pictures of documents (FaceUp)
    // or the ceiling (FaceDown) in any orientation, by first holding the device in the desired orientation, and then pointing the camera
    // at the desired subject.
    if (args->Orientation != SimpleOrientation::Faceup && args->Orientation != SimpleOrientation::Facedown)
    {
        _deviceOrientation = args->Orientation;

        Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            UpdateButtonOrientation();
        }));
    }
}

void MainPage::HardwareButtons_CameraPress(Platform::Object^, Windows::Phone::UI::Input::CameraEventArgs^)
{
    TakePhotoAsync();
}

/// <summary>
/// This is a notification that recording has to stop, and the app is expected to finalize the recording
/// </summary>
/// <param name="args">The event data.</param>
void MainPage::MediaCapture_RecordLimitationExceeded(Capture::MediaCapture^)
{
    StopRecordingAsync()
        .then([this]()
    {
        Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            UpdateCaptureControls();
        }));
    });
}

void MainPage::MediaCapture_Failed(Capture::MediaCapture ^currentCaptureObject, Capture::MediaCaptureFailedEventArgs^ errorEventArgs)
{
    std::wstringstream ss;
    ss << "MediaCapture_Failed: 0x" << errorEventArgs->Code << ": " << errorEventArgs->Message->Data();
    WriteLine(ref new String(ss.str().c_str()));

    CleanupCameraAsync()
        .then([this]()
    {
        Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            UpdateCaptureControls();
        }));
    });
}

void MainPage::FaceDetectionEffect_FaceDetected(Windows::Media::Core::FaceDetectionEffect^ sender, FaceDetectedEventArgs^ args)
{
    // Ask the UI thread to render the face bounding boxes
    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, args]()
    {
        HighlightDetectedFaces(args->ResultFrame->DetectedFaces);
    }));
}

void MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    SetupUiAsync();
    InitializeCameraAsync();
}

void MainPage::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
    // Handling of this event is included for completeness, as it will only fire when navigating between pages and this sample only includes one page
    CleanupCameraAsync();
    CleanupUiAsync();
}