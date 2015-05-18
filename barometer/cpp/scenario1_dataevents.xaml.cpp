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

using namespace BarometerCPP;
using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_DataEvents::Scenario1_DataEvents() : rootPage(MainPage::Current), desiredReportIntervalMs(0)
{
    InitializeComponent();

    sensor = Barometer::GetDefault();
    if (nullptr != sensor)
    {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        // This value will be used later to activate the sensor.
        uint32 minReportIntervalMs = sensor->MinimumReportInterval;
        desiredReportIntervalMs = minReportIntervalMs > 1000 ? minReportIntervalMs : 1000;
    }
    else
    {
        rootPage->NotifyUser("No barometer found", NotifyType::ErrorMessage);
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
        Window::Current->VisibilityChanged::remove(visibilityToken);
        sensor->ReadingChanged::remove(readingToken);

        // Restore the default report interval to release resources while the sensor is not in use
        sensor->ReportInterval = 0;
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
            readingToken = sensor->ReadingChanged::add(ref new TypedEventHandler<Barometer^, BarometerReadingChangedEventArgs^>(this, &Scenario1_DataEvents::ReadingChanged));
        }
        else
        {
            // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
            sensor->ReadingChanged::remove(readingToken);
        }
    }
}

void Scenario1_DataEvents::ReadingChanged(Barometer^ sender, BarometerReadingChangedEventArgs^ e)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, e]()
            {
                BarometerReading^ reading = e->Reading;

                ScenarioOutput_hPa->Text = reading->StationPressureInHectopascals.ToString();
            },
            CallbackContext::Any
            )
        );
}

void Scenario1_DataEvents::ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (nullptr != sensor)
    {
        // Establish the report interval
        sensor->ReportInterval = desiredReportIntervalMs;

        visibilityToken = Window::Current->VisibilityChanged::add(ref new WindowVisibilityChangedEventHandler(this, &Scenario1_DataEvents::VisibilityChanged));
        readingToken = sensor->ReadingChanged::add(ref new TypedEventHandler<Barometer^, BarometerReadingChangedEventArgs^>(this, &Scenario1_DataEvents::ReadingChanged));

        ScenarioEnableButton->IsEnabled = false;
        ScenarioDisableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser("No barometer found", NotifyType::ErrorMessage);
    }
}

void Scenario1_DataEvents::ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Window::Current->VisibilityChanged::remove(visibilityToken);
    sensor->ReadingChanged::remove(readingToken);

    // Restore the default report interval to release resources while the sensor is not in use
    sensor->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
