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

#include "Scenario1_Launched.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Launched : Scenario1_LaunchedT<Scenario1_Launched>
    {
        Scenario1_Launched();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Launched : Scenario1_LaunchedT<Scenario1_Launched, implementation::Scenario1_Launched>
    {
    };
}
