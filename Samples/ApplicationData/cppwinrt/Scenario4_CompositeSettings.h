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

#include "Scenario4_CompositeSettings.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_CompositeSettings : Scenario4_CompositeSettingsT<Scenario4_CompositeSettings>
    {
        Scenario4_CompositeSettings();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void WriteCompositeSetting_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void DeleteCompositeSetting_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:

        void DisplayOutput();

        Windows::Storage::ApplicationDataContainer localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();

        inline static const auto settingName = L"exampleCompositeSetting";
        inline static const auto settingName1 = L"one";
        inline static const auto settingName2 = L"hello";
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_CompositeSettings : Scenario4_CompositeSettingsT<Scenario4_CompositeSettings, implementation::Scenario4_CompositeSettings>
    {
    };
}

