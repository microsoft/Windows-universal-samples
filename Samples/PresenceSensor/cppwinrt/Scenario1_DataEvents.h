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

#include "Scenario1_DataEvents.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_DataEvents : Scenario1_DataEventsT<Scenario1_DataEvents>
    {
        Scenario1_DataEvents();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        // These methods are public so they can be called by binding.
        fire_and_forget ScenarioGetSensor_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Devices::Sensors::HumanPresenceSensor sensor{ nullptr };
        event_token sensorReadingChangedToken{};

        fire_and_forget ReadingChanged(Windows::Devices::Sensors::HumanPresenceSensor const&, Windows::Devices::Sensors::HumanPresenceSensorReadingChangedEventArgs e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_DataEvents : Scenario1_DataEventsT<Scenario1_DataEvents, implementation::Scenario1_DataEvents>
    {
    };
}
