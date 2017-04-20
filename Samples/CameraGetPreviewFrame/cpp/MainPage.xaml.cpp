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
#include <MemoryBuffer.h>   // IMemoryBufferByteAccess
#include <sstream>

using namespace Concurrency;
using namespace CameraGetPreviewFrame;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media;
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
    , _externalCamera(false)
    , _mirroringPreview(false)
    , _displayOrientation(DisplayOrientations::Portrait)
    , _displayRequest(ref new Windows::System::Display::DisplayRequest())
    , RotationKey({ 0xC380465D, 0x2271, 0x428C,{ 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1 } })
    , _captureFolder(nullptr)
{
    InitializeComponent();

    _displayInformation = DisplayInformation::GetForCurrentView();
    _systemMediaControls = SystemMediaTransportControls::GetForCurrentView();

    // Cache the UI to have the checkboxes retain their state, as the enabled/disabled state of the
    // GetPreviewFrameButton is reset in code when suspending/navigating (see Start/StopPreviewAsync)
    Page::NavigationCacheMode = Navigation::NavigationCacheMode::Required;

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
    _systemMediaControls->PropertyChanged -= _mediaControlPropChangedEventToken;
}

/// <summary>
/// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, and starts preview
/// </summary>
/// <returns></returns>
task<void> MainPage::InitializeCameraAsync()
{
    WriteLine("InitializeCameraAsync");

    // Attempt to get the back camera if one is available, but use any camera device if not
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

        // Register for a notification when something goes wrong
        _mediaCaptureFailedEventToken =
            _mediaCapture->Failed += ref new Capture::MediaCaptureFailedEventHandler(this, &MainPage::MediaCapture_Failed);

        auto settings = ref new Capture::MediaCaptureInitializationSettings();
        settings->VideoDeviceId = camera->Id;

        // Initialize media capture and start the preview
        create_task(_mediaCapture->InitializeAsync(settings))
            .then([this]()
        {
            _isInitialized = true;
            
            return StartPreviewAsync();
        // Different return types, must do the error checking here since we cannot return and send
        // execeptions back up the chain.
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
    }).then([this]()
    {
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
    });
}

/// <summary>
/// Cleans up the camera resources (after stopping the preview if necessary) and unregisters from MediaCapture events
/// </summary>
/// <returns></returns>
task<void> MainPage::CleanupCameraAsync()
{
    WriteLine("CleanupCameraAsync");

    std::vector<task<void>> taskList;

    if (_isInitialized)
    {
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
        if (_mediaCapture.Get() != nullptr)
        {
            _mediaCapture->Failed -= _mediaCaptureFailedEventToken;
            _mediaCapture = nullptr;
        }
    });
}

/// <summary>
/// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on and unlocks the UI
/// </summary>
/// <returns></returns>
task<void> MainPage::StartPreviewAsync()
{
    WriteLine("StartPreviewAsync");

    // Prevent the device from sleeping while the preview is running
    _displayRequest->RequestActive();

    // Register to listen for media property changes
    _mediaControlPropChangedEventToken =
        _systemMediaControls->PropertyChanged += ref new TypedEventHandler<SystemMediaTransportControls^, SystemMediaTransportControlsPropertyChangedEventArgs^>(this, &MainPage::SystemMediaControls_PropertyChanged);

    // Set the preview source in the UI and mirror it if necessary
    PreviewControl->Source = _mediaCapture.Get();
    PreviewControl->FlowDirection = _mirroringPreview ? Windows::UI::Xaml::FlowDirection::RightToLeft : Windows::UI::Xaml::FlowDirection::LeftToRight;

    // Start the preview
    return create_task(_mediaCapture->StartPreviewAsync())
        .then([this](task<void> previousTask)
    {
        _isPreviewing = true;
        GetPreviewFrameButton->IsEnabled = _isPreviewing;

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
/// Gets the current orientation of the UI in relation to the device and applies a corrective rotation to the preview
/// </summary>
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
            PreviewControl->Source = nullptr;
            // Allow the device screen to sleep now that the preview is stopped
            _displayRequest->RequestRelease();

            GetPreviewFrameButton->IsEnabled = _isPreviewing;
        }));
    });
}

