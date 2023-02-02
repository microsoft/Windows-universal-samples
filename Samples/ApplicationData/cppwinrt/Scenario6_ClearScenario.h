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

#include "Scenario6_ClearScenario.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario6_ClearScenario : Scenario6_ClearScenarioT<Scenario6_ClearScenario>
    {
        Scenario6_ClearScenario();

        fire_and_forget Clear_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario6_ClearScenario : Scenario6_ClearScenarioT<Scenario6_ClearScenario, implementation::Scenario6_ClearScenario>
    {
    };
}

