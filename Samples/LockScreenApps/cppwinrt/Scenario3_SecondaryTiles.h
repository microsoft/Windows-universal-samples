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
#include "Scenario3_SecondaryTiles.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_SecondaryTiles : Scenario3_SecondaryTilesT<Scenario3_SecondaryTiles>
    {
        Scenario3_SecondaryTiles();

        fire_and_forget CreateBadgeTile_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget CreateBadgeAndTextTile_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        static constexpr wchar_t BADGE_TILE_ID[] = L"ST_BADGE";
        static constexpr wchar_t TEXT_TILE_ID[] = L"ST_BADGE_AND_TEXT";

        static Windows::Foundation::Rect GetElementRect(Windows::UI::Xaml::FrameworkElement const& element);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_SecondaryTiles : Scenario3_SecondaryTilesT<Scenario3_SecondaryTiles, implementation::Scenario3_SecondaryTiles>
    {
    };
}
