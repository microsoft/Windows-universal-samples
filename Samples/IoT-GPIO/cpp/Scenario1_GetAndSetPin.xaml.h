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
// Scenario1_GetAndSetPin.xaml.h
//

#pragma once

#include "Scenario1_GetAndSetPin.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_GetAndSetPin sealed
    {
    public:
        Scenario1_GetAndSetPin();
        void StartStopScenario_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SetPinHigh_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SetPinLow_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GetPinValue_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs ^e) override;

    private:
        void StartScenario();
        void StopScenario();

    private:
        const int LED_PIN = 5;
        Windows::Devices::Gpio::GpioPin ^pin;
    };
}
