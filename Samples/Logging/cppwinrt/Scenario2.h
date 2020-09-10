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

#include "Scenario2.g.h"
#include "LoggingSessionScenario.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2 : Scenario2T<Scenario2>
    {
        Scenario2();

        fire_and_forget
        DoScenario(
            Windows::Foundation::IInspectable const& sender,
            Windows::UI::Xaml::RoutedEventArgs const& args);

        void
        EnableDisableLogging(
            Windows::Foundation::IInspectable const& sender,
            Windows::UI::Xaml::RoutedEventArgs const& args);

        void
            OnNavigatedTo(
                Windows::UI::Xaml::Navigation::NavigationEventArgs const& args);

        void
            OnNavigatedFrom(
                Windows::UI::Xaml::Navigation::NavigationEventArgs const& args);

    private:

        void
        UpdateControls();

        void
        AddMessage(_In_ LPCWSTR message);

        fire_and_forget
        OnStatusChanged(SDKTemplate::LoggingScenarioEventArgs args);

        LoggingSessionScenario& _instance = LoggingSessionScenario::Instance();
        event_token _statusChangedToken;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2 : Scenario2T<Scenario2, implementation::Scenario2>
    {
    };
}
