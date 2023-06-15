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
#include "Scenario1_NetworkConnectivity.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_NetworkConnectivity : Scenario1_NetworkConnectivityT<Scenario1_NetworkConnectivity>
    {
        Scenario1_NetworkConnectivity();
        fire_and_forget QueryCurrentNetworkConnectivityButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_NetworkConnectivity : Scenario1_NetworkConnectivityT<Scenario1_NetworkConnectivity, implementation::Scenario1_NetworkConnectivity>
    {
    };
}
