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

#include "Scenario1_Toggle.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Toggle : Scenario1_ToggleT<Scenario1_Toggle>
    {
        Scenario1_Toggle();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Toggle : Scenario1_ToggleT<Scenario1_Toggle, implementation::Scenario1_Toggle>
    {
    };
}
