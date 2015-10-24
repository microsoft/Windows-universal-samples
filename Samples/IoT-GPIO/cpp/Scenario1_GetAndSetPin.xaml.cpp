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
#include "Scenario1_GetAndSetPin.xaml.h"

using namespace SDKTemplate;

using namespace Windows::Devices::Gpio;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_GetAndSetPin::Scenario1_GetAndSetPin()
{
    InitializeComponent();
}

void Scenario1_GetAndSetPin::OnNavigatedFrom(NavigationEventArgs ^e)
{
    StopScenario();
}

void Scenario1_GetAndSetPin::StartScenario()
{
    auto gpio = GpioController::GetDefault();

    // Set up our GPIO pin for setting values.
    // If this next line crashes with an Access Violation reading 0x00000000,
    // then the problem is that there is no GPIO controller on the device.
    pin = gpio->OpenPin(LED_PIN);

    // Configure pin for output.
    pin->SetDriveMode(GpioPinDriveMode::Output);
}

void Scenario1_GetAndSetPin::StopScenario()
{
    // Release the GPIO pin.
    delete pin;
    pin = nullptr;
}

void Scenario1_GetAndSetPin::StartStopScenario()
{
    if (pin != nullptr)
    {
        StopScenario();
        StartStopButton->Content = "Start";
        ScenarioControls->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    else
    {
        StartScenario();
        StartStopButton->Content = "Stop";
        ScenarioControls->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

void Scenario1_GetAndSetPin::SetPinHigh()
{
    // Set the GPIO pin to High.
    pin->Write(GpioPinValue::High);
}

void Scenario1_GetAndSetPin::SetPinLow()
{
    // Set the GPIO pin to Low.
    pin->Write(GpioPinValue::Low);
}

void Scenario1_GetAndSetPin::GetPinValue()
{
    // Change the GUI to reflect the current pin value.
    CurrentPinValue->Text = pin->Read().ToString();
}


