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

#include "Scenario6_SecondaryTileNotification.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario6_SecondaryTileNotification : Scenario6_SecondaryTileNotificationT<Scenario6_SecondaryTileNotification>
    {
        Scenario6_SecondaryTileNotification();

        fire_and_forget PinLiveTile_Click(Windows::Foundation::IInspectable sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SendTileNotification_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SendBadgeNotification_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SendTileNotificationWithStringManipulation_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void SendBadgeNotificationWithStringManipulation_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        bool VerifyTileIsPinned();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario6_SecondaryTileNotification : Scenario6_SecondaryTileNotificationT<Scenario6_SecondaryTileNotification, implementation::Scenario6_SecondaryTileNotification>
    {
    };
}
