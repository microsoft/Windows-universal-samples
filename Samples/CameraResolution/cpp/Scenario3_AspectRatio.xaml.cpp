//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario3_AspectRatio.xaml.h"
#include "StreamResolutionHelper.h"
#include <sstream>
#include <math.h>

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes a new instance of the <see cref="Scenario3_AspectRatio"/> class.
/// </summary>
Scenario3_AspectRatio::Scenario3_AspectRatio()
    : _rootPage(MainPage::Current)
    , _mediaCapture(nullptr)
    , _isPreviewing(false)
    , _isRecording(false)
    , _captureFolder(nullptr)
{
    InitializeComponent();
}

Scenario3_AspectRatio::~Scenario3_AspectRatio()
{
    CleanupCameraAsync();
}

void Scenario3_AspectRatio::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^)
{
    CleanupCameraAsync();
}

/// <summary>
/// Initializes the camera
/// </summary>
task<void> Scenario3_AspectRatio::InitializeCameraAsync()
{
    _mediaCapture = ref new MediaCapture();
    _mediaCaptureFailedEventToken =
        _mediaCapture->Failed += ref new MediaCaptureFailedEventHandler(this, &Scenario3_AspectRatio::MediaCapture_Failed);

    return create_task(_mediaCapture->InitializeAsync())
        .then([this]()
    {
        PreviewControl->Source = _mediaCapture.Get();
        return create_task(_mediaCapture->StartPreviewAsync());
    }).then([this]()
    {
        return create_task(StorageLibrary::GetLibraryAsync(KnownLibraryId::Pictures));
    }).then([this](StorageLibrary^ picturesLibrary)
    {
        _captureFolder = picturesLibrary->SaveFolder;
        if (_captureFolder == nullptr)
        {
            // In this case fall back to the local app storage since the Pictures Library is not available
            _captureFolder = ApplicationData::Current->LocalFolder;
        }
    }).then([this](task<void> previousTask)
    {
        try
        {
            _isPreviewing = true;
            previousTask.get();
            return previousTask;
        }
        catch (AccessDeniedException^)
        {
            _rootPage->Current->NotifyUser("The app was denied access to the camera", NotifyType::ErrorMessage);
            return CleanupCameraAsync();
        }
    });
}

/// <summary>
/// Cleans up the camera and state variables
/// </summary>
task<void> Scenario3_AspectRatio::CleanupCameraAsync()
{
    std::list<task<void>> taskList;
    if (_isRecording)
    {
        auto stopRecordTask = create_task(_mediaCapture->StopRecordAsync());
        taskList.push_back(stopRecordTask);
        _isRecording = false;
    }
    if (_isPreviewing)
    {
        auto stopPreviewTask = create_task(_mediaCapture->StopPreviewAsync());
        taskList.push_back(stopPreviewTask);
        _isPreviewing = false;
    }

    return when_all(taskList.begin(), taskList.end()).then([this]()
    {
        if (_mediaCapture != nullptr)
        {
            _mediaCapture->Failed -= _mediaCaptureFailedEventToken;
            _mediaCapture = nullptr;
        }
    });
}

/// <summary>
/// Initializes the camera and populates the UI
/// </summary>
/// <param name="sender"></param>
void Scenario3_AspectRatio::InitializeCameraButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
    Button^ button = static_cast<Button^>(sender);
    button->IsEnabled = false;

    create_task(InitializeCameraAsync())
        .then([this, button]()
    {
        if (_isPreviewing)
        {
            button->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            PreviewControl->Visibility = Windows::UI::Xaml::Visibility::Visible;
            CheckIfStreamsAreIdentical();
            PopulateComboBoxes();
            VideoButton->IsEnabled = true;
        }
    });
}

