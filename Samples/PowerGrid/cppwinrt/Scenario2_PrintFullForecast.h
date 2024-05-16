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

#include "Scenario2_PrintFullForecast.g.h"
#include "ForecastItem.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct ForecastItem : ForecastItemT<ForecastItem>
    {
        ForecastItem(hstring const& dateTimeString, hstring const& severityString, hstring const& lowImpactString) :
            m_dateTimeString(dateTimeString), m_severityString(severityString), m_lowImpactString(lowImpactString)
        {
        }

        hstring DateTimeString() const { return m_dateTimeString; }
        hstring SeverityString() const { return m_severityString; }
        hstring LowImpactString() const { return m_lowImpactString; }

        const hstring m_dateTimeString;
        const hstring m_severityString;
        const hstring m_lowImpactString;
    };

    struct Scenario2_PrintFullForecast : Scenario2_PrintFullForecastT<Scenario2_PrintFullForecast>
    {
        Scenario2_PrintFullForecast();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget GetForecastButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        event_token forecastUpdatedToken;

        void PowerGridForecast_ForecastUpdated(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_PrintFullForecast : Scenario2_PrintFullForecastT<Scenario2_PrintFullForecast, implementation::Scenario2_PrintFullForecast>
    {
    };
}
