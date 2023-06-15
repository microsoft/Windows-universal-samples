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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Networking::Connectivity;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::Web::Http;
using namespace winrt::SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"NetworkConnectivity C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Query network connectivity", xaml_typename<SDKTemplate::Scenario1_NetworkConnectivity>() },
    Scenario{ L"Get network cost information", xaml_typename<SDKTemplate::Scenario2_NetworkCost>() },
    Scenario{ L"Listen to connectivity changes", xaml_typename<SDKTemplate::Scenario3_NetworkConnectivityChanges>() },
});

hstring winrt::to_hstring(NetworkConnectivityLevel value)
{
    switch (value)
    {
    case NetworkConnectivityLevel::None: return L"None";
    case NetworkConnectivityLevel::LocalAccess: return L"LocalAccess";
    case NetworkConnectivityLevel::ConstrainedInternetAccess: return L"ConstrainedInternetAccess";
    case NetworkConnectivityLevel::InternetAccess: return L"InternetAccess";
    }
    return to_hstring(static_cast<int32_t>(value));
}

hstring winrt::to_hstring(NetworkCostType type)
{
    switch (type)
    {
    case NetworkCostType::Unknown: return L"Unknown";
    case NetworkCostType::Unrestricted: return L"Unrestricted";
    case NetworkCostType::Fixed: return L"Fixed";
    case NetworkCostType::Variable: return L"Variable";
    }
    return winrt::to_hstring(static_cast<int32_t>(type));
}

void winrt::SDKTemplate::AppendLine(TextBlock const& textBlock, hstring const& message)
{
    textBlock.Text(textBlock.Text() + message + L"\n\n");
}

bool winrt::SDKTemplate::ShouldAttemptToConnectToInternet(NetworkConnectivityLevel level)
{
    bool shouldConnectToInternet = false;
    if (level == NetworkConnectivityLevel::LocalAccess || level == NetworkConnectivityLevel::InternetAccess)
    {
        shouldConnectToInternet = true;
    }

    return shouldConnectToInternet;
}

namespace
{
    IAsyncOperation<hstring> SendHttpGetRequestAsync()
    {
        HttpClient httpClient;
        HttpGetStringResult result = co_await httpClient.TryGetStringAsync(Uri(L"http://www.msftconnecttest.com/connecttest.txt"));
        if (result.Succeeded())
        {
            co_return L"Success: \"" + result.Value() + L"\"";
        }
        else if (HttpResponseMessage responseMessage = result.ResponseMessage(); responseMessage != nullptr)
        {
            co_return L"HTTP error: " + to_hstring(static_cast<int32_t>(responseMessage.StatusCode()));
        }
        else
        {
            co_return L"Error: " + hresult_error(result.ExtendedError(), take_ownership_from_abi).message();
        }
    }
}


CostGuidance winrt::SDKTemplate::GetNetworkCostGuidance(ConnectionCost cost)
{
    CostGuidance costGuidance = CostGuidance::Normal;
    if (cost.Roaming() || cost.OverDataLimit())
    {
        costGuidance = CostGuidance::OptIn;
    }
    else if (cost.NetworkCostType() == NetworkCostType::Fixed || cost.NetworkCostType() == NetworkCostType::Variable)
    {
        costGuidance = CostGuidance::Conservative;
    }
    else
    {
        costGuidance = CostGuidance::Normal;
    }
    return costGuidance;
}

void winrt::SDKTemplate::EvaluateAndReportConnectionCost(ConnectionCost cost, TextBlock const& textBlock)
{
    CostGuidance costGuidance = GetNetworkCostGuidance(cost);
    switch (costGuidance)
    {
        // In opt-in scenarios, apps handle cases where the network access cost is significantly higher than the plan cost.
        // For example, when a user is roaming, a mobile carrier may charge a higher rate data usage.
    case CostGuidance::OptIn:
        AppendLine(textBlock, L"Apps should implement opt-in behavior.");
        break;
        // In conservative scenarios, apps implement restrictions for optimizing network usage to handle transfers over metered networks.
    case CostGuidance::Conservative:
        AppendLine(textBlock, L"Apps should implement conservative behavior.");
        break;
        // In normal scenarios, apps do not implement restrictions. Apps treat the connection as unlimited in cost.
    case CostGuidance::Normal:
    default:
        AppendLine(textBlock, L"Apps should implement normal behavior.");
        break;
    }
}

IAsyncAction winrt::SDKTemplate::EvaluateCostAndConnectAsync(ConnectionCost cost, bool optedInToCharges, TextBlock textBlock)
{
    CostGuidance costGuidance = GetNetworkCostGuidance(cost);

    switch (costGuidance)
    {
    case CostGuidance::OptIn:
    {
        AppendLine(textBlock, L"Connecting to the Internet may incur charges.");
        if (optedInToCharges)
        {
            AppendLine(textBlock, L"User has opted in to additional network usage charges.");
            AppendLine(textBlock, co_await SendHttpGetRequestAsync());
        }
        else
        {
            AppendLine(textBlock, L"User has opted out of additional network usage charges. Not connecting to the Internet.");
        }
    }
    break;
    case CostGuidance::Conservative:
        AppendLine(textBlock, L"Attempting to connect to the Internet, optimizing traffic for conservative network usage.");
        AppendLine(textBlock, co_await SendHttpGetRequestAsync());
        break;
    case CostGuidance::Normal:
    default:
        AppendLine(textBlock, L"Attempting to connect to the Internet.");
        AppendLine(textBlock, co_await SendHttpGetRequestAsync());
        break;
    }
}
