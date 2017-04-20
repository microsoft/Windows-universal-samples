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
// Scenario5_DataEventsBatching.xaml.cpp
// Implementation of the Scenario5_DataEventsBatching class
//

#include "pch.h"
#include "Scenario5_DataEventsBatching.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario5_DataEventsBatching::Scenario5_DataEventsBatching()
{
    InitializeComponent();
}

void Scenario5_DataEventsBatching::OnNavigatedTo(NavigationEventArgs^ e)
{
    accelerometer = Accelerometer::GetDefault(rootPage->AccelerometerReadingType);
    if (accelerometer != nullptr)
    {
        // Select a report interval and report latency that is both suitable for the purposes of the app and supported by the sensor.
        // This value will be used later to activate the sensor.
        desiredReportInterval = (std::max)(accelerometer->MinimumReportInterval, 16U);

        // MaxBatchSize will be 0 if the accelerometer does not support batching.
        desiredReportLatency = (std::min)(desiredReportInterval * accelerometer->MaxBatchSize, 10000U);

        rootPage->NotifyUser(rootPage->AccelerometerReadingType.ToString() + " accelerometer ready", NotifyType::StatusMessage);
        ScenarioEnableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser(rootPage->AccelerometerReadingType.ToString() + " accelerometer not found", NotifyType::ErrorMessage);
    }
}

void Scenario5_DataEventsBatching::OnNavigatedFrom(NavigationEventArgs^ e)
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
void Scenario5_DataEventsBatching::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
{
    // The app should watch for VisibilityChanged events to disable and re-enable sensor input as appropriate
    if (ScenarioDisableButton->IsEnabled)
    {
        if (e->Visible)
        {
            // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
            readingToken = accelerometer->ReadingChanged += ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(this, &Scenario5_DataEventsBatching::ReadingChanged);
        }
        else
        {
            // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
            accelerometer->ReadingChanged -= readingToken;
        }
    }
}

void Scenario5_DataEventsBatching::ReadingChanged(Accelerometer^ sender, AccelerometerReadingChangedEventArgs^ e)
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

void Scenario5_DataEventsBatching::ScenarioEnable()
{
    // Establish the report interval
    accelerometer->ReportInterval = desiredReportInterval;

    // Establish the report latency. This is a no-op if the accelerometer does not support batching
    accelerometer->ReportLatency = desiredReportLatency;

    visibilityToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario5_DataEventsBatching::VisibilityChanged);
    readingToken = accelerometer->ReadingChanged += ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(this, &Scenario5_DataEventsBatching::ReadingChanged);

    ScenarioEnableButton->IsEnabled = false;
    ScenarioDisableButton->IsEnabled = true;
}

void Scenario5_DataEventsBatching::ScenarioDisable()
{
    Window::Current->VisibilityChanged -= visibilityToken;
    accelerometer->ReadingChanged -= readingToken;

    // Restore the default report interval to release resources while the sensor is not in use
    accelerometer->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
