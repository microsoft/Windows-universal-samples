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
#include "HttpRetryFilter.h"
#include "HttpRetryFilter.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;
using namespace winrt::Windows::Web::Http::Headers;

namespace winrt::HttpFilters::implementation
{
    HttpRetryFilter::HttpRetryFilter(IHttpFilter const& innerFilter) : m_innerFilter(innerFilter)
    {
        if (!m_innerFilter)
        {
            throw hresult_invalid_argument(L"innerFilter cannot be null.");
        }
    }

    uint32_t HttpRetryFilter::GetRetriesValue(HttpRequestMessage const& request)
    {
        // Retries property keeps the number of times the request has been resent.
        return unbox_value_or(request.Properties().TryLookup(RetriesPropertyName), 0U);
    }

    HttpRequestMessage HttpRetryFilter::CopyRequestMessage(HttpRequestMessage const& request)
    {
        HttpRequestMessage copy(request.Method(), request.RequestUri());

        // Copy the headers.
        HttpRequestHeaderCollection headers = copy.Headers();
        for (auto [key, value] : request.Headers())
        {
            if (!headers.TryAppendWithoutValidation(key, value))
            {
                throw hresult_error(E_FAIL, L"Unable to copy headers.");
            }
        }

        // Copy the properties.
        IMap<hstring, IInspectable> properties = copy.Properties();
        for (auto [key, value] : request.Properties())
        {
            properties.Insert(key, value);
        }

        // Increment the retry count.
        properties.Insert(RetriesPropertyName, box_value(GetRetriesValue(request) + 1));

        return copy;
    }

    IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> HttpRetryFilter::SendRequestAsync(HttpRequestMessage request)
    {
        auto lifetime = get_strong();
        auto progress = co_await get_progress_token();

        while (true)
        {
            // Propagate progress. Increment HttpProgress.Retries by the number of times we have resent the request.
            IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> operation = m_innerFilter.SendRequestAsync(request);
            operation.Progress([progress, retries = GetRetriesValue(request)](auto&&, HttpProgress data) mutable
                {
                    data.Retries += retries;
                    progress(data);
                });

            HttpResponseMessage response = co_await operation;

            // Retry requires 503 status code.
            if (response.StatusCode() != HttpStatusCode::ServiceUnavailable)
            {
                co_return response;
            }

            // Retry requires Retry-After header.
            HttpDateOrDeltaHeaderValue retryAfter = response.Headers().RetryAfter();
            if (!retryAfter)
            {
                co_return response;
            }

            TimeSpan delay;
            if (auto delta = retryAfter.Delta())
            {
                delay = delta.Value();
            }
            else if (auto date = retryAfter.Date())
            {
                delay = date.Value() - clock::now();
            }
            else
            {
                co_return response;
            }

            co_await resume_after(delay);

            // Try again with a new request.
            request = CopyRequestMessage(request);
        }
    }
}
