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
#include "PlugInFilter.h"
#include "PlugInFilter.g.cpp"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

namespace winrt::SDKTemplate::implementation
{
    PlugInFilter::PlugInFilter(IHttpFilter const& innerFilter) : m_innerFilter(innerFilter)
    {
        if (innerFilter == nullptr)
        {
            throw hresult_invalid_argument(L"innerFilter cannot be null.");
        }
    }

    IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> PlugInFilter::SendRequestAsync(HttpRequestMessage request)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();
        auto progress = co_await get_progress_token();

        request.Headers().Append(L"Custom-Header", L"CustomRequestValue");

        IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> operation = m_innerFilter.SendRequestAsync(request);
        operation.Progress([progress](auto&&, auto&& data) mutable
            {
                progress(data);
            });
        HttpResponseMessage response = co_await operation;

        response.Headers().Append(L"Custom-Header", L"CustomResponseValue");
        co_return response;
    }

    void PlugInFilter::Close()
    {
        m_innerFilter.Close();
    }
}


