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

#include "Scenario6_GetLastVisit.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario6_GetLastVisit : Scenario6_GetLastVisitT<Scenario6_GetLastVisit>
    {
        Scenario6_GetLastVisit();

        fire_and_forget GetLastVisitButtonClicked(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };

        void UpdateLastVisit(Windows::Devices::Geolocation::Geovisit const& visit);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario6_GetLastVisit : Scenario6_GetLastVisitT<Scenario6_GetLastVisit, implementation::Scenario6_GetLastVisit>
    {
    };
}
