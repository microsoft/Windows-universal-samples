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

#include "Scenario2_Settings.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Settings : Scenario2_SettingsT<Scenario2_Settings>
    {
        Scenario2_Settings();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void WriteSetting_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const& e);
        void DeleteSetting_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:

        void DisplayOutput();

        Windows::Storage::ApplicationDataContainer localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();

        inline static const auto SettingName = L"exampleSetting";
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Settings : Scenario2_SettingsT<Scenario2_Settings, implementation::Scenario2_Settings>
    {
    };
}

