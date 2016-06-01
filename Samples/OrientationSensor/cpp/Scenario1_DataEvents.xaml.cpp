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

using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario1_DataEvents::Scenario1_DataEvents()
{
    InitializeComponent();
}

void Scenario1_DataEvents::OnNavigatedTo(NavigationEventArgs^ e)
{
    sensor = OrientationSensor::GetDefault(rootPage->SensorReadingType, rootPage->SensorOptimizationGoal);
    if (sensor != nullptr)
    {
        rootPage->NotifyUser(rootPage->SensorDescription + " is ready", NotifyType::StatusMessage);
        ScenarioEnableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser(rootPage->SensorDescription + " not found", NotifyType::ErrorMessage);
    }
}

void Scenario1_DataEvents::OnNavigatedFrom(NavigationEventArgs^ e)
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
void Scenario1_DataEvents::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
{
    // The app should watch for VisibilityChanged events to disable and re-enable sensor input as appropriate
    if (ScenarioDisableButton->IsEnabled)
    {
        if (e->Visible)
        {
            // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
            readingToken = sensor->ReadingChanged += ref new TypedEventHandler<OrientationSensor^, OrientationSensorReadingChangedEventArgs^>(this, &Scenario1_DataEvents::ReadingChanged);
        }
        else
        {
            // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
            sensor->ReadingChanged -= readingToken;
        }
    }
}

void Scenario1_DataEvents::ReadingChanged(OrientationSensor^ sender, OrientationSensorReadingChangedEventArgs^ e)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, e]()
            {
                MainPage::SetReadingText(ScenarioOutput, e->Reading);
            },
            CallbackContext::Any
            )
        );
}

void Scenario1_DataEvents::ScenarioEnable()
{
    // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
    sensor->ReportInterval = (std::max)(sensor->MinimumReportInterval, 16U);

    visibilityToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario1_DataEvents::VisibilityChanged);
    readingToken = sensor->ReadingChanged += ref new TypedEventHandler<OrientationSensor^, OrientationSensorReadingChangedEventArgs^>(this, &Scenario1_DataEvents::ReadingChanged);

    ScenarioEnableButton->IsEnabled = false;
    ScenarioDisableButton->IsEnabled = true;
}

void Scenario1_DataEvents::ScenarioDisable()
{
    Window::Current->VisibilityChanged -= visibilityToken;
    sensor->ReadingChanged -= readingToken;

    // Restore the default report interval to release resources while the sensor is not in use
    sensor->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
