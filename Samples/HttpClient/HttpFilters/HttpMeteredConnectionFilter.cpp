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
#include <ppltasks.h>
#include <winerror.h>

using namespace Concurrency;
using namespace HttpFilters;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

String^ HttpMeteredConnectionFilter::PriorityPropertyName = ref new String(L"meteredConnectionPriority");

HttpMeteredConnectionFilter::HttpMeteredConnectionFilter(IHttpFilter^ innerFilter)
{
    if (innerFilter == nullptr)
    {
        throw ref new Exception(E_INVALIDARG, "innerFilter cannot be null.");
    }
    this->innerFilter = innerFilter;
}

HttpMeteredConnectionFilter::~HttpMeteredConnectionFilter()
{
}

IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ HttpMeteredConnectionFilter::SendRequestAsync(
    HttpRequestMessage^ request)
{
    MeteredConnectionPriority priority = MeteredConnectionPriority::Low;
    if (request->Properties->HasKey(PriorityPropertyName))
    {
        Object^ value = request->Properties->Lookup(PriorityPropertyName);
        IPropertyValue^ propertyValue = static_cast<IPropertyValue^>(value);

        if (propertyValue->Type == PropertyType::Double)
        {
            // Enum values in JS are stored as double values.
            unsigned int intValue = static_cast<unsigned int>(propertyValue->GetDouble());
            priority = static_cast<MeteredConnectionPriority>(intValue) ;
        }
        else
        {
            // A static cast is enough when value comes from C# or C++.
            priority = static_cast<MeteredConnectionPriority>(value);
        }
    }

    if (!ValidatePriority(priority))
    {
        throw ref new Exception(E_ACCESSDENIED, "The request priority is not allowed under the current connection behavior.");
    }

    return innerFilter->SendRequestAsync(request);
}

bool HttpMeteredConnectionFilter::OptIn::get()
{
    return optIn;
}

void HttpMeteredConnectionFilter::OptIn::set(bool value)
{
    optIn = value;
}

bool HttpMeteredConnectionFilter::ValidatePriority(MeteredConnectionPriority priority)
{
    MeteredConnectionBehavior behavior = GetBehavior();

    switch (behavior)
    {
    case MeteredConnectionBehavior::Normal:
        // Under normal behavior all requests are allowed.
        return true;
    case MeteredConnectionBehavior::Conservative:
        // Under conservative behavior all requests except low priority requests are allowed.
        if (priority != MeteredConnectionPriority::Low)
        {
            return true;
        }
        break;
    case MeteredConnectionBehavior::OptIn:
        // Under opt-in only high priority requests are allowed and only if user opted in.
        if (priority == MeteredConnectionPriority::High && OptIn)
        {
            return true;
        }
        break;
    }

    // Everything else is not allowed.
    return false;
}

MeteredConnectionBehavior HttpMeteredConnectionFilter::GetBehavior()
{
    ConnectionProfile^ connectionProfile = NetworkInformation::GetInternetConnectionProfile();

    if (connectionProfile == nullptr)
    {
        return MeteredConnectionBehavior::None;
    }

    ConnectionCost^ connectionCost = connectionProfile->GetConnectionCost();

    if (!connectionCost->Roaming && 
        (connectionCost->NetworkCostType == NetworkCostType::Unrestricted ||
        connectionCost->NetworkCostType == NetworkCostType::Unknown))
    {
        return MeteredConnectionBehavior::Normal;
    }
    else  if (!connectionCost->Roaming && 
        !connectionCost->OverDataLimit &&
        (connectionCost->NetworkCostType == NetworkCostType::Fixed ||
        connectionCost->NetworkCostType == NetworkCostType::Variable))
    {
        return MeteredConnectionBehavior::Conservative;
    }

    return MeteredConnectionBehavior::OptIn;
}
