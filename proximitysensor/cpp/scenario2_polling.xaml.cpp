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
// Scenario2_Polling.xaml.cpp
// Implementation of the Scenario2_Polling class
//

#include "pch.h"
#include "Scenario2_Polling.xaml.h"

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

Scenario2_Polling::Scenario2_Polling() : rootPage(MainPage::Current)
{
    InitializeComponent();

    watcher = DeviceInformation::CreateWatcher(ProximitySensor::GetDeviceSelector());
    watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &Scenario2_Polling::OnProximitySensorAdded);
    watcher->Start();
}

/// <summary>
/// Invoked when the device watcher finds a proximity sensor
/// </summary>
/// <param name="sender">The device watcher</param>
/// <param name="device">Device information for the proximity sensor that was found</param>
void Scenario2_Polling::OnProximitySensorAdded(DeviceWatcher^ /*sender*/, DeviceInformation^ device)
{
    if (nullptr == sensor)
    {
        ProximitySensor^ foundSensor = ProximitySensor::FromId(device->Id);
        if (nullptr != foundSensor)
        {
            // Optionally check the ProximitySensor.MaxDistanceInCentimeters/MinDistanceInCentimeters
            // properties here. Refer to Scenario 1 for details.
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
/// This is the click handler for the 'GetData' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_Polling::ScenarioGetData(Object^ sender, RoutedEventArgs^ e)
{
    if (nullptr != sensor)
    {
        ProximitySensorReading^ reading = sensor->GetCurrentReading();
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
    }
    else
    {
        rootPage->NotifyUser("No proximity sensor found", NotifyType::ErrorMessage);
    }
}
