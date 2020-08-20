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
#include "HttpMeteredConnectionFilter.h"
#include "HttpMeteredConnectionFilter.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Networking::Connectivity;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

namespace winrt::HttpFilters::implementation
{
    HttpMeteredConnectionFilter::HttpMeteredConnectionFilter(IHttpFilter const& innerFilter) : m_innerFilter(innerFilter)
    {
        if (!m_innerFilter)
        {
            throw hresult_invalid_argument(L"innerFilter cannot be null.");
        }
    }

    IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> HttpMeteredConnectionFilter::SendRequestAsync(HttpRequestMessage request)
    {
        auto lifetime = get_strong();

        MeteredConnectionPriority priority = MeteredConnectionPriority::Low;
        auto value = request.Properties().TryLookup(MeteredConnectionPriorityPropertyNameLiteral).try_as<IPropertyValue>();
        if (value)
        {
            if (value.Type() == PropertyType::Double)
            {
                // Numeric values in JavaScript are represented as doubles.
                priority = static_cast<MeteredConnectionPriority>(value.GetDouble());
            }
            else
            {
                priority = unbox_value_or(value, MeteredConnectionPriority::Low);
            }
        }

        CheckPriority(priority);

        return m_innerFilter.SendRequestAsync(request);
    }

    void HttpMeteredConnectionFilter::CheckPriority(MeteredConnectionPriority priority)
    {
        MeteredConnectionBehavior behavior = GetBehavior();

        switch (behavior)
        {
        case MeteredConnectionBehavior::Normal:
            // Under normal behavior all requests are allowed.
            return;

        case MeteredConnectionBehavior::Conservative:
            // Under conservative behavior all requests except low priority requests are allowed.
            if (priority != MeteredConnectionPriority::Low)
            {
                return;
            }
            break;

        case MeteredConnectionBehavior::OptIn:
            // Under opt-in only high priority requests are allowed and only if user opted in.
            if (priority == MeteredConnectionPriority::High && OptIn())
            {
                return;
            }
            break;
        }

        // Everything else is not allowed.
        throw hresult_access_denied(L"The request priority is not allowed under the current connection behavior.");
    }

    HttpMeteredConnectionFilter::MeteredConnectionBehavior HttpMeteredConnectionFilter::GetBehavior()
    {
        ConnectionProfile connectionProfile = NetworkInformation::GetInternetConnectionProfile();

        if (connectionProfile == nullptr)
        {
            return MeteredConnectionBehavior::None;
        }

        ConnectionCost connectionCost = connectionProfile.GetConnectionCost();

        if (connectionCost.Roaming())
        {
            return MeteredConnectionBehavior::OptIn;
        }

        NetworkCostType type = connectionCost.NetworkCostType();
        if (type == NetworkCostType::Unrestricted || type == NetworkCostType::Unknown)
        {
            return MeteredConnectionBehavior::Normal;
        }

        else if (!connectionCost.OverDataLimit() && (type == NetworkCostType::Fixed || type == NetworkCostType::Variable))
        {
            return MeteredConnectionBehavior::Conservative;
        }

        return MeteredConnectionBehavior::OptIn;
    }

}
