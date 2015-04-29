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

using namespace OrientationCPP;
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

    sensor = OrientationSensor::GetDefault();
    if (sensor != nullptr)
    {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        // This value will be used later to activate the sensor.
        uint32 minReportInterval = sensor->MinimumReportInterval;
        desiredReportInterval = minReportInterval > 16 ? minReportInterval : 16;

        // Set up a DispatchTimer
        TimeSpan span;
        span.Duration = static_cast<int32>(desiredReportInterval) * 10000;   // convert to 100ns ticks
        dispatcherTimer = ref new DispatcherTimer();
        dispatcherTimer->Interval = span;
        dispatcherTimer->Tick += ref new Windows::Foundation::EventHandler<Object^>(this, &Scenario2_Polling::DisplayCurrentReading);
    }
    else
    {
        rootPage->NotifyUser("No orientation sensor found", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_Polling::OnNavigatedTo(NavigationEventArgs^ e)
{
    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario2_Polling::OnNavigatedFrom(NavigationEventArgs^ e)
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
        dispatcherTimer->Stop();

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
        
        // Yaw accuracy
        switch (reading->YawAccuracy)
        {
            case MagnetometerAccuracy::Unknown:
                ScenarioOutput_YawAccuracy->Text = "Unknown";
                break;
            case MagnetometerAccuracy::Unreliable:
                ScenarioOutput_YawAccuracy->Text = "Unreliable";
                break;
            case MagnetometerAccuracy::Approximate:
                ScenarioOutput_YawAccuracy->Text = "Approximate";
                break;
            case MagnetometerAccuracy::High:
                ScenarioOutput_YawAccuracy->Text = "High";
                break;
            default:
                ScenarioOutput_YawAccuracy->Text = "No data";
                break;
        }
    }
}

void Scenario2_Polling::ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (sensor != nullptr)
    {
        visibilityToken = Window::Current->VisibilityChanged::add(ref new WindowVisibilityChangedEventHandler(this, &Scenario2_Polling::VisibilityChanged));

        // Set the report interval to enable the sensor for polling
        sensor->ReportInterval = desiredReportInterval;

        dispatcherTimer->Start();

        ScenarioEnableButton->IsEnabled = false;
        ScenarioDisableButton->IsEnabled = true;
    }
    else
    {
        rootPage->NotifyUser("No orientation sensor found", NotifyType::ErrorMessage);
    }
}

void Scenario2_Polling::ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Window::Current->VisibilityChanged::remove(visibilityToken);

    dispatcherTimer->Stop();

    // Restore the default report interval to release resources while the sensor is not in use
    sensor->ReportInterval = 0;

    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;
}
