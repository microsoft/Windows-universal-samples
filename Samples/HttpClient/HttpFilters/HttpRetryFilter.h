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
#include "HttpRetryFilter.g.h"

namespace winrt::HttpFilters::implementation
{
    struct HttpRetryFilter : HttpRetryFilterT<HttpRetryFilter>
    {
        HttpRetryFilter(Windows::Web::Http::Filters::IHttpFilter const& innerFilter);
        void Close() {}
        Windows::Foundation::IAsyncOperationWithProgress<Windows::Web::Http::HttpResponseMessage, Windows::Web::Http::HttpProgress> SendRequestAsync(Windows::Web::Http::HttpRequestMessage request);

    private:
        Windows::Web::Http::Filters::IHttpFilter m_innerFilter;

        static constexpr wchar_t RetriesPropertyName[] = L"HttpFilters.HttpRetryFilter.Retries";
        static uint32_t GetRetriesValue(Windows::Web::Http::HttpRequestMessage const& request);
        static Windows::Web::Http::HttpRequestMessage CopyRequestMessage(Windows::Web::Http::HttpRequestMessage const& message);
    };
}
namespace winrt::HttpFilters::factory_implementation
{
    struct HttpRetryFilter : HttpRetryFilterT<HttpRetryFilter, implementation::HttpRetryFilter>
    {
    };
}
