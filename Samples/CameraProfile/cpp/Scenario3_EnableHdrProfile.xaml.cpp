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
#include "Scenario3_EnableHdrProfile.xaml.h"
#include <sstream>

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Capture;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes a new instance of the <see cref="Scenario3_EnableHdrProfile"/> class.
/// </summary>
Scenario3_EnableHdrProfile::Scenario3_EnableHdrProfile()
{
    InitializeComponent();
}

/// <summary>
/// Override default OnNavigateTo to initialize 
/// the rootPage object for Status message updates
/// </summary>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario3_EnableHdrProfile::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    _rootPage = MainPage::Current;
}

/// <summary>
/// Finds the video profiles for the back camera. Then queries for a profile that supports
/// HDR video recording. If a HDR supported video profile is located, then we configure media settings to the
/// matching profile. Finally we initialize media capture HDR recording mode to auto.
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario3_EnableHdrProfile::CheckHdrSupportBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Select the first video capture device found on the back of the device
    LogStatusToOutputBoxAsync("Querying for video capture device on back of the device that supports Video Profile");
    GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel::Back)
        .then([this](String^ videoDeviceId)
    {
        if (videoDeviceId == nullptr)
        {
            LogStatusAsync("ERROR: No Video Device Id found, verify your device supports profiles", NotifyType::ErrorMessage);
            return;
        }

        MediaCapture^ mediaCapture = ref new MediaCapture();
        MediaCaptureInitializationSettings^ mediaCaptureInitSetttings = ref new MediaCaptureInitializationSettings();

        std::wstringstream ss;
        ss << "Found device that supports Video Profile, Device Id:\r\n " << videoDeviceId->Data();
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));
        auto profiles = MediaCapture::FindKnownVideoProfiles(videoDeviceId, KnownVideoProfile::VideoRecording);

        // Walk through available profiles, look for first profile with HDR supported Video Profile
        bool HdrVideoSupported = false;
        for (auto profile : profiles)
        {
            auto recordMediaDescription = profile->SupportedRecordMediaDescription;
            for (auto videoProfileMediaDescription : recordMediaDescription)
            {
                if (videoProfileMediaDescription->IsHdrVideoSupported)
                {
                    // We've located the profile and description for HDR Video, set profile and flag
                    mediaCaptureInitSetttings->VideoProfile = profile;
                    mediaCaptureInitSetttings->RecordMediaDescription = videoProfileMediaDescription;
                    HdrVideoSupported = true;
                    LogStatusAsync("HDR supported video profile found, set video profile to current HDR supported profile", NotifyType::StatusMessage);
                    break;
                }
            }

            if (HdrVideoSupported)
            {
                break;
            }
        }

        LogStatusToOutputBoxAsync("Initializing Media settings to HDR Supported video profile");
        create_task(mediaCapture->InitializeAsync(mediaCaptureInitSetttings))
            .then([this, mediaCapture, HdrVideoSupported]()
        {
            if (HdrVideoSupported)
            {
                LogStatusToOutputBoxAsync("Initializing HDR Video Mode to Auto");
                mediaCapture->VideoDeviceController->HdrVideoControl->Mode = Windows::Media::Devices::HdrVideoMode::Auto;
            }
        });
    });
}
/// <summary>
/// Helper method to obtain a device Id that supports a Video Profile.
/// If no Video Profile is found we return empty string indicating that there isn't a device on 
/// the desired panel with a supported Video Profile.
/// </summary>
/// <param name="panel">Contains Device Enumeration information regarding camera panel we are looking for</param>
/// <returns>Device Information Id string</returns>
task<String^> Scenario3_EnableHdrProfile::GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel panel)
{
    // Find all video capture devices
    LogStatusToOutputBoxAsync("Looking for all video capture devices");

    return create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
        .then([this, panel](DeviceInformationCollection^ devices)
    {
        String^ deviceId = nullptr;
        std::wstringstream ss;
        ss << "Number of video capture devices found: " << devices->Size;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

        // Loop through devices looking for device that supports Video Profile
        for (auto cameraDeviceInfo : devices)
        {
            if (MediaCapture::IsVideoProfileSupported(cameraDeviceInfo->Id) && cameraDeviceInfo->EnclosureLocation->Panel == panel)
            {
                deviceId = cameraDeviceInfo->Id;
                break;
            }
        }

        return deviceId;
    });
}

/// <summary>
/// Marshall log message to the UI thread and update outputBox, this method is for more general messages
/// Note: This method is asynchronous and is not guaranteed to display messages in the order in 
/// which they were called without task chaining
/// </summary>
/// <param name="message">Message to log to the outputBox</param>
/// <returns></returns>
task<void> Scenario3_EnableHdrProfile::LogStatusToOutputBoxAsync(String^ message)
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message]()
    {
        outputBox->Text += message + "\r\n";
        outputScrollViewer->ChangeView(0.0, outputBox->ActualHeight, 1.0f);
    })));
}

/// <summary>
/// Method to log Status to Main Status block, this method is for important status messages to be 
/// displayed at bottom of scenario page
/// Note: This method is asynchronous and is not guaranteed to display messages in the order in 
/// which they were called without task chaining
/// </summary>
/// <param name="message">Message to log to the output box</param>
/// <param name="type">Notification Type for status message</param>
/// <returns></returns>
task<void> Scenario3_EnableHdrProfile::LogStatusAsync(String^ message, NotifyType type)
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type]()
    {
        _rootPage->NotifyUser(message, type);
    }))).then([this, message]()
    {
        return LogStatusToOutputBoxAsync(message);
    });
}
