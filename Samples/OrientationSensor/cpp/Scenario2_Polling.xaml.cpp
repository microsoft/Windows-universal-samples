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

using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario2_Polling::Scenario2_Polling() : rootPage(MainPage::Current), desiredReportInterval(0)
{
    InitializeComponent();
}

void Scenario2_Polling::OnNavigatedTo(NavigationEventArgs^ e)
{
    sensor = OrientationSensor::GetDefault();
    if (sensor != nullptr)
    {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        // This value will be used later to activate the sensor.
        desiredReportInterval = (std::max)(sensor->MinimumReportInterval, 16U);

        // Set up a DispatchTimer
        TimeSpan span;
        span.Duration = static_cast<int32>(desiredReportInterval) * 10000;   // convert to 100ns ticks
        dispatcherTimer = ref new DispatcherTimer();
        dispatcherTimer->Interval = span;
        dispatcherTimer->Tick += ref new Windows::Foundation::EventHandler<Object^>(this, &Scenario2_Polling::DisplayCurrentReading);

        rootPage->NotifyUser(rootPage->SensorDescription + " is ready", NotifyType::StatusMessage);
        ScenarioEnableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser(rootPage->SensorDescription + " not found", NotifyType::ErrorMessage);
    }
}

void Scenario2_Polling::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (ScenarioDisableButton->IsEnabled)
    {
        ScenarioDisable();
    }
}

/// <summary>
/// This is the event handler for VisibilityChanged events. You would register for these notifications
/// if handling sensor data when the app is not visible could cause unintended actions in the app.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">
/// Event data that can be examined for the current visibility state.
/// </param>
void Scenario2_Polling::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
{
    // The app should watch for VisibilityChanged events to disable and re-enable sensor input as appropriate
    if (ScenarioDisableButton->IsEnabled)
    {
        if (e->Visible)
        {
            // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
            dispatcherTimer->Start();
        }
        else
        {
            // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
            dispatcherTimer->Stop();
        }
    }
}

void Scenario2_Polling::DisplayCurrentReading(Object^ sender, Object^ e)
{
    OrientationSensorReading^ reading = sensor->GetCurrentReading();
    if (reading != nullptr)
    {
        MainPage::SetReadingText(ScenarioOutput, reading);
    }
}

void Scenario2_Polling::ScenarioEnable()
{
    visibilityToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario2_Polling::VisibilityChanged);

    // Set the report interval to enable the sensor for polling
    sensor->ReportInterval = desiredReportInterval;

    dispatcherTimer->Start();

    ScenarioEnableButton->IsEnabled = false;
    ScenarioDisableButton->IsEnabled = true;
}

void Scenario2_Polling::ScenarioDisable()
{
    Window::Current->VisibilityChanged -= visibilityToken;

    dispatcherTimer->Stop();

    // Restore the default report interval to release resources while the sensor is not in use
    sensor->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
