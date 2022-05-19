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

#include "Scenario3_SettingContainer.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_SettingContainer : Scenario3_SettingContainerT<Scenario3_SettingContainer>
    {
        Scenario3_SettingContainer();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void CreateContainer_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void DeleteContainer_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void WriteSetting_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void DeleteSetting_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:

        void DisplayOutput();

        Windows::Storage::ApplicationDataContainer localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();

        inline static const auto containerName = L"exampleContainer";
        inline static const auto settingName = L"exampleSetting";
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_SettingContainer : Scenario3_SettingContainerT<Scenario3_SettingContainer, implementation::Scenario3_SettingContainer>
    {
    };
}

