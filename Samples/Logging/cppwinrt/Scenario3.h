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

#include "Scenario3.g.h"
#include "FileLoggingSessionScenario.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3 : Scenario3T<Scenario3>
    {
        Scenario3();

        fire_and_forget
        DoScenario(
            Windows::Foundation::IInspectable const& sender,
            Windows::UI::Xaml::RoutedEventArgs const& args);

        fire_and_forget
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

        void
        AddLogFileMessage(
            _In_ LPCWSTR message,
            _In_opt_ LPCWSTR logFileFullPath);

        fire_and_forget
        OnStatusChanged(SDKTemplate::LoggingScenarioEventArgs args);

        FileLoggingSessionScenario& _instance = FileLoggingSessionScenario::Instance();
        event_token _statusChangedToken;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3 : Scenario3T<Scenario3, implementation::Scenario3>
    {
    };
}
