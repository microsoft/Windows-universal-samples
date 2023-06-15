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
#include "pch.h"

namespace winrt
{
    hstring to_hstring(Windows::Networking::Connectivity::NetworkConnectivityLevel value);
    hstring to_hstring(Windows::Networking::Connectivity::NetworkCostType type);
}

namespace winrt::SDKTemplate
{
    void AppendLine(Windows::UI::Xaml::Controls::TextBlock const& textBlock, hstring const& message);

    enum class CostGuidance
    {
        Normal,
        OptIn,
        Conservative
    };

    bool ShouldAttemptToConnectToInternet(Windows::Networking::Connectivity::NetworkConnectivityLevel level);
    CostGuidance GetNetworkCostGuidance(Windows::Networking::Connectivity::ConnectionCost cost);
    void EvaluateAndReportConnectionCost(Windows::Networking::Connectivity::ConnectionCost cost, Windows::UI::Xaml::Controls::TextBlock const& textBlock);
    winrt::Windows::Foundation::IAsyncAction EvaluateCostAndConnectAsync(Windows::Networking::Connectivity::ConnectionCost cost, bool optedInToCharges, Windows::UI::Xaml::Controls::TextBlock textBlock);
}

