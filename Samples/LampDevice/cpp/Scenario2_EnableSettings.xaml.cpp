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
#include "Scenario2_EnableSettings.xaml.h"
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
/// Initializes a new instance of the <see cref="Scenario2_EnableSettings"/> class.
/// </summary>
Scenario2_EnableSettings::Scenario2_EnableSettings() : _rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Event handler for the OnNavigatedTo event. In this scenario, we want
/// to get a lamp object for the lifetime of the page, so when we first 
/// navigate to the page, we Initialize a private lamp object
/// </summary>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario2_EnableSettings::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    _rootPage = MainPage::Current;
    Windows::UI::Xaml::Controls::Page::OnNavigatedTo(e);
}

/// <summary>
/// Event Handler for Brightness Button Click
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario2_EnableSettings::BrightnessBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LogStatusToOutputBoxAsync("Initializing lamp");
    _rootPage->NotifyUser("Initializing lamp", NotifyType::StatusMessage);

    create_task(Lights::Lamp::GetDefaultAsync())
        .then([this](Lights::Lamp^ lamp)
    {
        if (lamp == nullptr)
        {
            LogStatusAsync("Error: No lamp device was found", NotifyType::ErrorMessage);
            return;
        }

        std::wstringstream ss;
        ss << "Default lamp instance acquired, Device Id: " << lamp->DeviceId->Data();
        LogStatusAsync(ref new String(ss.str().c_str()), NotifyType::StatusMessage);
        LogStatusToOutputBoxAsync("Lamp Default settings:");
        ss.str(std::wstring());
        ss << "Lamp Enabled: " << lamp->IsEnabled << ", Brightness: " << lamp->BrightnessLevel;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

        // Set the Brightness Level
        LogStatusToOutputBoxAsync("Adjusting Brightness");
        lamp->BrightnessLevel = 0.5F;
        ss.str(std::wstring());
        ss << "Lamp Settings After Brightness Adjustment: Brightness: " << lamp->BrightnessLevel;
        LogStatusAsync(ref new String(ss.str().c_str()), NotifyType::StatusMessage);

        // Turn Lamp on
        LogStatusToOutputBoxAsync("Turning Lamp on");
        lamp->IsEnabled = true;
        ss.str(std::wstring());
        ss << "Lamp Enabled: " << lamp->IsEnabled;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

        // Turn Lamp off
        LogStatusToOutputBoxAsync("Turning Lamp off");
        lamp->IsEnabled = false;
        ss.str(std::wstring());
        ss << "Lamp Enabled: " << lamp->IsEnabled;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));
    });
}

/// <summary>
/// Event handler for Color Lamp Button. Queries the lamp device for color adjustment
/// support and if device supports being color settable, it sets lamp color to blue. 
/// Click Event
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario2_EnableSettings::ColorLampBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LogStatusToOutputBoxAsync("Initializing lamp");

    create_task(Lights::Lamp::GetDefaultAsync())
        .then([this](Lights::Lamp^ lamp)
    {
        if (lamp == nullptr)
        {
            LogStatusAsync("Error: No lamp device was found", NotifyType::ErrorMessage);
            return;
        }

        // With Lamp instance required, query for the default settings of the Lamp
        LogStatusAsync("Default lamp instance acquired", NotifyType::StatusMessage);
        LogStatusToOutputBoxAsync("Lamp Default settings:");
        std::wstringstream ss;
        ss << "Lamp Enabled: " << lamp->IsEnabled << ", Is Color Settable: " << lamp->IsColorSettable;;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

        // If this lamp device is not Color Settable exit color lamp adjustment
        if (lamp->IsColorSettable == false)
        {
            LogStatusAsync("Selected Lamp device doesn't support Color lamp adjustment", NotifyType::ErrorMessage);
            return;
        }

        // Change Lamp Color
        LogStatusToOutputBoxAsync("Adjusting Color");
        lamp->Color = Windows::UI::Colors::Blue;
        LogStatusAsync("Lamp color has been changed", NotifyType::StatusMessage);

        // Turn Lamp on
        LogStatusToOutputBoxAsync("Turning Lamp on");
        lamp->IsEnabled = true;
        ss.str(std::wstring());
        ss << "Lamp Enabled: " << lamp->IsEnabled;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));

        // Turn Lamp off
        LogStatusToOutputBoxAsync("Turning Lamp off");
        lamp->IsEnabled = false;
        ss.str(std::wstring());
        ss << "Lamp Enabled: " << lamp->IsEnabled;
        LogStatusToOutputBoxAsync(ref new String(ss.str().c_str()));
    });
}

/// <summary>
/// Marshall log message to the UI thread and update outputBox, this method is for more general messages
/// Note: This method is asynchronous and is not guaranteed to display messages in the order in 
/// which they were called without task chaining
/// </summary>
/// <param name="message">Message to log to the outputBox</param>
task<void> Scenario2_EnableSettings::LogStatusToOutputBoxAsync(String^ message)
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
task<void> Scenario2_EnableSettings::LogStatusAsync(String^ message, NotifyType type)
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type]()
    {
        _rootPage->NotifyUser(message, type);
    }))).then([this, message]()
    {
        return LogStatusToOutputBoxAsync(message);
    });
}
