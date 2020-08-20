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

#include "Scenario2_Listen.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Listen : Scenario2_ListenT<Scenario2_Listen>
    {
        Scenario2_Listen();
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void StartStopScenario_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        // Use GPIO pin 5 to set values
        static constexpr int SET_PIN = 5;
        Windows::Devices::Gpio::GpioPin setPin{ nullptr };

        // Use GPIO pin 6 to listen for value changes
        static constexpr int LISTEN_PIN = 6;
        Windows::Devices::Gpio::GpioPin listenPin{ nullptr };
        event_token valueChangedEventToken{};

        Windows::Devices::Gpio::GpioPinValue currentValue = Windows::Devices::Gpio::GpioPinValue::High;
        Windows::UI::Xaml::DispatcherTimer timer{ nullptr };
        event_token timerEventToken{};

    private:
        void StartScenario();
        void StopScenario();
        fire_and_forget Pin_ValueChanged(Windows::Devices::Gpio::GpioPin const& sender, Windows::Devices::Gpio::GpioPinValueChangedEventArgs const& e);
        void Timer_Tick(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Listen : Scenario2_ListenT<Scenario2_Listen, implementation::Scenario2_Listen>
    {
    };
}
