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

#include "Scenario2_Polling.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Polling : Scenario2_PollingT<Scenario2_Polling>
    {
        Scenario2_Polling();

        // These methods are public so they can be called by binding.
        fire_and_forget ScenarioGetSensor_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void ScenarioGetData_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Devices::Sensors::HumanPresenceSensor sensor{ nullptr };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Polling : Scenario2_PollingT<Scenario2_Polling, implementation::Scenario2_Polling>
    {
    };
}
