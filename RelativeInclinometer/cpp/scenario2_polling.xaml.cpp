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

using namespace RelativeInclinometerCPP;
using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;

Scenario2_Polling::Scenario2_Polling() : rootPage(MainPage::Current)
{
    InitializeComponent();

    inclinometer = Inclinometer::GetDefaultForRelativeReadings();
    if (inclinometer == nullptr)
    {
        rootPage->NotifyUser("No relative inclinometer found", NotifyType::ErrorMessage);
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
void Scenario2_Polling::OnNavigatedTo(NavigationEventArgs^ e)
{
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
}

void Scenario2_Polling::GetData(Object^ sender, Object^ e)
{
    InclinometerReading^ reading = inclinometer->GetCurrentReading();
    if (reading != nullptr)
    {
        ScenarioOutput_X->Text = reading->PitchDegrees.ToString();
        ScenarioOutput_Y->Text = reading->RollDegrees.ToString();
        ScenarioOutput_Z->Text = reading->YawDegrees.ToString();
    }
}
