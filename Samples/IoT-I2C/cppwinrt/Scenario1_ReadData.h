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

#include "Scenario1_ReadData.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_ReadData : Scenario1_ReadDataT<Scenario1_ReadData>
    {
        Scenario1_ReadData();
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget StartStopScenario_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        Windows::Devices::I2c::I2cDevice htu21dSensor{ nullptr };
        Windows::UI::Xaml::DispatcherTimer timer{ nullptr };
        event_token timerEventToken{};

        Windows::Foundation::IAsyncAction StartScenarioAsync();
        void StopScenario();
        void Timer_Tick(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_ReadData : Scenario1_ReadDataT<Scenario1_ReadData, implementation::Scenario1_ReadData>
    {
    };
}
