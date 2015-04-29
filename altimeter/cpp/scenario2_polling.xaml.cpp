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

using namespace AltimeterCPP;
using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_Polling::Scenario2_Polling() : rootPage(MainPage::Current)
{
    InitializeComponent();

    sensor = Altimeter::GetDefault();
    if (nullptr == sensor)
    {
        rootPage->NotifyUser("No altimeter found", NotifyType::ErrorMessage);
    }
}

void Scenario2_Polling::ScenarioGetData(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (nullptr != sensor)
    {
        AltimeterReading^ reading = sensor->GetCurrentReading();
        if (nullptr != reading)
        {
            ScenarioOutput_M->Text = reading->AltitudeChangeInMeters.ToString();
        }
    }
    else
    {
        rootPage->NotifyUser("No altimeter found", NotifyType::ErrorMessage);
    }
}
