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
#include "Scenario2_PhotoSettings.xaml.h"
#include "StreamResolutionHelper.h"
#include <sstream>

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
/// Initializes a new instance of the <see cref="Scenario2_EnableSettings"/> class.
/// </summary>
Scenario2_PhotoSettings::Scenario2_PhotoSettings()
    : _rootPage(MainPage::Current)
    , _mediaCapture(nullptr)
    , _isPreviewing(false)
    , _captureFolder(nullptr)
{
    InitializeComponent();
}

Scenario2_PhotoSettings::~Scenario2_PhotoSettings()
{
    CleanupCameraAsync();
}

void Scenario2_PhotoSettings::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    CleanupCameraAsync();
}

/// <summary>
/// Initializes the camera
/// </summary>
task<void> Scenario2_PhotoSettings::InitializeCameraAsync()
{
    _mediaCapture = ref new MediaCapture();
    _mediaCaptureFailedEventToken =
        _mediaCapture->Failed += ref new MediaCaptureFailedEventHandler(this, &Scenario2_PhotoSettings::MediaCapture_Failed);

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
task<void> Scenario2_PhotoSettings::CleanupCameraAsync()
{
    task<void> cleanupTask;
    if (_isPreviewing)
    {
        cleanupTask = create_task(_mediaCapture->StopPreviewAsync());
        _isPreviewing = false;
    }
    else
    {
        cleanupTask = create_task([]() {});
    }

    return cleanupTask.then([this]()
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
void Scenario2_PhotoSettings::InitializeCameraButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
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
            PopulateComboBox(MediaStreamType::VideoPreview, PreviewSettings);
            PopulateComboBox(MediaStreamType::Photo, PhotoSettings, false);
            PhotoButton->IsEnabled = true;
        }
    });
}

/// <summary>
/// Takes a photo to and saves to a StorageFile
/// </summary>
void Scenario2_PhotoSettings::PhotoButton_Click(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^)
{
    if (_isPreviewing)
    {
        // Disable the photo button while taking a photo
        PhotoButton->IsEnabled = false;

        create_task(_captureFolder->CreateFileAsync("SimplePhoto.jpg", CreationCollisionOption::GenerateUniqueName))
            .then([this](StorageFile^ file)
        {
            return create_task(_mediaCapture->CapturePhotoToStorageFileAsync(ImageEncodingProperties::CreateJpeg(), file))
                .then([this, file]()
            {
                // Done taking the photo, so re-enable the button
                PhotoButton->IsEnabled = true;

                _rootPage->Current->NotifyUser(L"Photo taken, saved to: " + file->Path, NotifyType::StatusMessage);
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
                _rootPage->Current->NotifyUser(L"Exception when taking a photo: " + ex->Message, NotifyType::ErrorMessage);
            }
        });
    }
}

/// <summary>
///  Event handler for Preview settings combo box. Updates stream resolution based on the selection.
/// </summary>
/// <param name="sender"></param>
void Scenario2_PhotoSettings::PreviewSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
    if (_isPreviewing)
    {
        auto selectedItem = static_cast<ComboBoxItem^>(static_cast<ComboBox^>(sender)->SelectedItem);
        auto encodingProperties = static_cast<IMediaEncodingProperties^>(selectedItem->Tag);
        create_task(_mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(MediaStreamType::VideoPreview, encodingProperties));
    }
}

/// <summary>
/// Event handler for Photo settings combo box. Updates stream resolution based on the selection.
/// </summary>
/// <param name="sender"></param>
void Scenario2_PhotoSettings::PhotoSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
    if (_isPreviewing)
    {
        auto selectedItem = static_cast<ComboBoxItem^>(static_cast<ComboBox^>(sender)->SelectedItem);
        auto encodingProperties = static_cast<IMediaEncodingProperties^>(selectedItem->Tag);
        create_task(_mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(MediaStreamType::Photo, encodingProperties));
    }
}

/// <summary>
/// On some devices there may not be seperate streams for preview/photo/video. In this case, changing any of them
/// will change all of them since they are the same stream
/// </summary>
void Scenario2_PhotoSettings::CheckIfStreamsAreIdentical()
{
    if (_mediaCapture->MediaCaptureSettings->VideoDeviceCharacteristic == VideoDeviceCharacteristic::AllStreamsIdentical ||
        _mediaCapture->MediaCaptureSettings->VideoDeviceCharacteristic == VideoDeviceCharacteristic::PreviewPhotoStreamsIdentical)
    {
        _rootPage->Current->NotifyUser(L"Warning: Preview and photo streams for this device are identical, changing one will affect the other", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Populates the given combo box with all possible combinations of the given stream type settings returned by the camera driver
/// </summary>
/// <param name="streamType"></param>
/// <param name="comboBox"></param>
void Scenario2_PhotoSettings::PopulateComboBox(MediaStreamType streamType, ComboBox^ comboBox, bool showFrameRate)
{
    // Query all properties of the device
    auto allProperties = _mediaCapture->VideoDeviceController->GetAvailableMediaStreamProperties(streamType);

    // Populate the combo box with the entries
    for (auto encodingProperties : allProperties)
    {
        ComboBoxItem^ comboBoxItem = ref new ComboBoxItem();
        auto streamHelper = ref new StreamResolutionHelper(encodingProperties);
        comboBoxItem->Content = streamHelper->GetFriendlyName(showFrameRate);
        comboBoxItem->Tag = encodingProperties;
        comboBox->Items->Append(comboBoxItem);
    }
}

void Scenario2_PhotoSettings::MediaCapture_Failed(MediaCapture^, MediaCaptureFailedEventArgs^ e)
{
    String^ message = e->Message;
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message]()
    {
        _rootPage->Current->NotifyUser("Preview stopped: " + message, NotifyType::ErrorMessage);
        _isPreviewing = false;
        CleanupCameraAsync();
    }));
}
