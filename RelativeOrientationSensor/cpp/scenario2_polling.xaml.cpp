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
// Implementation of the Scenario2 class
//

#include "pch.h"
#include "Scenario2_Polling.xaml.h"

using namespace RelativeOrientationCPP;
using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario2::Scenario2() : rootPage(MainPage::Current)
{
    InitializeComponent();

    sensor = OrientationSensor::GetDefaultForRelativeReadings();
    if (sensor == nullptr)
    {
        rootPage->NotifyUser("No relative orientation sensor found", NotifyType::ErrorMessage);
        GetDataButton->IsEnabled = false;
    }
    else
    {
        GetDataButton->IsEnabled = true;
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario2::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }
}

/// <summary>
/// Invoked when a user clicks on the GetDataButton
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2::GetData(Object^ sender, Object^ e)
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
    }
}
