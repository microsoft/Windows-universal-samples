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
#include "Scenario3_AvailabilityChanged.xaml.h"
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
/// Initializes a new instance of the <see cref="Scenario3_AvailabilityChanged("/> class.
/// </summary>
Scenario3_AvailabilityChanged::Scenario3_AvailabilityChanged() 
    : _rootPage(MainPage::Current)
    , _lamp(nullptr)
{
    InitializeComponent();
}

/// <summary>
/// Event handler for the OnNavigatedTo event. In this scenario, we want
/// to get a lamp object for the lifetime of the page, so when we first 
/// navigate to the page, we Initialize a private lamp object
/// </summary>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario3_AvailabilityChanged::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    _rootPage = MainPage::Current;
    InitializeLampAsync()
        .then([this, e]()
    {
        Windows::UI::Xaml::Controls::Page::OnNavigatedTo(e);
    });
}

/// <summary>
/// Initialize the lamp acquiring the default instance
/// </summary>
/// <returns>async Task</returns>
task<void> Scenario3_AvailabilityChanged::InitializeLampAsync()
{
    return create_task(Lights::Lamp::GetDefaultAsync())
        .then([this](Lights::Lamp^ lamp)
    {
        if (lamp == nullptr)
        {
            LogStatusAsync("Error: No lamp device was found", NotifyType::ErrorMessage);
            lampToggle->IsEnabled = false;
            return;
        }

        // Set our private member for later use
        _lamp = lamp;

        std::wstringstream ss;
        ss << "Default lamp instance acquired, Device Id: " << lamp->DeviceId->Data();
        LogStatusAsync(ref new String(ss.str().c_str()), NotifyType::StatusMessage);
        lampToggle->IsEnabled = true;
    });
}

/// <summary>
/// Event handler for the OnNavigatingFrom event. Here 
/// we want to make sure to release the lamp before exiting this scenario page.
/// </summary>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario3_AvailabilityChanged::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
    DisposeLamp();
    Windows::UI::Xaml::Controls::Page::OnNavigatingFrom(e);
}

/// <summary>
/// Method to dispose of current Lamp instance
/// and release all resources
/// </summary>
void Scenario3_AvailabilityChanged::DisposeLamp()
{
    if (_lamp == nullptr)
    {
        return;
    }

    lampToggle->IsEnabled = false;
    _lamp->AvailabilityChanged -= _lampAvailabilutyChangedEventToken;
    _lamp->IsEnabled = false;
    _lamp = nullptr;
}

/// <summary>
/// Event Handler for Lamp Availability Changed. When it fires, we want to update to
/// Lamp toggle UI to show that lamp is available or not
/// </summary>
/// <param name="sender">Contains information regarding  Lamp object that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario3_AvailabilityChanged::Lamp_AvailabilityChanged(Lights::Lamp^ sender, Lights::LampAvailabilityChangedEventArgs^ args)
{
    // Update the Lamp Toggle UI to indicate lamp has been consumed by another application
    create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, args]()
    {
        if (lampToggle->IsOn)
        {
            lampToggle->IsOn = args->IsAvailable;
        }

        lampToggle->IsEnabled = args->IsAvailable;
    }))).then([this, args]()
    {
        std::wstringstream ss;
        ss << "Lamp Availability Changed Notification has fired, IsAvailable= " << args->IsAvailable;
        LogStatusAsync(ref new String(ss.str().c_str()), NotifyType::StatusMessage);
    });
}

/// <summary>
/// Event Handler for the register button click event
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario3_AvailabilityChanged::RegisterBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_lamp == nullptr)
    {
        LogStatusAsync("Error: No lamp device was found", NotifyType::ErrorMessage);
        return;
    }

    _lampAvailabilutyChangedEventToken =
        _lamp->AvailabilityChanged += ref new TypedEventHandler<Lights::Lamp^, Lights::LampAvailabilityChangedEventArgs^>(this, &Scenario3_AvailabilityChanged::Lamp_AvailabilityChanged);
    LogStatusAsync("Registered for Lamp Availability Changed Notification", NotifyType::StatusMessage);
}

/// <summary>
/// Event handler for the Unregister Availability Event
/// </summary>
/// <param name="sender">Contains information regarding button that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario3_AvailabilityChanged::UnRegisterBtn_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (_lamp == nullptr)
    {
        LogStatusAsync("Error: No lamp device was found", NotifyType::ErrorMessage);
        return;
    }

    _lamp->AvailabilityChanged -= _lampAvailabilutyChangedEventToken;
    LogStatusAsync("Unregistered for Lamp Availability Changed Notification", NotifyType::StatusMessage);
}

/// <summary>
/// Event handler for Lamp Toggle Switch
/// Toggling the switch will turn Lamp on or off
/// </summary>
/// <param name="sender">Contains information regarding toggle switch that fired event</param>
/// <param name="e">Contains state information and event data associated with the event</param>
void Scenario3_AvailabilityChanged::lampToggle_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_lamp == nullptr)
    {
        LogStatusAsync("Error: No lamp device was found", NotifyType::ErrorMessage);
        return;
    }

    ToggleSwitch^ toggleSwitch = static_cast<ToggleSwitch^>(sender);
    if (toggleSwitch != nullptr)
    {
        if (toggleSwitch->IsOn)
        {
            _lamp->IsEnabled = true;
            LogStatusAsync("Lamp is Enabled", NotifyType::StatusMessage);
        }
        else
        {
            _lamp->IsEnabled = false;
            LogStatusAsync("Lamp is Disabled", NotifyType::StatusMessage);
        }
    }
}

/// <summary>
/// Marshall log message to the UI thread and update outputBox, this method is for more general messages
/// Note: This method is asynchronous and is not guaranteed to display messages in the order in 
/// which they were called without task chaining
/// </summary>
/// <param name="message">Message to log to the outputBox</param>
task<void> Scenario3_AvailabilityChanged::LogStatusToOutputBoxAsync(String^ message)
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
task<void> Scenario3_AvailabilityChanged::LogStatusAsync(String^ message, NotifyType type)
{
    return create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type]()
    {
        _rootPage->NotifyUser(message, type);
    }))).then([this, message]()
    {
        return LogStatusToOutputBoxAsync(message);
    });
}
