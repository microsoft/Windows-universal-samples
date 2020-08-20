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

#include "Scenario1_CheckConsentAvailability.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_CheckConsentAvailability : Scenario1_CheckConsentAvailabilityT<Scenario1_CheckConsentAvailability>
    {
        Scenario1_CheckConsentAvailability();

        fire_and_forget CheckAvailability_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_CheckConsentAvailability : Scenario1_CheckConsentAvailabilityT<Scenario1_CheckConsentAvailability, implementation::Scenario1_CheckConsentAvailability>
    {
    };
}
