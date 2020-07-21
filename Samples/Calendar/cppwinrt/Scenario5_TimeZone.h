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

#include "Scenario5_TimeZone.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_TimeZone : Scenario5_TimeZoneT<Scenario5_TimeZone>
    {
        Scenario5_TimeZone();

        void ShowResults_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_TimeZone : Scenario5_TimeZoneT<Scenario5_TimeZone, implementation::Scenario5_TimeZone>
    {
    };
}
