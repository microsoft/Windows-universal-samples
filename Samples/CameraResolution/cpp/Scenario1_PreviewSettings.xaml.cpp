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
#include "Scenario1_PreviewSettings.xaml.h"
#include "StreamResolutionHelper.h"
#include <sstream>

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes a new instance of the <see cref="Scenario1_PreviewSettings"/> class.
/// </summary>
Scenario1_PreviewSettings::Scenario1_PreviewSettings() 
    : _rootPage(MainPage::Current)
    , _mediaCapture(nullptr)
    , _isPreviewing(false)
{
    InitializeComponent();
}

Scenario1_PreviewSettings::~Scenario1_PreviewSettings()
{
    CleanupCameraAsync();
}

void Scenario1_PreviewSettings::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    CleanupCameraAsync();
}

/// <summary>
/// Initializes the camera
/// </summary>
task<void> Scenario1_PreviewSettings::InitializeCameraAsync()
{
    _mediaCapture = ref new MediaCapture();
    _mediaCaptureFailedEventToken =
        _mediaCapture->Failed += ref new MediaCaptureFailedEventHandler(this, &Scenario1_PreviewSettings::MediaCapture_Failed);

    return create_task(_mediaCapture->InitializeAsync())
        .then([this]()
    {
        PreviewControl->Source = _mediaCapture.Get();
        return create_task(_mediaCapture->StartPreviewAsync());
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
task<void> Scenario1_PreviewSettings::CleanupCameraAsync()
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
void Scenario1_PreviewSettings::InitializeCameraButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
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
            PopulateSettingsComboBox();
        }
    });    
}

/// <summary>
///  Event handler for Preview settings combo box. Updates stream resolution based on the selection.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_PreviewSettings::ComboBoxSettings_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
    if (_isPreviewing)
    {
        auto selectedItem = static_cast<ComboBoxItem^>(static_cast<ComboBox^>(sender)->SelectedItem);
        auto encodingProperties = static_cast<IMediaEncodingProperties^>(selectedItem->Tag);
        create_task(_mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(MediaStreamType::VideoPreview, encodingProperties));
    }
}

/// <summary>
/// Populates the combo box with all possible combinations of settings returned by the camera driver
/// </summary>
void Scenario1_PreviewSettings::PopulateSettingsComboBox()
{
    // Query all properties of the device
    auto allProperties = _mediaCapture->VideoDeviceController->GetAvailableMediaStreamProperties(MediaStreamType::VideoPreview);
    
    // Populate the combo box with the entries
    for (auto encodingProperties : allProperties)
    {
        ComboBoxItem^ comboBoxItem = ref new ComboBoxItem();
        auto streamHelper = ref new StreamResolutionHelper(encodingProperties);
        comboBoxItem->Content = streamHelper->GetFriendlyName();
        comboBoxItem->Tag = encodingProperties;
        CameraSettings->Items->Append(comboBoxItem);
    }
}

void Scenario1_PreviewSettings::MediaCapture_Failed(MediaCapture^, MediaCaptureFailedEventArgs^ e)
{
    String^ message = e->Message;
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message]()
    {
        _rootPage->Current->NotifyUser("Preview stopped: " + message, NotifyType::ErrorMessage);
        _isPreviewing = false;
        CleanupCameraAsync();
    }));
}