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

#include "Scenario5_LaunchedFromSecondaryTile.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_LaunchedFromSecondaryTile : Scenario5_LaunchedFromSecondaryTileT<Scenario5_LaunchedFromSecondaryTile>
    {
        Scenario5_LaunchedFromSecondaryTile();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_LaunchedFromSecondaryTile : Scenario5_LaunchedFromSecondaryTileT<Scenario5_LaunchedFromSecondaryTile, implementation::Scenario5_LaunchedFromSecondaryTile>
    {
    };
}
