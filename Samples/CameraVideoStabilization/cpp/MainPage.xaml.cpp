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

using namespace CameraVideoStabilization;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media;
using namespace Windows::Phone::UI::Input;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

MainPage::MainPage()
    : _mediaCapture(nullptr)
    , _isInitialized(false)
    , _isPreviewing(false)
    , _isRecording(false)
    , _encodingProfile(nullptr)
    , _externalCamera(false)
    , _mirroringPreview(false)
    , _deviceOrientation(SimpleOrientation::NotRotated)
    , _displayOrientation(DisplayOrientations::Portrait)
    , _displayRequest(ref new Windows::System::Display::DisplayRequest())
    , RotationKey({ 0xC380465D, 0x2271, 0x428C,{ 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1 } })
    , _videoStabilizationEffect(nullptr)
    , _inputPropertiesBackup(nullptr)
    , _outputPropertiesBackup(nullptr)
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
    return FindCameraDeviceByPanelAsync(Windows::Devices::Enumeration::Panel::Back)
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
            // Set up the encoding profile for video recording
            _encodingProfile = MediaProperties::MediaEncodingProfile::CreateMp4(MediaProperties::VideoEncodingQuality::Auto);
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
            catch (AccessDeniedException^)
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
            auto stopRecordTask = create_task(StopRecordingAsync());
            taskList.push_back(stopRecordTask);
        }

        // Clear the effect if one exists
        if (_videoStabilizationEffect != nullptr)
        {
            auto cleanStabilizationTask = CleanUpVideoStabilizationEffectAsync();
            taskList.push_back(cleanStabilizationTask);
        }

        if (_isPreviewing)
        {
            auto stopPreviewTask = create_task(StopPreviewAsync());
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
        _isPreviewing = true;

        // Only need to update the orientation if the camera is mounted on the device
        if (!_externalCamera)
        {
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
    _isPreviewing = false;

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
/// Adds video stabilization to the video record stream, registers for its event, enables it, and gets the effect instance
/// </summary>
/// <returns></returns>
task<void> MainPage::CreateVideoStabilizationEffectAsync()
{
    // No work to be done if there already is an effect
    if (_videoStabilizationEffect != nullptr)
    {
        return EmptyTask();
    }

    // Create the definition, which will contain some initialization settings
    auto definition = ref new Core::VideoStabilizationEffectDefinition();

    // Add the effect to the video record stream
    return create_task(_mediaCapture->AddVideoEffectAsync(definition, Capture::MediaStreamType::VideoRecord))
        .then([this](IMediaExtension^ mediaExtension)
    {
        WriteLine("VS effect added to pipeline");

        _videoStabilizationEffect = static_cast<Core::VideoStabilizationEffect^>(mediaExtension);

        // Subscribe to notifications for changes in the enabled state for the effect
        _videoStabilizationEnabledToken = 
            _videoStabilizationEffect->EnabledChanged += ref new TypedEventHandler<Core::VideoStabilizationEffect^, Core::VideoStabilizationEffectEnabledChangedEventArgs^>(this, &MainPage::VideoStabilizationEffect_EnabledChanged);

        // Enable the effect
        _videoStabilizationEffect->Enabled = true;
    
#if (USE_VS_RECOMMENDATION)
        // Configure the pipeline to use the optimal settings for VS
        return SetUpVideoStabilizationRecommendationAsync();

        // At this point, one of two things has happened:
        //
        // a) If a more suitable capture resolution was available:
        //    1. Such resolution will have been set up for video capture ("input")
        //    2. The MediaEncodingProfile ("output") will have been changed to specify dimensions reflecting the amount of cropping
        //       done on said capture resolution (possibly even none if the new resolution offers enough padding)
        // b) If no better suited capture resolution was available:
        //    1. The video capture resolution will not have changed
        //    2. The MediaEncodingProfile will have been changed to specify smaller dimensions than the capture resolution
        //       so that the video isn't scaled back up to the capture resolution after cropping, which could cause a loss in quality
#else
        WriteLine("Not setting up VS recommendation");
        return EmptyTask();
        // Not setting up VS recommendation means that the video will be captured at the desired resolution,
        // then cropped by the VS effect as part of the stabilization process, and then scaled back up to the
        // original capture resolution
#endif
    });
}

/// <summary>
/// Configures the pipeline to use the optimal resolutions for VS based on the settings currently in use
/// </summary>
/// <returns></returns>
task<void> MainPage::SetUpVideoStabilizationRecommendationAsync()
{
    WriteLine("Setting up VS recommendation...");

    // Get the recommendation from the effect based on our current input and output configuration
    auto recommendation = _videoStabilizationEffect->GetRecommendedStreamConfiguration(_mediaCapture->VideoDeviceController, _encodingProfile->Video);

    // Handle the recommendations for the output from the effect
    if (recommendation->OutputProperties != nullptr)
    {
        // Back up the current output properties from before VS was activated
        _outputPropertiesBackup = _encodingProfile->Video;

        // Apply the recommended encoding profile for the output, which will result in a video with the same dimensions as configured
        // before VideoStabilization was added if an appropriate padded capture resolution was available. Otherwise, it will be slightly
        // smaller (due to cropping). This prevents upscaling back to the original size, which can result in a loss of quality
        _encodingProfile->Video = recommendation->OutputProperties;
        WriteLine("VS recommendation for the MediaEncodingProfile (output) has been applied");
    }

    // Handle the recommendation for the input into the effect, which can contain a larger resolution than currently configured, so cropping is minimized
    if (recommendation->InputProperties != nullptr)
    {
        // Back up the current input properties from before VS was activated
        _inputPropertiesBackup = static_cast<MediaProperties::VideoEncodingProperties^>(_mediaCapture->VideoDeviceController->GetMediaStreamProperties(Capture::MediaStreamType::VideoRecord));

        // Set the recommendation from the effect (a resolution higher than the current one to allow for cropping) on the input
        return create_task(_mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(Capture::MediaStreamType::VideoRecord, recommendation->InputProperties))
            .then([this]()
        {
            WriteLine("VS recommendation for the MediaStreamProperties (input) has been applied");
        });
    }
    else
    {
        return EmptyTask();
    }
}

/// <summary>
///  Disables and removes the video stabilization effect, and unregisters the event handler for the EnabledChanged event of the effect
/// </summary>
/// <returns></returns>
task<void> MainPage::CleanUpVideoStabilizationEffectAsync()
{
    // No work to be done if there is no effect
    if (_videoStabilizationEffect == nullptr)
    {
        return EmptyTask();
    }

    // Disable the effect
    _videoStabilizationEffect->Enabled = false;

    _videoStabilizationEffect->EnabledChanged -= _videoStabilizationEnabledToken;

    // Remove the effect (see ClearEffectsAsync method to remove all effects from a stream)
    return create_task(_mediaCapture->RemoveEffectAsync(_videoStabilizationEffect))
        .then([this]()
    {
        WriteLine("VS effect removed from pipeline");

        // If backed up settings (stream properties and encoding profile) exist, restore them and clear the backups
        if (_outputPropertiesBackup != nullptr)
        {
            _encodingProfile->Video = _outputPropertiesBackup;
            _outputPropertiesBackup = nullptr;
        }

        task<void> taskToExecute;
        if (_inputPropertiesBackup != nullptr)
        {
            taskToExecute = create_task(_mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(Capture::MediaStreamType::VideoRecord, _inputPropertiesBackup));
            _inputPropertiesBackup = nullptr;
        }
        else
        {
            taskToExecute = EmptyTask();
        }

        return taskToExecute;
    }).then([this]()
    {
        // Clear the member variable that held the effect instance
        _videoStabilizationEffect = nullptr;
    });
}

/// <summary>
/// Records an MP4 video to a StorageFile and adds rotation metadata to it
/// </summary>
/// <returns></returns>
task<void> MainPage::StartRecordingAsync()
{
    // Create storage file for the capture
    return create_task(_captureFolder->CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption::GenerateUniqueName))
        .then([this](StorageFile^ file)
    {
        // Calculate rotation angle, taking mirroring into account if necessary
        auto rotationAngle = 360 - ConvertDeviceOrientationToDegrees(GetCameraOrientation());

        // Add it to the encoding profile, or edit the value if the GUID was already a part of the properties
        _encodingProfile->Video->Properties->Insert(RotationKey, rotationAngle);

        return create_task(_mediaCapture->StartRecordToStorageFileAsync(_encodingProfile, file))
            .then([this, file]()
        {
            _isRecording = true;
            WriteLine("Started recording to: " + file->Path);
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

        // If VS disabled itself during recording, clean up the effect
        if (_videoStabilizationEffect != nullptr && !_videoStabilizationEffect->Enabled)
        {
            // Cleaning up the effect here is an implementation choice in this sample, so that, at the end of the recording, it's clearer
            // to the user that the effect turned itself off. Alternatively, one could try to re-enable the effect (e.g. if the reason
            // it disabled itself was RunningSlowly, there's a chance it will work next time), and/or prompt the user to choose a lower capture
            // resolution / framerate instead.
            return CleanUpVideoStabilizationEffectAsync();
        }
        else
        {
            return EmptyTask();
        }
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
/// This method will update the icons, enable/disable and show/hide the photo/video buttons depending on the current state of the app and the capabilities of the device
/// </summary>
void MainPage::UpdateCaptureControls()
{
    // The button should only be enabled if the preview started sucessfully
    VideoButton->IsEnabled = _isPreviewing;

    // Allow toggling VS only while not recording or when the effect exists but it has been disabled
    VsToggleButton->IsEnabled = (_isPreviewing && !_isRecording) ||
        (_videoStabilizationEffect != nullptr && !_videoStabilizationEffect->Enabled);

    // Update recording button to show "Stop" icon instead of red "Record" icon
    StartRecordingIcon->Visibility = _isRecording ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
    StopRecordingIcon->Visibility = _isRecording ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;

    // Show "VS On" icon when effect exists and is enabled, otherwise show "VS Off icon"
    if (_videoStabilizationEffect != nullptr && _videoStabilizationEffect->Enabled)
    {
        VsOnIcon->Visibility = Windows::UI::Xaml::Visibility::Visible;
        VsOffIcon->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    else
    {
        VsOnIcon->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        VsOffIcon->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

/// <summary>
/// Attempts to lock the page orientation, hide the StatusBar (on Phone) and registers event handlers for orientation sensors
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
/// Unregisters event handlers for orientation sensors, allows the StatusBar (on Phone) to show, and removes the page orientation lock
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
            result = SimpleOrientation::Rotated270DegreesCounterclockwise;
            break;
        }
    }

    // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
    if (_mirroringPreview)
    {
        // This only affects the 90 and 270 degree cases, because rotating 0 and 180 degrees is the same clockwise and counter-clockwise
        switch (_deviceOrientation)
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
    auto angle = (display + device) % 360;

    // Rotate the buttons in the UI to match the rotation of the device
    auto transform = ref new RotateTransform();
    transform->Angle = angle;

    // The RenderTransform is safe to use (i.e. it won't cause layout issues) in this case, because these buttons have a 1:1 aspect ratio
    VsToggleButton->RenderTransform = transform;
    VideoButton->RenderTransform = transform;
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
    // This event will fire when the page is rotated, when the DisplayInformation.AutoRotationPreferences
    // value set in the SetupUiAsync() method cannot be not honored
    _displayOrientation = sender->CurrentOrientation;

    if (_isPreviewing)
    {
        SetPreviewRotationAsync();
    }

    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        UpdateButtonOrientation();
    }));
}

void MainPage::VsToggleButton_Click(Object^, Windows::UI::Xaml::RoutedEventArgs^)
{
    // Note that for the most part, this button is disabled during recording, except when VS is turned off automatically
    task<void> taskToExecute;

    if (!_isRecording)
    {
        // While *not* recording, either the effect doesn't exist (and should be created) or it exists and is enabled.
        // This sample doesn't allow the user to disable the effect while recording. For the case that the effect disables
        // itself while recording, StopRecordingAsync() will clean up the effect after the recording session ends.

        if (_videoStabilizationEffect == nullptr)
        {
            // VS didn't exist, so create and enable the effect
            taskToExecute = CreateVideoStabilizationEffectAsync();
        }
        else if (_videoStabilizationEffect->Enabled)
        {
            // VS was enabled, so clean it up and restore any capture settings (MediaStreamType and EncodingProfile)
            taskToExecute = CleanUpVideoStabilizationEffectAsync();
        }
    }
    else if (!_videoStabilizationEffect->Enabled)
    {
        // While recording this button is diabled. The situation the user is allowed to tap it while recording, is when
        // VS turns itself off (see VideoStabilizationEffect_EnabledChanged), so proceed to re-enable the effect
        _videoStabilizationEffect->Enabled = true;
        taskToExecute = EmptyTask();
    }

    taskToExecute.then([this](task<void> previousTask)
    {
        Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            UpdateCaptureControls();
        }));
    });
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
/// Triggers when the Enabled property of the VideoStabilizationEffect changes.
/// </summary>
/// <param name="sender">The instance of the effect firing the event.</param>
/// <param name="args">Information about the event.</param>
void MainPage::VideoStabilizationEffect_EnabledChanged(Windows::Media::Core::VideoStabilizationEffect^ sender, Windows::Media::Core::VideoStabilizationEffectEnabledChangedEventArgs^ args)
{
    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([sender, args, this]()
    {
        WriteLine("VS Enabled: " + sender->Enabled.ToString() + ". Reason: " + args->Reason.ToString());
        UpdateCaptureControls();
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