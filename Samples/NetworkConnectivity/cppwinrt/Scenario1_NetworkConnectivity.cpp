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
#include "Scenario1_NetworkConnectivity.h"
#include "Scenario1_NetworkConnectivity.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Networking::Connectivity;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_NetworkConnectivity::Scenario1_NetworkConnectivity()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_NetworkConnectivity::QueryCurrentNetworkConnectivityButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        ResultsText().Text(L"");

        ConnectionProfile profile = NetworkInformation::GetInternetConnectionProfile();
        NetworkConnectivityLevel connectivityLevel = NetworkConnectivityLevel::None;
        if (profile != nullptr)
        {
            // It can take the system a few moments to detect a change in network connectivity,
            // so it's possible that the value returned is slightly out of date.
            // Scenario3_NetworkConnectivityChanges shows how to be notified when the connectivity
            // changes.
            connectivityLevel = profile.GetNetworkConnectivityLevel();
        }

        AppendLine(ResultsText(), L"Current connectivity is " + to_hstring(connectivityLevel) + L".");
        if (ShouldAttemptToConnectToInternet(connectivityLevel))
        {
            co_await EvaluateCostAndConnectAsync(profile.GetConnectionCost(), OptedInToNetworkUsageToggle().IsOn(), ResultsText());
        }
        else
        {
            AppendLine(ResultsText(), L"Not attempting to connect to the Internet.");
        }
    }
}