/// <summary>
/// Records an MP4 video to a StorageFile
/// </summary>
void Scenario3_AspectRatio::VideoButton_Click(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^)
{
    if (_isPreviewing)
    {
        if (!_isRecording)
        {
            // Create a storage file and begin recording
            create_task(_captureFolder->CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption::GenerateUniqueName))
                .then([this](StorageFile^ file)
            {
                return create_task(_mediaCapture->StartRecordToStorageFileAsync(MediaEncodingProfile::CreateMp4(VideoEncodingQuality::Auto), file))
                    .then([this, file]()
                {
                    // Reflect changes in the UI
                    VideoButton->Content = "Stop Video";
                    _isRecording = true;

                    _rootPage->Current->NotifyUser("Recording file, saving to: " + file->Path, NotifyType::StatusMessage);
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
                    _rootPage->Current->NotifyUser(L"Exception when starting video recording: " + ex->Message, NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            // Reflect the changes in the UI and stop recording
            VideoButton->Content = "Record Video";
            _isRecording = false;

            create_task(_mediaCapture->StopRecordAsync())
                .then([this]()
            {
                _rootPage->Current->NotifyUser("Stopped recording", NotifyType::StatusMessage);
            });
        }
    }
}

/// <summary>
///  Event handler for Preview settings combo box. Updates stream resolution based on the selection.
/// </summary>
/// <param name="sender"></param>
void Scenario3_AspectRatio::PreviewSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
    if (_isPreviewing)
    {
        auto selectedItem = static_cast<ComboBoxItem^>(static_cast<ComboBox^>(sender)->SelectedItem);
        auto encodingProperties = static_cast<IMediaEncodingProperties^>(selectedItem->Tag);
        create_task(_mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(MediaStreamType::VideoPreview, encodingProperties));

        // The preview just changed, update the video combo box
        MatchPreviewAspectRatio();
    }
}

/// <summary>
///  Event handler for Record settings combo box. Updates stream resolution based on the selection.
/// </summary>
/// <param name="sender"></param>
void Scenario3_AspectRatio::VideoSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
    if (_isPreviewing)
    {
        auto selectedItem = static_cast<ComboBoxItem^>(static_cast<ComboBox^>(sender)->SelectedItem);
        auto encodingProperties = static_cast<IMediaEncodingProperties^>(selectedItem->Tag);
        create_task(_mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(MediaStreamType::VideoRecord, encodingProperties));
    }
}

/// <summary>
/// Populates the combo boxes with preview settings and matching ratio settings for the video stream
/// </summary>
void Scenario3_AspectRatio::PopulateComboBoxes()
{
    // Query all properties of the device
    auto allProperties = _mediaCapture->VideoDeviceController->GetAvailableMediaStreamProperties(MediaStreamType::VideoPreview);

    // Populate the preview combo box with the entries
    for (auto encodingProperties : allProperties)
    {
        ComboBoxItem^ comboBoxItem = ref new ComboBoxItem();
        auto streamHelper = ref new StreamResolutionHelper(encodingProperties);
        comboBoxItem->Content = streamHelper->GetFriendlyName();
        comboBoxItem->Tag = encodingProperties;
        PreviewSettings->Items->Append(comboBoxItem);
    }

    MatchPreviewAspectRatio();
}

/// <summary>
/// Finds all the available video resolutions that match the aspect ratio of the preview stream
/// Note: This should also be done with photos as well. This same method can be modified for photos
/// by changing the MediaStreamType from VideoPreview to Photo.
/// </summary>
void Scenario3_AspectRatio::MatchPreviewAspectRatio()
{
    // Clear out old entries
    VideoSettings->Items->Clear();

    // Query all video properties of the device
    auto allProperties = _mediaCapture->VideoDeviceController->GetAvailableMediaStreamProperties(MediaStreamType::VideoRecord);

    // Query the current preview settings
    auto previewProperties = ref new StreamResolutionHelper(_mediaCapture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview));

    // Get all formats that have the same-ish aspect ratio as the preview and create new entries in the UI
    // Allow for some tolerance in the aspect ratio comparison
    const double ASPECT_RATIO_TOLERANCE = 0.015;
    for (auto properties : allProperties)
    {
        auto streamHelper = ref new StreamResolutionHelper(properties);
        if (abs(streamHelper->AspectRatio() - previewProperties->AspectRatio()) < ASPECT_RATIO_TOLERANCE)
        {
            ComboBoxItem^ comboBoxItem = ref new ComboBoxItem();
            comboBoxItem->Content = streamHelper->GetFriendlyName();
            comboBoxItem->Tag = properties;
            VideoSettings->Items->Append(comboBoxItem);
        }
    }
}

/// <summary>
/// On some devices there may not be seperate streams for preview/photo/video. In this case, changing any of them
/// will change all of them since they are the same stream
/// </summary>
void Scenario3_AspectRatio::CheckIfStreamsAreIdentical()
{
    if (_mediaCapture->MediaCaptureSettings->VideoDeviceCharacteristic == VideoDeviceCharacteristic::AllStreamsIdentical ||
        _mediaCapture->MediaCaptureSettings->VideoDeviceCharacteristic == VideoDeviceCharacteristic::PreviewRecordStreamsIdentical)
    {
        _rootPage->Current->NotifyUser(L"Warning: Preview and record streams for this device are identical, changing one will affect the other", NotifyType::ErrorMessage);
    }
}

void Scenario3_AspectRatio::MediaCapture_Failed(MediaCapture^, MediaCaptureFailedEventArgs^ e)
{
    String^ message = e->Message;
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message]()
    {
        _rootPage->Current->NotifyUser("Preview stopped: " + message, NotifyType::ErrorMessage);
        _isPreviewing = false;
        CleanupCameraAsync();
    }));
}
