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

//
// Scenario3_DisplayOnOff.xaml.cpp
// Implementation of the Scenario3_DisplayOnOff class
//

#include "pch.h"
#include "Scenario3_DisplayOnOff.xaml.h"

using namespace ProximityCPP;
using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_DisplayOnOff::Scenario3_DisplayOnOff() : rootPage(MainPage::Current), sensor(nullptr), displayController(nullptr)
{
    InitializeComponent();

    watcher = DeviceInformation::CreateWatcher(ProximitySensor::GetDeviceSelector());
    watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &Scenario3_DisplayOnOff::OnProximitySensorAdded);
    watcher->Start();
}

/// <summary>
/// Invoked when the device watcher finds a proximity sensor
/// </summary>
/// <param name="sender">The device watcher</param>
/// <param name="device">Device information for the proximity sensor that was found</param>
void Scenario3_DisplayOnOff::OnProximitySensorAdded(DeviceWatcher^ /*sender*/, DeviceInformation^ device)
{
    if (nullptr == sensor)
    {
        ProximitySensor^ foundSensor = ProximitySensor::FromId(device->Id);
        if (nullptr != foundSensor)
        {
            sensor = foundSensor;
        }
        else
        {
            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
            {
                rootPage->NotifyUser("Could not get a proximity sensor from the device id", NotifyType::ErrorMessage);
            }, CallbackContext::Any));
        }
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3_DisplayOnOff::OnNavigatedTo(NavigationEventArgs^ e)
{
    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario3_DisplayOnOff::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (NavigationMode::Forward == e->NavigationMode && nullptr == e->Uri)
    {
        return;
    }

    if (ScenarioDisableButton->IsEnabled)
    {
        if (nullptr != displayController)
        {
            delete displayController; // closes the controller
            displayController = nullptr;
        }
    }
}

/// <summary>
/// This is the click handler for the 'Enable' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_DisplayOnOff::ScenarioEnable(Object^ sender, RoutedEventArgs^ e)
{
    if (nullptr != sensor)
    {
        // Acquires the display on/off controller for this proximity sensor.
        // This tells the system to use the sensor's IsDetected state to
        // turn the screen on or off.  If the display does not support this
        // feature, this code will do nothing.
        displayController = sensor->CreateDisplayOnOffController();

        ScenarioEnableButton->IsEnabled = false;
        ScenarioDisableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser("No proximity sensor found", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// This is the click handler for the 'Disable' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_DisplayOnOff::ScenarioDisable(Object^ sender, RoutedEventArgs^ e)
{
    // Releases the display on/off controller that is previously acquired.
    // This tells the system to stop using the sensor's IsDetected state to
    // turn the screen on or off.  If the display does not support this
    // feature, this code will do nothing.
    if (nullptr != displayController)
    {
        delete displayController; // closes the controller
        displayController = nullptr;
    }

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
