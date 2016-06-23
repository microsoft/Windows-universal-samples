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
#include "Scenario1_GetLamp.xaml.h"
#include <sstream>

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Capture;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes a new instance of the <see cref="Scenario1_GetLamp"/> class.
/// </summary>
Scenario1_GetLamp::Scenario1_GetLamp() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Event handler for the OnNavigatedTo event. In this scenario, we want
/// to get a lamp object for the lifetime of the page, so when we first 
/// navigate to the page, we Initialize a private lamp object
/// </summary>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario1_GetLamp::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    _rootPage = MainPage::Current;
    Windows::UI::Xaml::Controls::Page::OnNavigatedTo(e);
}

/// <summary>
/// acquires Lamp instance by getting class selection string 
/// for Lamp devices, then selecting the first lamp device on
/// the back of the device
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario1_GetLamp::InitLampBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LogStatusToOutputBoxAsync("Getting class selection string for lamp devices...");

    // Returns class selection string for Lamp devices
    String^ selectorStr = Lights::Lamp::GetDeviceSelector();
    std::wstringstream ss;
    ss << "Lamp class selection string:\n " << selectorStr->Data();
    LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

    LogStatusToOutputBoxAsync("Finding all lamp devices...");

    // Finds all devices based on lamp class selector string
    create_task(DeviceInformation::FindAllAsync(selectorStr))
        .then([this](DeviceInformationCollection^ devices)
    {
        std::wstringstream ss;
        ss << "Number of lamp devices found: " << devices->Size;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

        // Select the first lamp device found on the back of the device
        LogStatusToOutputBoxAsync("Selecting first lamp device found on back of the device");
        DeviceInformation^ deviceInfo = nullptr;
        for (auto device : devices)
        {
            if (device->EnclosureLocation != nullptr && device->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Back)
            {
                deviceInfo = device;
            }
        }
        if (deviceInfo == nullptr)
        {
            LogStatusAsync("Error: No lamp device was found", NotifyType::ErrorMessage);
            return;
        }

        ss.str(std::wstring());
        ss << "Acquiring Lamp instance from Id:\n " << deviceInfo->Id->Data();
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));
        create_task(Lights::Lamp::FromIdAsync(deviceInfo->Id))
            .then([this](Lights::Lamp^ lamp)
        {
            std::wstringstream ss;
            ss << "Lamp instance acquired, Device Id:\n " << lamp->DeviceId->Data();
            LogStatusAsync(ref new String(ss.str().c_str()), NotifyType::StatusMessage);

            // Here we must Dispose of the lamp object once we are no longer 
            // using it to release any native resources
            LogStatusToOutputBoxAsync("Disposing of lamp instance");
            lamp = nullptr;
            LogStatusToOutputBoxAsync("Disposed");
        });
    });
}

/// <summary>
/// Gets the Default Lamp instance
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario1_GetLamp::GetDefaultAsyncBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LogStatusToOutputBoxAsync("Initializing lamp");

    create_task(Lights::Lamp::GetDefaultAsync())
        .then([this](Lights::Lamp^ lamp)
    {
        if (lamp == nullptr)
        {
            LogStatusAsync("Error: No Lamp device found", NotifyType::ErrorMessage);
            return;
        }

        std::wstringstream ss;
        ss << "Default lamp instance acquired, Device Id: " << lamp->DeviceId->Data();
        LogStatusAsync(ref new String(ss.str().c_str()), NotifyType::StatusMessage);
    }); 
}

/// <summary>
/// Marshall log message to the UI thread and update outputBox, this method is for more general messages
/// Note: This method is asynchronous and is not guaranteed to display messages in the order in 
/// which they were called without task chaining
/// </summary>
/// <param name="message">Message to log to the outputBox</param>
task<void> Scenario1_GetLamp::LogStatusToOutputBoxAsync(String^ message)
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
task<void> Scenario1_GetLamp::LogStatusAsync(String^ message, NotifyType type)
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type]()
    {
        _rootPage->NotifyUser(message, type);
    }))).then([this, message]()
    {
            return LogStatusToOutputBoxAsync(message);
    });
}
