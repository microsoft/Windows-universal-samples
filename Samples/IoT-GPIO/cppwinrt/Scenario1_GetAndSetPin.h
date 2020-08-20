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

#pragma once

#include "Scenario1_GetAndSetPin.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_GetAndSetPin : Scenario1_GetAndSetPinT<Scenario1_GetAndSetPin>
    {
        Scenario1_GetAndSetPin();
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void StartStopScenario_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SetPinHigh_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SetPinLow_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void GetPinValue_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        // Use GPIO pin 5 to set values
        static constexpr int LED_PIN = 5;
        Windows::Devices::Gpio::GpioPin pin{ nullptr };

        void StartScenario();
        void StopScenario();

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_GetAndSetPin : Scenario1_GetAndSetPinT<Scenario1_GetAndSetPin, implementation::Scenario1_GetAndSetPin>
    {
    };
}
