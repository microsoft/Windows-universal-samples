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
// Scenario1_DataEvents.xaml.cpp
// Implementation of the Scenario1_DataEvents class
//

#include "pch.h"
#include "Scenario1_DataEvents.xaml.h"

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

Scenario1_DataEvents::Scenario1_DataEvents() : rootPage(MainPage::Current)
{
    InitializeComponent();

    watcher = DeviceInformation::CreateWatcher(ProximitySensor::GetDeviceSelector());
    watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &Scenario1_DataEvents::OnProximitySensorAdded);
    watcher->Start();
}

/// <summary>
/// Invoked when the device watcher finds a proximity sensor
/// </summary>
/// <param name="sender">The device watcher</param>
/// <param name="device">Device information for the proximity sensor that was found</param>
void Scenario1_DataEvents::OnProximitySensorAdded(DeviceWatcher^ /*sender*/, DeviceInformation^ device)
{
    if (nullptr == sensor)
    {
        ProximitySensor^ foundSensor = ProximitySensor::FromId(device->Id);
        if (nullptr != foundSensor)
        {
            if (nullptr != foundSensor->MaxDistanceInMillimeters)
            {
                // Check if this is the sensor that matches our ranges.

                // TODO: Customize these values to your application's needs.
                // Here, we are looking for a sensor that can detect close objects
                // up to 3cm away, so we check the upper bound of the detection range.
                const unsigned int distanceInMillimetersLValue = 30; // 3 cm
                const unsigned int distanceInMillimetersRValue = 50; // 5 cm

                if (foundSensor->MaxDistanceInMillimeters->Value >= distanceInMillimetersLValue &&
                    foundSensor->MaxDistanceInMillimeters->Value <= distanceInMillimetersRValue)
                {
                    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
                    {
                        rootPage->NotifyUser("Found a proximity sensor that meets the detection range", NotifyType::StatusMessage);
                    }, CallbackContext::Any));
                }
                else
                {
                    foundSensor = nullptr;
                }
            }
            else
            {
                Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
                {
                    rootPage->NotifyUser("Proximity sensor does not report detection ranges, using it anyway", NotifyType::StatusMessage);
                }, CallbackContext::Any));
            }

            if (nullptr != foundSensor)
            {
                sensor = foundSensor;
            }
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
void Scenario1_DataEvents::OnNavigatedTo(NavigationEventArgs^ e)
{
    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario1_DataEvents::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (NavigationMode::Forward == e->NavigationMode && nullptr == e->Uri)
    {
        return;
    }

    if (ScenarioDisableButton->IsEnabled)
    {
        sensor->ReadingChanged::remove(readingToken);
    }
}

void Scenario1_DataEvents::ReadingChanged(ProximitySensor^ sender, ProximitySensorReadingChangedEventArgs^ e)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, e]()
    {
        ProximitySensorReading^ reading = e->Reading;
        if (nullptr != reading)
        {
            ScenarioOutput_DetectionState->Text = reading->IsDetected ? "Detected" : "Not detected";

            auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
            ScenarioOutput_Timestamp->Text = timestampFormatter->Format(reading->Timestamp);

            // Show the detection distance, if available
            if (nullptr != reading->DistanceInMillimeters)
            {
                ScenarioOutput_DetectionDistance->Text = reading->DistanceInMillimeters->Value.ToString();
            }
        }
    }, CallbackContext::Any));
}

/// <summary>
/// This is the click handler for the 'Enable' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_DataEvents::ScenarioEnable(Object^ sender, RoutedEventArgs^ e)
{
    if (nullptr != sensor)
    {
        // Establish the report interval
        readingToken = sensor->ReadingChanged::add(ref new TypedEventHandler<ProximitySensor^, ProximitySensorReadingChangedEventArgs^>(this, &Scenario1_DataEvents::ReadingChanged));

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
void Scenario1_DataEvents::ScenarioDisable(Object^ sender, RoutedEventArgs^ e)
{
    sensor->ReadingChanged::remove(readingToken);

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
