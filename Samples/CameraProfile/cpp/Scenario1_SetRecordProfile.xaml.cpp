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
#include "Scenario1_SetRecordProfile.xaml.h"
#include <math.h>
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
/// Initializes a new instance of the <see cref="Scenario1_SetRecordProfile"/> class.
/// </summary>
Scenario1_SetRecordProfile::Scenario1_SetRecordProfile() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Override default OnNavigateTo to initialize 
/// the rootPage object for Status message updates
/// </summary>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario1_SetRecordProfile::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    _rootPage = MainPage::Current;
}

/// <summary>
/// Locates a video profile for the back camera and then queries if
/// the discovered profile supports 640x480 30 FPS recording.
/// If a supported profile is located, then we configure media
/// settings to the matching profile. Else we use default profile.
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario1_SetRecordProfile::InitRecordProfileBtn_Click(Object^, RoutedEventArgs^)
{
    // Look for a video capture device Id with a video profile matching panel location
    GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel::Back)
        .then([this](String^ videoDeviceId)
    {
        if (videoDeviceId == nullptr)
        {
            LogStatusAsync("ERROR: No Video Device Id found, verify your device supports profiles", NotifyType::ErrorMessage);
            return;
        }

        std::wstringstream ss;
        ss << "Found video capture device that supports Video Profile, Device ID: \r\n" << videoDeviceId->Data();
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));
        LogStatusToOutputBoxAsync("Retrieving all Video Profiles");

        MediaCapture^ mediaCapture = ref new MediaCapture();
        auto profiles = mediaCapture->FindAllVideoProfiles(videoDeviceId);
        
        ss.str(std::wstring());
        ss << "Number of Video Profiles found: " << profiles->Size;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));
        LogStatusToOutputBoxAsync("Querying for matching WVGA 30FPS Profile");

        // Output all Video Profiles found, frame rate is rounded up for display purposes
        // Also looking for WVGA 30FPS profile
        MediaCaptureInitializationSettings^ mediaInitSettings = ref new MediaCaptureInitializationSettings();
        bool profileFound = false;
        for (auto profile : profiles)
        {
            auto description = profile->SupportedRecordMediaDescription;
            for (auto desc : description)
            {
                PrintProfileInformation(profile, desc);

                if (!profileFound)
                {
                    if (desc->Width == 640 &&
                        desc->Height == 480 &&
                        round(desc->FrameRate) == 30)
                    {
                        mediaInitSettings->VideoProfile = profile;
                        mediaInitSettings->RecordMediaDescription = desc;
                        profileFound = true;;
                    }
                }
            }
        }

        if (profileFound)
        {
            LogStatusToOutputBoxAsync("WVGA 30FPS profile found");
            PrintProfileInformation(mediaInitSettings->VideoProfile, mediaInitSettings->RecordMediaDescription);
        }
        else
        {
            // Could not locate a WVGA 30FPS profile, use default video recording profile
            mediaInitSettings->VideoProfile = profiles->GetAt(0);
            LogStatusAsync("Could not locate a matching profile, setting to default recording profile", NotifyType::ErrorMessage);
        }
        
        create_task(mediaCapture->InitializeAsync(mediaInitSettings))
            .then([this]()
        {
            LogStatusToOutputBoxAsync("Media Capture settings initialized to selected profile");
        });
    });
}

/// <summary>
/// Locates a video profile for the back camera and then queries if
/// the discovered profile is a matching custom profile.
/// If a custom profile is located, we configure media
/// settings to the custom profile. Else we use default profile.
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario1_SetRecordProfile::InitCustomProfileBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel::Back)
        .then([this](String^ videoDeviceId)
    {
        if (videoDeviceId == nullptr)
        {
            _rootPage->NotifyUser("ERROR: No Video Device Id found, verify your device supports profiles", NotifyType::ErrorMessage);
            return;
        }

        MediaCapture^ mediaCapture = ref new MediaCapture();
        MediaCaptureInitializationSettings^ mediaInitSettings = ref new MediaCaptureInitializationSettings();

        // For demonstration purposes, we use the Profile Id from WVGA 640x480 30 FPS profile available
        // on desktop simulator and phone emulators 
        String^ customProfileId = "{A0E517E8-8F8C-4F6F-9A57-46FC2F647EC0},0";

        std::wstringstream ss;
        ss << "Found video capture device that supports Video Profile, Device ID: \r\n" << videoDeviceId->Data();
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

        LogStatusToOutputBoxAsync("Querying device for custom profile support");
        mediaInitSettings->VideoProfile = nullptr;
        auto allVideoProfiles = mediaCapture->FindAllVideoProfiles(videoDeviceId);
        for (auto profile : allVideoProfiles)
        {
            if (profile->Id == customProfileId)
            {
                mediaInitSettings->VideoProfile = profile;
                break;
            }
        }

        // In the event the profile isn't located, we set Video Profile to the default
        if (mediaInitSettings->VideoProfile == nullptr)
        {
            LogStatusAsync("Could not locate a matching profile, setting to default recording profile", NotifyType::ErrorMessage);
            mediaInitSettings->VideoProfile = allVideoProfiles->GetAt(0);
        }

        ss.str(std::wstring());
        ss << "Custom recording profile located: " << customProfileId->Data();
        LogStatusAsync(ref new String(ss.str().c_str()), NotifyType::StatusMessage);
        create_task(mediaCapture->InitializeAsync(mediaInitSettings));
    });
}

/// <summary>
/// Helper method to obtain a device Id that supports a Video Profile.
/// If no Video Profile is found we return empty string indicating that there isn't a device on 
/// the desired panel with a supported Video Profile.
/// </summary>
/// <param name="panel">Contains Device Enumeration information regarding camera panel we are looking for</param>
/// <returns>Device Information Id string</returns>
task<String^> Scenario1_SetRecordProfile::GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel panel)
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
task<void> Scenario1_SetRecordProfile::LogStatusToOutputBoxAsync(String^ message)
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
task<void> Scenario1_SetRecordProfile::LogStatusAsync(String^ message, NotifyType type)
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type]()
    {
        _rootPage->NotifyUser(message, type);
    }))).then([this, message]()
    {
        return LogStatusToOutputBoxAsync(message);
    });
}

/// <summary>
/// Helper to print MediaCapture profile information
/// Note: This method is asynchronous and is not guaranteed to display messages in the order in 
/// which they were called without task chaining
/// </summary>
/// <param name="message">MediaCapture video profile</param>
/// <param name="type">MediaCapture video profile description</param>
/// <returns></returns>
task<void> Scenario1_SetRecordProfile::PrintProfileInformation(MediaCaptureVideoProfile^ profile, MediaCaptureVideoProfileMediaDescription^ description)
{
    std::wstringstream ss;
    ss << "VideoProfile: Profile Id:" << profile->Id->Data() << "\r\n" << \
        "Width: " << description->Width << \
        "Height:" << description->Height << \
        "FrameRate" << round(description->FrameRate);
    return LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));
}
