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

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Choose accelerometer", "SDKTemplate.Scenario0_Choose" },
    { "Data Events", "SDKTemplate.Scenario1_DataEvents" },
    { "Shake Events", "SDKTemplate.Scenario2_ShakeEvents" },
    { "Polling", "SDKTemplate.Scenario3_Polling" },
    { "OrientationChanged", "SDKTemplate.Scenario4_OrientationChanged" },
    { "Data Events Batching", "SDKTemplate.Scenario5_DataEventsBatching" }
};

void MainPage::SetReadingText(Windows::UI::Xaml::Controls::TextBlock^ textBlock, Windows::Devices::Sensors::AccelerometerReading^ reading)
{
    textBlock->Text = "X: " + reading->AccelerationX.ToString() +
        ", Y: " + reading->AccelerationY.ToString() +
        ", Z: " + reading->AccelerationZ.ToString();
}
