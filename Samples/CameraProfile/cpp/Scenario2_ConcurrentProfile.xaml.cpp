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
#include "Scenario2_ConcurrentProfile.xaml.h"
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
/// Initializes a new instance of the <see cref="Scenario2_ConcurrentProfile"/> class.
/// </summary>
Scenario2_ConcurrentProfile::Scenario2_ConcurrentProfile() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Override default OnNavigateTo to initialize 
/// the rootPage object for Status message updates
/// </summary>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario2_ConcurrentProfile::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    _rootPage = MainPage::Current;
}

/// <summary>
/// Finds the video profile for the front and back camera and queries if
/// both cameras have a matching video recording profile that supports concurrency.
/// If a matching concurrent profile is supported, then we configure media initialization
/// settings for both cameras to the matching profile. 
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario2_ConcurrentProfile::CheckConcurrentProfileBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Find front and back Device Id of capture device that supports Video Profile
    LogStatusToOutputBoxAsync("Looking for all video capture devices on front panel");
    GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel::Front)
        .then([this](String^ frontVideoDeviceId)
    {
        LogStatusToOutputBoxAsync("Looking for all video capture devices on back panel");
        return GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel::Back)
            .then([this, frontVideoDeviceId](String^ backVideoDeviceId)
        {
            // First check if the devices support video profiles, if not there's no reason to proceed
            if (frontVideoDeviceId == nullptr || backVideoDeviceId == nullptr)
            {
                LogStatusAsync("ERROR: A capture device doesn't support Video Profile", NotifyType::ErrorMessage);
                return;
            }

            LogStatusToOutputBoxAsync("Video Profiles are supported on both cameras");

            MediaCapture^ mediaCaptureFront = ref new MediaCapture();
            MediaCapture^ mediaCaptureBack = ref new MediaCapture();

            // Create MediaCaptureInitilization settings for each video capture device
            MediaCaptureInitializationSettings^ mediaInitSettingsFront = ref new MediaCaptureInitializationSettings();
            MediaCaptureInitializationSettings^ mediaInitSettingsBack = ref new MediaCaptureInitializationSettings();

            mediaInitSettingsFront->VideoDeviceId = frontVideoDeviceId;
            mediaInitSettingsBack->VideoDeviceId = backVideoDeviceId;
            std::wstringstream ss;
            ss << "Front Device Id located: " << frontVideoDeviceId->Data();
            LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));
            ss.str(std::wstring());
            ss << "Back Device Id located: " << backVideoDeviceId->Data();
            LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

            LogStatusToOutputBoxAsync("Querying for concurrent profile");

            // Acquire concurrent profiles available to front and back capture devices, then locate a concurrent
            // profile Id that matches for both devices
            auto allFrontProfiles = MediaCapture::FindConcurrentProfiles(frontVideoDeviceId);
            auto allBackProfiles = MediaCapture::FindConcurrentProfiles(backVideoDeviceId);
            MediaCaptureVideoProfile^ frontProfile = nullptr;
            MediaCaptureVideoProfile^ backProfile = nullptr;
            for (auto fProfile : allFrontProfiles)
            {
                for (auto bProfile : allBackProfiles)
                {
                    if (fProfile->Id == bProfile->Id)
                    {
                        frontProfile = fProfile;
                        backProfile = bProfile;
                    }
                }
            }

            // Initialize our concurrency support flag.
            bool concurrencySupported = false;
            if (frontProfile != nullptr && backProfile != nullptr)
            {
                mediaInitSettingsFront->VideoProfile = frontProfile;
                mediaInitSettingsBack->VideoProfile = backProfile;
                concurrencySupported = true;

                LogStatusAsync("Concurrent profile located, device supports concurrency", NotifyType::StatusMessage);
            }
            else
            {
                // There are no concurrent profiles available on this device, thus
                // there is not profile to select. With a lack of concurrent profile
                // each camera will have to be managed manually and cannot support
                // simultaneous streams. So we set the Video profile to null to 
                // indicate that camera must be managed individually.
                mediaInitSettingsFront->VideoProfile = nullptr;
                mediaInitSettingsBack->VideoProfile = nullptr;

                LogStatusAsync("No Concurrent profiles located, device does not support concurrency", NotifyType::ErrorMessage);
            }

            LogStatusToOutputBoxAsync("Initializing Front camera settings");

            std::vector<task<void>> initializeTasks;
            initializeTasks.push_back(create_task(mediaCaptureFront->InitializeAsync(mediaInitSettingsFront)));

            if (concurrencySupported)
            {
                LogStatusToOutputBoxAsync("Device supports concurrency, initializing Back camera settings");

                // Only initialize the back camera if concurrency is available.  Otherwise,
                // we can only use one camera at a time (which in this case will be the front
                // camera).
                initializeTasks.push_back(create_task(mediaCaptureBack->InitializeAsync(mediaInitSettingsBack)));
            }

            when_all(initializeTasks.begin(), initializeTasks.end())
                .then([this]()
            {
                LogStatusToOutputBoxAsync("Initialization complete");
            });
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
task<String^> Scenario2_ConcurrentProfile::GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel panel)
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
task<void> Scenario2_ConcurrentProfile::LogStatusToOutputBoxAsync(String^ message)
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
task<void> Scenario2_ConcurrentProfile::LogStatusAsync(String^ message, NotifyType type)
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type]()
    {
        _rootPage->NotifyUser(message, type);
    }))).then([this, message]()
    {
        return LogStatusToOutputBoxAsync(message);
    });
}
