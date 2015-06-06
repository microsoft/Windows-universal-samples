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

using namespace OrientationCPP;
using namespace Calibration;
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
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3_Calibration::OnNavigatedTo(NavigationEventArgs^ e)
{
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario3_Calibration::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    calibrationBar->Hide();
}

/// <summary>
/// This is the click handler for high accuracy. Acceptable accuracy met, so
/// hide the calibration bar.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_Calibration::OnHighAccuracy(Object^ sender, RoutedEventArgs^ e)
{
    calibrationBar->Hide();
}

/// <summary>
/// This is the click handler for approximate accuracy. Acceptable accuracy met, so
/// hide the calibration bar.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_Calibration::OnApproximateAccuracy(Object^ sender, RoutedEventArgs^ e)
{
    calibrationBar->Hide();
}

/// <summary>
/// This is the click handler for unreliable accuracy. Sensor does not meet accuracy
/// requirements. Request to show the calibration bar per the calibration guidance.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_Calibration::OnUnreliableAccuracy(Object^ sender, RoutedEventArgs^ e)
{
    calibrationBar->RequestCalibration(MagnetometerAccuracy::Unreliable);
}