/// <summary>
/// Gets the current preview frame as a SoftwareBitmap, displays its properties in a TextBlock, and can optionally display the image
/// in the UI and/or save it to disk as a jpg
/// </summary>
/// <returns></returns>
task<void> MainPage::GetPreviewFrameAsSoftwareBitmapAsync()
{
    // Get information about the preview
    auto previewProperties = static_cast<MediaProperties::VideoEncodingProperties^>(_mediaCapture->VideoDeviceController->GetMediaStreamProperties(Capture::MediaStreamType::VideoPreview));
    unsigned int videoFrameWidth = previewProperties->Width;
    unsigned int videoFrameHeight = previewProperties->Height;

    // Create the video frame to request a SoftwareBitmap preview frame
    auto videoFrame = ref new VideoFrame(BitmapPixelFormat::Bgra8, videoFrameWidth, videoFrameHeight);

    // Capture the preview frame
    return create_task(_mediaCapture->GetPreviewFrameAsync(videoFrame))
        .then([this](VideoFrame^ currentFrame)
    {
        // Collect the resulting frame
        auto previewFrame = currentFrame->SoftwareBitmap;

        // Show the frame information
        std::wstringstream ss;
        ss << previewFrame->PixelWidth << "x" << previewFrame->PixelHeight << " " << previewFrame->BitmapPixelFormat.ToString()->Data();
        FrameInfoTextBlock->Text = ref new String(ss.str().c_str());

        // Add a simple green filter effect to the SoftwareBitmap
        if (GreenEffectCheckBox->IsChecked->Value)
        {
            ApplyGreenFilter(previewFrame);
        }

        std::vector<task<void>> taskList;

        // Show the frame (as is, no rotation is being applied)
        if (ShowFrameCheckBox->IsChecked->Value == true)
        {
            // Copy it to a WriteableBitmap to display it to the user
            auto sbSource = ref new Media::Imaging::SoftwareBitmapSource();
            taskList.push_back(create_task(sbSource->SetBitmapAsync(previewFrame))
                .then([this, sbSource]() 
            {
                // Display it in the Image control
                PreviewFrameImage->Source = sbSource;
            }));
        }

        // Save the frame (as is, no rotation is being applied)
        if (SaveFrameCheckBox->IsChecked->Value == true)
        {
            taskList.push_back(SaveSoftwareBitmapAsync(previewFrame));
        }

        return when_all(taskList.begin(), taskList.end()).then([currentFrame]() {
            // IClosable.Close projects into CX as operator delete.
            delete currentFrame;
        });
    });
}

/// <summary>
/// Gets the current preview frame as a Direct3DSurface and displays its properties in a TextBlock
/// </summary>
/// <returns></returns>
task<void> MainPage::GetPreviewFrameAsD3DSurfaceAsync()
{
    // Capture the preview frame as a D3D surface
    return create_task(_mediaCapture->GetPreviewFrameAsync())
        .then([this](VideoFrame^ currentFrame)
    {
        std::wstringstream ss;

        // Check that the Direct3DSurface isn't null. It's possible that the device does not support getting the frame
        // as a D3D surface
        if (currentFrame->Direct3DSurface != nullptr)
        {
            // Collect the resulting frame
            auto surface = currentFrame->Direct3DSurface;

            // Show the frame information
            ss << surface->Description.Width << "x" << surface->Description.Height << " " << surface->Description.Format.ToString()->Data();
            FrameInfoTextBlock->Text = ref new String(ss.str().c_str());
        }
        else // Fall back to software bitmap
        {
            // Collect the resulting frame
            auto previewFrame = currentFrame->SoftwareBitmap;

            // Show the frame information
            ss << previewFrame->PixelWidth << "x" << previewFrame->PixelHeight << " " << previewFrame->BitmapPixelFormat.ToString()->Data();
            FrameInfoTextBlock->Text = ref new String(ss.str().c_str());
        }

        // Clear the image
        PreviewFrameImage->Source = nullptr;

        // IClosable.Close projects into CX as operator delete.
        delete currentFrame;
    });
}

