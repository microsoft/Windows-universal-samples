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

#include "HistoryAndRoaming.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct HistoryAndRoaming : HistoryAndRoamingT<HistoryAndRoaming>
    {
        HistoryAndRoaming();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget CopyButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        event_token historyEnabledToken;
        event_token roamingEnabledToken;

        void OnHistoryEnabledChanged(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
        void OnRoamingEnabledChanged(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
        static void SelectVisibleElement(Windows::UI::Xaml::UIElement const& ifTrue, Windows::UI::Xaml::UIElement const& ifFalse, bool value);
        fire_and_forget CheckHistoryAndRoaming();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct HistoryAndRoaming : HistoryAndRoamingT<HistoryAndRoaming, implementation::HistoryAndRoaming>
    {
    };
}
