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
#include "Scenario3_NetworkConnectivityChanges.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_NetworkConnectivityChanges : Scenario3_NetworkConnectivityChangesT<Scenario3_NetworkConnectivityChanges>
    {
        Scenario3_NetworkConnectivityChanges();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void RegisterUnregisterButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OptInOutComboBox_SelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&);

    private:
        void UpdateButtonStates();
        fire_and_forget UpdateNetworkStatus();
        fire_and_forget OnNetworkStatusChanged(Windows::Foundation::IInspectable const&);
        void RegisterNetworkStatusChanged();
        void UnregisterNetworkStatusChanged();

        winrt::event_token m_networkStatusChangeEventToken{};
        bool m_isUpdatingNetworkStatus = false;
        bool m_isNetworkStatusUpdatePending = false;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_NetworkConnectivityChanges : Scenario3_NetworkConnectivityChangesT<Scenario3_NetworkConnectivityChanges, implementation::Scenario3_NetworkConnectivityChanges>
    {
    };
}
