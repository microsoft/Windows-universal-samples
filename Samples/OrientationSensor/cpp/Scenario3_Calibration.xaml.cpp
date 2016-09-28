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
// Scenario3_Calibration.xaml.cpp
// Implementation of the Scenario3_Calibration class
//

#include "pch.h"
#include "Scenario3_Calibration.xaml.h"

using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario3_Calibration::Scenario3_Calibration() : rootPage(MainPage::Current)
{
    InitializeComponent();
    calibrationBar = ref new CalibrationBar();

    if (rootPage->SensorReadingType == SensorReadingType::Relative)
    {
        EnabledContent->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        DisabledContent->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

void Scenario3_Calibration::OnNavigatedFrom(NavigationEventArgs^ e)
{
    calibrationBar->Hide();
}

/// <summary>
/// This is the click handler for high accuracy. Acceptable accuracy met, so
/// hide the calibration bar.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_Calibration::OnHighAccuracy()
{
    calibrationBar->Hide();
}

/// <summary>
/// This is the click handler for approximate accuracy. Acceptable accuracy met, so
/// hide the calibration bar.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_Calibration::OnApproximateAccuracy()
{
    calibrationBar->Hide();
}

/// <summary>
/// This is the click handler for unreliable accuracy. Sensor does not meet accuracy
/// requirements. Request to show the calibration bar per the calibration guidance.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_Calibration::OnUnreliableAccuracy()
{
    calibrationBar->RequestCalibration(MagnetometerAccuracy::Unreliable);
}
