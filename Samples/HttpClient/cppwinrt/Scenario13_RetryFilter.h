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

#include "Scenario13_RetryFilter.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario13_RetryFilter : Scenario13_RetryFilterT<Scenario13_RetryFilter>
    {
        Scenario13_RetryFilter();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void RetryAfterSwitch_Toggled(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget Start_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void Cancel_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Web::Http::HttpClient httpClient{ nullptr };
        Windows::Foundation::IAsyncAction pendingAction{ nullptr };

        void UpdateAddressField();
        Windows::Foundation::IAsyncAction CancelableRequestAsync(Windows::Foundation::Uri resourceUri);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario13_RetryFilter : Scenario13_RetryFilterT<Scenario13_RetryFilter, implementation::Scenario13_RetryFilter>
    {
    };
}
