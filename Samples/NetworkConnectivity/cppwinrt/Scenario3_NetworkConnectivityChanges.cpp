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

#include "pch.h"
#include "Scenario3_NetworkConnectivityChanges.h"
#include "Scenario3_NetworkConnectivityChanges.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Networking::Connectivity;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_NetworkConnectivityChanges::Scenario3_NetworkConnectivityChanges()
    {
        InitializeComponent();
    }

    void Scenario3_NetworkConnectivityChanges::OnNavigatedTo(NavigationEventArgs const&)
    {
        UpdateButtonStates();
    }

    void Scenario3_NetworkConnectivityChanges::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (m_networkStatusChangeEventToken)
        {
            UnregisterNetworkStatusChanged();
        }
    }

    fire_and_forget Scenario3_NetworkConnectivityChanges::UpdateNetworkStatus()
    {
        // This code runs on the UI thread, so there are no race conditions on these
        // member variables.
        if (m_isUpdatingNetworkStatus)
        {
            // Update is already in progress. Let it finish before we re-evaluate.
            m_isNetworkStatusUpdatePending = true;
            co_return;
        }

        auto lifetime = get_strong();

        m_isUpdatingNetworkStatus = true;

        do
        {
            m_isNetworkStatusUpdatePending = false;
            ResultsText().Text(L"");

            ConnectionProfile internetConnectionProfile = NetworkInformation::GetInternetConnectionProfile();
            NetworkConnectivityLevel connectivityLevel = NetworkConnectivityLevel::None;
            if (internetConnectionProfile != nullptr)
            {
                connectivityLevel = internetConnectionProfile.GetNetworkConnectivityLevel();
            }

            AppendLine(ResultsText(), L"Current connectivity is " + to_hstring(connectivityLevel) + L".");
            if (ShouldAttemptToConnectToInternet(connectivityLevel))
            {
                AppendLine(ResultsText(), L"Checking network cost before connecting to the Internet.");
                co_await EvaluateCostAndConnectAsync(internetConnectionProfile.GetConnectionCost(), OptedInToNetworkUsageToggle().IsOn(), ResultsText());
            }
            else
            {
                AppendLine(ResultsText(), L"Not attempting to connect to the Internet.");
            }
        }
        while (m_isNetworkStatusUpdatePending);
        m_isUpdatingNetworkStatus = false;
    }

    fire_and_forget Scenario3_NetworkConnectivityChanges::OnNetworkStatusChanged(IInspectable const&)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(Dispatcher());

        UpdateNetworkStatus();
    }

    void Scenario3_NetworkConnectivityChanges::RegisterNetworkStatusChanged()
    {
        m_networkStatusChangeEventToken = NetworkInformation::NetworkStatusChanged({ get_weak(), &Scenario3_NetworkConnectivityChanges::OnNetworkStatusChanged });
        UpdateNetworkStatus();
    }

    void Scenario3_NetworkConnectivityChanges::UnregisterNetworkStatusChanged()
    {
        NetworkInformation::NetworkStatusChanged(std::exchange(m_networkStatusChangeEventToken, {}));
    }

    void Scenario3_NetworkConnectivityChanges::RegisterUnregisterButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (m_networkStatusChangeEventToken)
        {
            UnregisterNetworkStatusChanged();
        }
        else
        {
            RegisterNetworkStatusChanged();
        }
        UpdateButtonStates();
    }

    /// <summary>
    /// Update the UI based on whether we are registered for the event.
    /// </summary>
    void Scenario3_NetworkConnectivityChanges::UpdateButtonStates()
    {
        VisualStateManager::GoToState(*this, m_networkStatusChangeEventToken ? L"Registered" : L"Unregistered", false);
    }
}
