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

using namespace AccelerometerCPP;
using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario5_DataEventsBatching::Scenario5_DataEventsBatching() : rootPage(MainPage::Current), desiredReportInterval(0), desiredReportLatency(0)
{
    InitializeComponent();

    accelerometer = Accelerometer::GetDefault();
    if (accelerometer != nullptr)
    {
        // Select a report interval and report latency that is both suitable for the purposes of the app and supported by the sensor.
        // This value will be used later to activate the sensor.
        uint32 minReportInterval = accelerometer->MinimumReportInterval;
        desiredReportInterval = minReportInterval > 16 ? minReportInterval : 16;

        uint32 maxSupportedLatency = desiredReportInterval * accelerometer->MaxBatchSize;
        desiredReportLatency = maxSupportedLatency < 10000 ? maxSupportedLatency : 10000;
    }
    else
    {
        rootPage->NotifyUser("No accelerometer found", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario5_DataEventsBatching::OnNavigatedTo(NavigationEventArgs^ e)
{
    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario5_DataEventsBatching::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    if (ScenarioDisableButton->IsEnabled)
    {
        Window::Current->VisibilityChanged::remove(visibilityToken);
        accelerometer->ReadingChanged::remove(readingToken);

        // Restore the default report interval to release resources while the sensor is not in use
        accelerometer->ReportInterval = 0;
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
            readingToken = accelerometer->ReadingChanged::add(ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(this, &Scenario5_DataEventsBatching::ReadingChanged));
        }
        else
        {
            // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
            accelerometer->ReadingChanged::remove(readingToken);
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
                AccelerometerReading^ reading = e->Reading;

                ScenarioOutput_X->Text = reading->AccelerationX.ToString();
                ScenarioOutput_Y->Text = reading->AccelerationY.ToString();
                ScenarioOutput_Z->Text = reading->AccelerationZ.ToString();
            },
            CallbackContext::Any
            )
        );
}

void Scenario5_DataEventsBatching::ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (accelerometer != nullptr)
    {
        // Establish the report interval
        accelerometer->ReportInterval = desiredReportInterval;
        //Establish the report latency
        accelerometer->ReportLatency = desiredReportLatency;

        visibilityToken = Window::Current->VisibilityChanged::add(ref new WindowVisibilityChangedEventHandler(this, &Scenario5_DataEventsBatching::VisibilityChanged));
        readingToken = accelerometer->ReadingChanged::add(ref new TypedEventHandler<Accelerometer^, AccelerometerReadingChangedEventArgs^>(this, &Scenario5_DataEventsBatching::ReadingChanged));

        ScenarioEnableButton->IsEnabled = false;
        ScenarioDisableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser("No accelerometer found", NotifyType::ErrorMessage);
    }
}

void Scenario5_DataEventsBatching::ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Window::Current->VisibilityChanged::remove(visibilityToken);
    accelerometer->ReadingChanged::remove(readingToken);

    // Restore the default report interval to release resources while the sensor is not in use
    accelerometer->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
