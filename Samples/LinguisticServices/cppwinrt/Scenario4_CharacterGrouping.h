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

#include "Scenario4_CharacterGrouping.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_CharacterGrouping : Scenario4_CharacterGroupingT<Scenario4_CharacterGrouping>
    {
        Scenario4_CharacterGrouping();

        void GetGroups_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void AssignGroup_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Globalization::Collation::CharacterGroupings ActiveGroupings{ nullptr };
        std::map<hstring, Windows::UI::Xaml::UIElement> Groups;
        static constexpr wchar_t GlobeString[] = L"\U0001F310";

        void AddGroup(hstring const& label, Windows::UI::Xaml::UIElement const& element);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_CharacterGrouping : Scenario4_CharacterGroupingT<Scenario4_CharacterGrouping, implementation::Scenario4_CharacterGrouping>
    {
    };
}
