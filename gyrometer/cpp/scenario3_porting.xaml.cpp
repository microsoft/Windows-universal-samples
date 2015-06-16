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
// Scenario3_Porting.xaml.cpp
// Implementation of the Scenario3_Porting class
//

#include "pch.h"
#include "Scenario3_Porting.xaml.h"

using namespace GyrometerCPP;
using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario3_Porting::Scenario3_Porting() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // Get two instances of the gyrometer:
    // One that returns the raw gyrometer data
    gyrometerWindows = Gyrometer::GetDefault();
    // Other on which the 'ReadingTransform' is updated so that data returned aligns with the native WP orientation (portrait)
    gyrometerWP = Gyrometer::GetDefault();

    if (nullptr == gyrometerWP || nullptr == gyrometerWindows)
    {
        rootPage->NotifyUser("No gyrometer found", NotifyType::ErrorMessage);
    }
    else
    {
        // Assumption is that this app has been developed for Windows Phone 8.1 (or earlier)
        // and hence assumes that the sensor returns readings in Portrait Mode, which may 
        // not be true when the app or sensor logic is being ported over to a 
        // Landscape-First Windows device
        // While we encourage you to re-design your app as a universal app to gain access 
        // to many other advantages of developing a universal app, this scenario demonstrates
        // a simple approach to let the runtime honor your assumption on the 
        // "native orientation" of the sensor.
        gyrometerWP->ReadingTransform = Windows::Graphics::Display::DisplayOrientations::Portrait;
        // If you were to go the route of universal app, make no assumptions about the 
        // native orientation of the device. Instead rely on using a display orientation 
        // (absolute or current) to enforce the reference frame for the sensor readings. 
        // (which is done by updating 'ReadingTransform' property with the appropriate orientation)
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3_Porting::OnNavigatedTo(NavigationEventArgs^ e)
{
    if (nullptr == gyrometerWP || nullptr == gyrometerWindows)
    {
        GetSampleButton->IsEnabled = false;
    }
    else
    {
        GetSampleButton->IsEnabled = true;
    }
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario3_Porting::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (NavigationMode::Forward == e->NavigationMode && nullptr == e->Uri)
    {
        return;
    }
}



/// <summary>
/// Invoked when 'Get Sample' button is pressed.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_Porting::GetGyrometerSample(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    GyrometerReading^ wpReading;
    GyrometerReading^ windowsReading;
    GetSampleButton->IsEnabled = false;

    // Establish the report interval
    gyrometerWP->ReportInterval = gyrometerWindows->MinimumReportInterval;
    gyrometerWindows->ReportInterval = gyrometerWindows->MinimumReportInterval;

    wpReading = gyrometerWP->GetCurrentReading();
    windowsReading = gyrometerWindows->GetCurrentReading();

    if (nullptr != wpReading)
    {
        ScenarioOutput_X_WP->Text = wpReading->AngularVelocityX.ToString();
        ScenarioOutput_Y_WP->Text = wpReading->AngularVelocityY.ToString();
        ScenarioOutput_Z_WP->Text = wpReading->AngularVelocityZ.ToString();
    }

    if (nullptr != windowsReading)
    {
        ScenarioOutput_X_Windows->Text = windowsReading->AngularVelocityX.ToString();
        ScenarioOutput_Y_Windows->Text = windowsReading->AngularVelocityY.ToString();
        ScenarioOutput_Z_Windows->Text = windowsReading->AngularVelocityZ.ToString();
    }
  
    gyrometerWP->ReportInterval = 0;
    gyrometerWindows->ReportInterval = 0;

    GetSampleButton->IsEnabled = true;
}
