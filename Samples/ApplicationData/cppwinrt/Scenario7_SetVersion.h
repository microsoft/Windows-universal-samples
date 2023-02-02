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

#include "Scenario7_SetVersion.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario7_SetVersion : Scenario7_SetVersionT<Scenario7_SetVersion>
    {
        Scenario7_SetVersion();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        fire_and_forget SetVersion0_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget SetVersion1_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

        void DisplayOutput();

    private:
        
        void SetVersionHandler0(Windows::Storage::SetVersionRequest const& request);
        void SetVersionHandler1(Windows::Storage::SetVersionRequest const& request);

        Windows::Storage::ApplicationData appData = Windows::Storage::ApplicationData::Current();

        inline static const auto settingName = L"SetVersionSetting";
        inline static const auto settingValue0 = L"Data.v0";
        inline static const auto settingValue1 = L"Data.v1";
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario7_SetVersion : Scenario7_SetVersionT<Scenario7_SetVersion, implementation::Scenario7_SetVersion>
    {
    };
}

