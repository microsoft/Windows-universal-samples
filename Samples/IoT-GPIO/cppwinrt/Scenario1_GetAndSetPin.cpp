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
#include "Scenario1_GetAndSetPin.h"
#include "Scenario1_GetAndSetPin.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Devices::Gpio;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_GetAndSetPin::Scenario1_GetAndSetPin()
    {
        InitializeComponent();
    }

    void Scenario1_GetAndSetPin::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        StopScenario();
    }

    void Scenario1_GetAndSetPin::StartScenario()
    {
        GpioController gpio = GpioController::GetDefault();

        // Set up our GPIO pin for setting values.
        // If this next line crashes with a NullReferenceException,
        // then the problem is that there is no GPIO controller on the device.
        pin = gpio.OpenPin(LED_PIN);

        // Configure pin for output.
        pin.SetDriveMode(GpioPinDriveMode::Output);
    }

    void Scenario1_GetAndSetPin::StopScenario()
    {
        // Release the GPIO pin.
        if (pin != nullptr)
        {
            pin.Close();
            pin = nullptr;
        }
    }

    void Scenario1_GetAndSetPin::StartStopScenario_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pin != nullptr)
        {
            StopScenario();
            StartStopButton().Content(box_value(L"Start"));
            ScenarioControls().Visibility(Visibility::Collapsed);
        }
        else
        {
            StartScenario();
            StartStopButton().Content(box_value(L"Stop"));
            ScenarioControls().Visibility(Visibility::Visible);
        }
    }

    void Scenario1_GetAndSetPin::SetPinHigh_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Set the pin value to High.
        pin.Write(GpioPinValue::High);
    }

    void Scenario1_GetAndSetPin::SetPinLow_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Set the pin value to Low.
        pin.Write(GpioPinValue::Low);
    }

    void Scenario1_GetAndSetPin::GetPinValue_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Change the GUI to reflect the current pin value.
        CurrentPinValue().Text(PinValueToString(pin.Read()));
    }

}
