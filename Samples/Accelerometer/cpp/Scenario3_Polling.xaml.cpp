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
// Scenario3_Polling.xaml.cpp
// Implementation of the Scenario3_Polling class
//

#include "pch.h"
#include "Scenario3_Polling.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_Polling::Scenario3_Polling()
{
    InitializeComponent();
}
void Scenario3_Polling::OnNavigatedTo(NavigationEventArgs^ e)
{
    accelerometer = Accelerometer::GetDefault(rootPage->AccelerometerReadingType);
    if (accelerometer != nullptr)
    {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        // This value will be used later to activate the sensor.
        desiredReportInterval = (std::max)(accelerometer->MinimumReportInterval, 16U);

        // Set up a DispatchTimer
        TimeSpan span;
        span.Duration = static_cast<int32>(desiredReportInterval) * 10000;   // convert to 100ns ticks
        dispatcherTimer = ref new DispatcherTimer();
        dispatcherTimer->Interval = span;
        dispatcherTimer->Tick += ref new Windows::Foundation::EventHandler<Object^>(this, &Scenario3_Polling::DisplayCurrentReading);

        rootPage->NotifyUser(rootPage->AccelerometerReadingType.ToString() + " accelerometer ready", NotifyType::StatusMessage);
        ScenarioEnableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser(rootPage->AccelerometerReadingType.ToString() + " accelerometer not found", NotifyType::ErrorMessage);
    }
}

void Scenario3_Polling::OnNavigatedFrom(NavigationEventArgs^ e)
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
void Scenario3_Polling::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
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

void Scenario3_Polling::DisplayCurrentReading(Object^ sender, Object^ e)
{
    AccelerometerReading^ reading = accelerometer->GetCurrentReading();
    if (reading != nullptr)
    {
        MainPage::SetReadingText(ScenarioOutput, reading);
    }
}

void Scenario3_Polling::ScenarioEnable()
{
    visibilityToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario3_Polling::VisibilityChanged);

    // Set the report interval to enable the sensor for polling
    accelerometer->ReportInterval = desiredReportInterval;

    dispatcherTimer->Start();

    ScenarioEnableButton->IsEnabled = false;
    ScenarioDisableButton->IsEnabled = true;
}

void Scenario3_Polling::ScenarioDisable()
{
    Window::Current->VisibilityChanged -= visibilityToken;

    dispatcherTimer->Stop();

    // Restore the default report interval to release resources while the sensor is not in use
    accelerometer->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
