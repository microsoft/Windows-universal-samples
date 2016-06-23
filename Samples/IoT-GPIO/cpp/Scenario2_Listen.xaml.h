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
// Scenario2_Listen.xaml.h
//

#pragma once

#include "Scenario2_Listen.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Listen sealed
    {
    public:
        Scenario2_Listen();
        void StartStopScenario();

    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs ^e) override;

    private:
        void StartScenario();
        void StopScenario();
        void Pin_ValueChanged(Windows::Devices::Gpio::GpioPin ^sender, Windows::Devices::Gpio::GpioPinValueChangedEventArgs ^e);
        void Timer_Tick(Platform::Object ^sender, Platform::Object ^e);

    private:
        const int SET_PIN = 5;
        const int LISTEN_PIN = 6;
        Windows::Devices::Gpio::GpioPin ^setPin;
        Windows::Devices::Gpio::GpioPin ^listenPin;
        Windows::Devices::Gpio::GpioPinValue currentValue;
        Windows::UI::Xaml::DispatcherTimer ^timer;
        Windows::Foundation::EventRegistrationToken valueChangedToken{};
        Windows::Foundation::EventRegistrationToken timerToken{};
    };
}
