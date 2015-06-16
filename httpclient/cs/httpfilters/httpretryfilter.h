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

#include <ppltasks.h>

namespace HttpFilters
{
    public ref class HttpRetryFilter sealed : public Windows::Web::Http::Filters::IHttpFilter
    {
    public:
        HttpRetryFilter(Windows::Web::Http::Filters::IHttpFilter^ innerFilter);
        virtual ~HttpRetryFilter();
        virtual Windows::Foundation::IAsyncOperationWithProgress<
            Windows::Web::Http::HttpResponseMessage^,
            Windows::Web::Http::HttpProgress>^ SendRequestAsync(Windows::Web::Http::HttpRequestMessage^ request);

    private:
        Concurrency::task<Windows::Web::Http::HttpResponseMessage^> DelayAsync(
            unsigned int deltaSeconds, 
            Windows::Web::Http::HttpResponseMessage^ response);
        Windows::Foundation::DateTime GetCurrentUtcDateTime();
        Windows::Web::Http::HttpRequestMessage^ CopyRequestMessage(Windows::Web::Http::HttpRequestMessage^ request);
        unsigned int GetRetriesProperty(Windows::Web::Http::HttpRequestMessage^ request);

        Windows::Web::Http::Filters::IHttpFilter^ innerFilter;
        static Platform::String^ RetriesPropertyName;
        static long long const TicksPerSecond;
    };
}
