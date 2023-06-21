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

#include "Scenario3_Choosing.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_Choosing : Scenario3_ChoosingT<Scenario3_Choosing>
    {
        Scenario3_Choosing();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        // These methods are public so they can be called by binding.
        fire_and_forget ScenarioPick_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void ScenarioStart_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void ScenarioStop_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget ScenarioReadSelected_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Devices::Enumeration::DeviceWatcher watcher{ nullptr };

        fire_and_forget OnHumanPresenceSensorAdded(Windows::Devices::Enumeration::DeviceWatcher const& sender, Windows::Devices::Enumeration::DeviceInformation deviceInfo);
        fire_and_forget OnHumanPresenceSensorRemoved(Windows::Devices::Enumeration::DeviceWatcher const& sender, Windows::Devices::Enumeration::DeviceInformationUpdate update);
        fire_and_forget OnHumanPresenceSensorUpdated(Windows::Devices::Enumeration::DeviceWatcher const& sender, Windows::Devices::Enumeration::DeviceInformationUpdate update);
        fire_and_forget OnHumanPresenceSensorEnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher const& sender, Windows::Foundation::IInspectable const& e);

        Windows::Foundation::IAsyncAction ReadOneReadingFromSensorAsync(Windows::Devices::Enumeration::DeviceInformation deviceInfo);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_Choosing : Scenario3_ChoosingT<Scenario3_Choosing, implementation::Scenario3_Choosing>
    {
    };
}
