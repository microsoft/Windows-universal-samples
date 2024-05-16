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

#include "Scenario1_FindBest.g.h"
#include "MainPage.h"

#include "SampleConfiguration.h"// TEMP
namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_FindBest : Scenario1_FindBestT<Scenario1_FindBest>
    {
        Scenario1_FindBest();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget FindBest_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        
    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        event_token forecastUpdatedToken;

        void PowerGridForecast_ForecastUpdated(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
        int GetForecastIndexContainingTime(Windows::Devices::Power::PowerGridForecast const& gridForecast, Windows::Foundation::DateTime time);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_FindBest : Scenario1_FindBestT<Scenario1_FindBest, implementation::Scenario1_FindBest>
    {
    };
}
