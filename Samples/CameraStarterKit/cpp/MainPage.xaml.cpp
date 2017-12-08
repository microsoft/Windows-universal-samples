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
using namespace Platform;
using namespace CameraStarterKit;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
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
    , _externalCamera(false)
    , _mirroringPreview(false)
    , _displayRequest(ref new Windows::System::Display::DisplayRequest())
    , RotationKey({ 0xC380465D, 0x2271, 0x428C,{ 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1 } })
    , _captureFolder(nullptr)
    , _rotationHelper(nullptr)
{
    InitializeComponent();

    _systemMediaControls = SystemMediaTransportControls::GetForCurrentView();

    // Do not cache the state of the UI when suspending/navigating
    Page::NavigationCacheMode = Navigation::NavigationCacheMode::Disabled;
}

MainPage::~MainPage()
{
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

        // Initialize rotationHelper
        _rotationHelper = ref new CameraRotationHelper(camera->EnclosureLocation);
        _orientationChangedEventToken = _rotationHelper->OrientationChanged += ref new EventHandler<bool>(this, &MainPage::RotationHelper_OrientationChanged);

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

        if (_rotationHelper != nullptr)
        {
            _rotationHelper->OrientationChanged -= _orientationChangedEventToken;
            _rotationHelper = nullptr;
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
    // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
    auto rotation = _rotationHelper->GetCameraPreviewOrientation();
    auto props = _mediaCapture->VideoDeviceController->GetMediaStreamProperties(Capture::MediaStreamType::VideoPreview);
    props->Properties->Insert(RotationKey, CameraRotationHelper::ConvertSimpleOrientationToClockwiseDegrees(rotation));
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

        auto photoOrientation = CameraRotationHelper::ConvertSimpleOrientationToPhotoOrientation(_rotationHelper->GetCameraCaptureOrientation());
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
    // Create storage file for the capture
    return create_task(_captureFolder->CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption::GenerateUniqueName))
        .then([this](StorageFile^ file)
    {
        // Calculate rotation angle, taking mirroring into account if necessary
        auto rotationAngle = CameraRotationHelper::ConvertSimpleOrientationToClockwiseDegrees(_rotationHelper->GetCameraCaptureOrientation());
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
    PhotoButton->IsEnabled = _isPreviewing;
    VideoButton->IsEnabled = _isPreviewing;

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
/// Initialize or clean up the camera and our UI,
/// depending on the page state.
/// </summary>
/// <returns></returns>
Concurrency::task<void> MainPage::SetUpBasedOnStateAsync()
{
    // Avoid reentrancy: Wait until nobody else is in this function.
    while (!_setupTask.is_done())
    {
        return _setupTask.then([this]() { return SetUpBasedOnStateAsync(); },
            task_continuation_context::get_current_winrt_context());
    }

    // We want our UI to be active if
    // * We are the current active page.
    // * The window is visible.
    // * The app is not suspending.
    bool wantUIActive = _isActivePage && Window::Current->Visible && !_isSuspending;

    if (_isUIActive != wantUIActive)
    {
        _isUIActive = wantUIActive;

        if (wantUIActive)
        {
            _setupTask = SetupUiAsync().then([this]()
            {
                return InitializeCameraAsync();
            }, task_continuation_context::get_current_winrt_context());
        }
        else
        {
            _setupTask = CleanupCameraAsync().then([this]()
            {
                return CleanupUiAsync();
            }, task_continuation_context::get_current_winrt_context());
        }
    }
    return _setupTask;
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
        return task_from_result();
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
        return task_from_result();
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
/// Uses the current device orientation in space and page orientation on the screen to calculate the rotation
/// transformation to apply to the controls
/// </summary>
void MainPage::UpdateButtonOrientation()
{
    // Rotate the buttons in the UI to match the rotation of the device
    auto angle = CameraRotationHelper::ConvertSimpleOrientationToClockwiseDegrees(_rotationHelper->GetUIOrientation());

    // Rotate the buttons in the UI to match the rotation of the device
    auto transform = ref new RotateTransform();
    transform->Angle = angle;

    // The RenderTransform is safe to use (i.e. it won't cause layout issues) in this case, because these buttons have a 1:1 aspect ratio
    PhotoButton->RenderTransform = transform;
    VideoButton->RenderTransform = transform;
}

void MainPage::Application_Suspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e)
{
    _isSuspending = true;

    auto deferral = e->SuspendingOperation->GetDeferral();
    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([this, deferral]()
    {
        SetUpBasedOnStateAsync().then([deferral]()
        {
            deferral->Complete();
        });
    }));
}

void MainPage::Application_Resuming(Platform::Object^ sender, Platform::Object^ args)
{
    _isSuspending = false;

    Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        SetUpBasedOnStateAsync();
    }));
}

void MainPage::Window_VisibilityChanged(Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e)
{
    SetUpBasedOnStateAsync();
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
/// Handles an orientation changed event
/// </summary>
void MainPage::RotationHelper_OrientationChanged(Object^, bool updatePreview)
{
    Concurrency::task<void> task = create_task([]() {});
    if (updatePreview)
    {
        task = create_task(SetPreviewRotationAsync());
    }

    task.then([this]()
    {
        Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            UpdateButtonOrientation();
        }));
    });
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

void MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{

    // Useful to know when to initialize/clean up the camera
    _applicationSuspendingEventToken =
        Application::Current->Suspending += ref new SuspendingEventHandler(this, &MainPage::Application_Suspending);
    _applicationResumingEventToken =
        Application::Current->Resuming += ref new EventHandler<Object^>(this, &MainPage::Application_Resuming);
    _windowVisibilityChangedEventToken =
        Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &MainPage::Window_VisibilityChanged);

    _isActivePage = true;
    SetUpBasedOnStateAsync();
}

void MainPage::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
    // Handling of this event is included for completeness, as it will only fire when navigating between pages and this sample only includes one page
    Application::Current->Suspending -= _applicationSuspendingEventToken;
    Application::Current->Resuming -= _applicationResumingEventToken;
    Window::Current->VisibilityChanged -= _windowVisibilityChangedEventToken;

    _isActivePage = false;
    SetUpBasedOnStateAsync();
}