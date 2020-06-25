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

#include "Scenario2_UnpinTile.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_UnpinTile : Scenario2_UnpinTileT<Scenario2_UnpinTile>
    {
        Scenario2_UnpinTile();

        fire_and_forget UnpinSecondaryTile_Click(Windows::Foundation::IInspectable sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_UnpinTile : Scenario2_UnpinTileT<Scenario2_UnpinTile, implementation::Scenario2_UnpinTile>
    {
    };
}