/// <summary>
/// Saves a SoftwareBitmap to the _captureFolder
/// </summary>
/// <param name="bitmap"></param>
/// <returns></returns>
task<void> MainPage::SaveSoftwareBitmapAsync(SoftwareBitmap^ bitmap)
{
    return create_task(_captureFolder->CreateFileAsync("PreviewFrame.jpg", CreationCollisionOption::GenerateUniqueName))
        .then([bitmap](StorageFile^ file)
    {
        return create_task(file->OpenAsync(FileAccessMode::ReadWrite));
    }).then([this, bitmap](Streams::IRandomAccessStream^ outputStream)
    {
        return create_task(BitmapEncoder::CreateAsync(BitmapEncoder::JpegEncoderId, outputStream))
            .then([bitmap](BitmapEncoder^ encoder)
        {
            // Grab the data from the SoftwareBitmap
            encoder->SetSoftwareBitmap(bitmap);
            return create_task(encoder->FlushAsync());
        }).then([this, outputStream](task<void> previousTask)
        {
            // IClosable.Close projects into CX as operator delete.
            delete outputStream;
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
    });
}


/// <summary>
/// Applies a basic effect to a Bgra8 SoftwareBitmap in-place
/// </summary>
/// <param name="bitmap">SoftwareBitmap that will receive the effect</param>
void MainPage::ApplyGreenFilter(SoftwareBitmap^ bitmap)
{
    // Effect is hard-coded to operate on BGRA8 format only
    if (bitmap->BitmapPixelFormat == BitmapPixelFormat::Bgra8)
    {
        // In BGRA8 format, each pixel is defined by 4 bytes
        const int BYTES_PER_PIXEL = 4;

        BitmapBuffer^ buffer = bitmap->LockBuffer(BitmapBufferAccessMode::ReadWrite);
        IMemoryBufferReference^ reference = buffer->CreateReference();

        Microsoft::WRL::ComPtr<IMemoryBufferByteAccess> byteAccess;
        if (SUCCEEDED(reinterpret_cast<IUnknown*>(reference)->QueryInterface(IID_PPV_ARGS(&byteAccess))))
        {
            // Get a pointer to the pixel buffer
            byte* data;
            unsigned capacity;
            byteAccess->GetBuffer(&data, &capacity);

            // Get information about the BitmapBuffer
            auto desc = buffer->GetPlaneDescription(0);

            // Iterate over all pixels
            for (int row = 0; row < desc.Height; row++)
            {
                for (int col = 0; col < desc.Width; col++)
                {
                    // Index of the current pixel in the buffer (defined by the next 4 bytes, BGRA8)
                    auto currPixel = desc.StartIndex + desc.Stride * row + BYTES_PER_PIXEL * col;

                    // Read the current pixel information into b,g,r channels (leave out alpha channel)
                    auto b = data[currPixel + 0]; // Blue
                    auto g = data[currPixel + 1]; // Green
                    auto r = data[currPixel + 2]; // Red

                    // Boost the green channel, leave the other two untouched
                    data[currPixel + 0] = b;
                    data[currPixel + 1] = min(g + 80, 255);
                    data[currPixel + 2] = r;
                }
            }
        }
        delete reference;
        delete buffer;
    }
}

/// <summary>
/// Queries the available video capture devices to try and find one mounted on the desired panel
/// </summary>
/// <param name="desiredPanel">The panel on the device that the desired camera is mounted on</param>
/// <returns>A DeviceInformation instance with a reference to the camera mounted on the desired panel if available,
///          any other camera if not, or null if no camera is available.</returns>
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

void MainPage::Application_Suspending(Object^, Windows::ApplicationModel::SuspendingEventArgs^ e)
{
    // Handle global application events only if this page is active
    if (Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPage::typeid).Name)
    {
        _displayInformation->OrientationChanged -= _displayInformationEventToken;
        auto deferral = e->SuspendingOperation->GetDeferral();
        CleanupCameraAsync()
        .then([this, deferral]()
        {
            deferral->Complete();
        });
    }
}

void MainPage::Application_Resuming(Object^, Object^)
{
    // Handle global application events only if this page is active
    if (Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPage::typeid).Name)
    {
        // Populate orientation variables with the current state and register for future changes
        _displayOrientation = _displayInformation->CurrentOrientation;
        _displayInformationEventToken =
            _displayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::DisplayInformation_OrientationChanged);

        InitializeCameraAsync();
    }
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
/// This event will fire when the page is rotated
/// </summary>
/// <param name="sender">The event source.</param>
/// <param name="args">The event data.</param>
void MainPage::DisplayInformation_OrientationChanged(DisplayInformation^ sender, Object^)
{
    _displayOrientation = sender->CurrentOrientation;
    
    if (_isPreviewing)
    {
        SetPreviewRotationAsync();
    }
}

void MainPage::GetPreviewFrameButton_Click(Object^, RoutedEventArgs^)
{
    // If preview is not running, no preview frames can be acquired
    if (!_isPreviewing) return;

    if ((ShowFrameCheckBox->IsChecked->Value == true) || (SaveFrameCheckBox->IsChecked->Value == true))
    {
        GetPreviewFrameAsSoftwareBitmapAsync();
    }
    else
    {
        GetPreviewFrameAsD3DSurfaceAsync();
    }
}

void MainPage::MediaCapture_Failed(Capture::MediaCapture^, Capture::MediaCaptureFailedEventArgs^ errorEventArgs)
{
    std::wstringstream ss;
    ss << "MediaCapture_Failed: 0x" << errorEventArgs->Code << ": " << errorEventArgs->Message->Data();
    WriteLine(ref new String(ss.str().c_str()));

    CleanupCameraAsync()
        .then([this]()
    {
        Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            GetPreviewFrameButton->IsEnabled = _isPreviewing;
        }));
    });
}

void MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^)
{
    // Populate orientation variables with the current state and register for future changes
    _displayOrientation = _displayInformation->CurrentOrientation;
    _displayInformationEventToken =
        _displayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::DisplayInformation_OrientationChanged);

    InitializeCameraAsync();
}

void MainPage::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^)
{
    // Handling of this event is included for completeness, as it will only fire when navigating between pages and this sample only includes one page
    CleanupCameraAsync();

    _displayInformation->OrientationChanged -= _displayInformationEventToken;
}