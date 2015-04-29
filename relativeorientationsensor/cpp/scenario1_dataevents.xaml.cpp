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
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1_DataEvents.xaml.h"

using namespace RelativeOrientationCPP;
using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario1::Scenario1() : rootPage(MainPage::Current), desiredReportInterval(0)
{
    InitializeComponent();

    sensor = OrientationSensor::GetDefaultForRelativeReadings();
    if (sensor != nullptr)
    {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        // This value will be used later to activate the sensor.
        uint32 minReportInterval = sensor->MinimumReportInterval;
        desiredReportInterval = minReportInterval > 16 ? minReportInterval : 16;
    }
    else
    {
        rootPage->NotifyUser("No relative orientation sensor found", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario1::OnNavigatedFrom(NavigationEventArgs^ e)
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
void Scenario1::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
{
    // The app should watch for VisibilityChanged events to disable and re-enable sensor input as appropriate
    if (ScenarioDisableButton->IsEnabled)
    {
        if (e->Visible)
        {
            // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
            readingToken = sensor->ReadingChanged::add(ref new TypedEventHandler<OrientationSensor^, OrientationSensorReadingChangedEventArgs^>(this, &Scenario1::ReadingChanged));
        }
        else
        {
            // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
            sensor->ReadingChanged::remove(readingToken);
        }
    }
}

void Scenario1::ReadingChanged(OrientationSensor^ sender, OrientationSensorReadingChangedEventArgs^ e)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, e]()
            {
                OrientationSensorReading^ reading = e->Reading;

                // Quaternion values
                SensorQuaternion^ quaternion = reading->Quaternion; // get a reference to the object to avoid re-creating it for each access
                ScenarioOutput_X->Text = quaternion->X.ToString();
                ScenarioOutput_Y->Text = quaternion->Y.ToString();
                ScenarioOutput_Z->Text = quaternion->Z.ToString();
                ScenarioOutput_W->Text = quaternion->W.ToString();

                // Rotation Matrix values
                SensorRotationMatrix^ rotationMatrix = reading->RotationMatrix;
                ScenarioOutput_M11->Text = rotationMatrix->M11.ToString();
                ScenarioOutput_M12->Text = rotationMatrix->M12.ToString();
                ScenarioOutput_M13->Text = rotationMatrix->M13.ToString();
                ScenarioOutput_M21->Text = rotationMatrix->M21.ToString();
                ScenarioOutput_M22->Text = rotationMatrix->M22.ToString();
                ScenarioOutput_M23->Text = rotationMatrix->M23.ToString();
                ScenarioOutput_M31->Text = rotationMatrix->M31.ToString();
                ScenarioOutput_M32->Text = rotationMatrix->M32.ToString();
                ScenarioOutput_M33->Text = rotationMatrix->M33.ToString();
            },
            CallbackContext::Any
            )
        );
}

void Scenario1::ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (sensor != nullptr)
    {
        // Establish the report interval
        sensor->ReportInterval = desiredReportInterval;

        visibilityToken = Window::Current->VisibilityChanged::add(ref new WindowVisibilityChangedEventHandler(this, &Scenario1::VisibilityChanged));
        readingToken = sensor->ReadingChanged::add(ref new TypedEventHandler<OrientationSensor^, OrientationSensorReadingChangedEventArgs^>(this, &Scenario1::ReadingChanged));

        ScenarioEnableButton->IsEnabled = false;
        ScenarioDisableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser("No relative orientation sensor found", NotifyType::ErrorMessage);
    }
}

void Scenario1::ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Window::Current->VisibilityChanged::remove(visibilityToken);
    sensor->ReadingChanged::remove(readingToken);

    // Restore the default report interval to release resources while the sensor is not in use
    sensor->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
