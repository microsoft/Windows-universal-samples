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
#include "Scenario2_NetworkCost.h"
#include "Scenario2_NetworkCost.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Connectivity;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_NetworkCost::Scenario2_NetworkCost()
    {
        InitializeComponent();
    }

    void Scenario2_NetworkCost::GetNetworkCost_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ResultsText().Text(L"");

        ConnectionProfile profile = NetworkInformation::GetInternetConnectionProfile();
        if (profile != nullptr)
        {
            ConnectionCost connectionCost = profile.GetConnectionCost();
            AppendLine(ResultsText(), L"Network cost is: " + to_hstring(connectionCost.NetworkCostType()) + L".");
            EvaluateAndReportConnectionCost(connectionCost, ResultsText());
        }
        else
        {
            AppendLine(ResultsText(), L"No internet connection profile.");
        }
    }
}
