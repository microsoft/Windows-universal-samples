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
#include "HttpMeteredConnectionFilter.g.h"

using namespace winrt;

namespace winrt::HttpFilters::implementation
{
    struct HttpMeteredConnectionFilter : HttpMeteredConnectionFilterT<HttpMeteredConnectionFilter>
    {
        HttpMeteredConnectionFilter(Windows::Web::Http::Filters::IHttpFilter const& innerFilter);
        bool OptIn() { return m_optIn; }
        void OptIn(bool value) { m_optIn = value; }
        void Close() { }
        Windows::Foundation::IAsyncOperationWithProgress<Windows::Web::Http::HttpResponseMessage, Windows::Web::Http::HttpProgress> SendRequestAsync(Windows::Web::Http::HttpRequestMessage request);

        static hstring MeteredConnectionPriorityPropertyName() { return MeteredConnectionPriorityPropertyNameLiteral; }

    private:
        enum class MeteredConnectionBehavior
        {
            None = 0, // Used when there is no Internet profile available.
            Normal,
            Conservative,
            OptIn
        };

        static constexpr wchar_t MeteredConnectionPriorityPropertyNameLiteral[] = L"HttpFilters.HttpMeteredConnectionFilter.MeteredConnectionPriority";

        Windows::Web::Http::Filters::IHttpFilter m_innerFilter;
        bool m_optIn;

        static MeteredConnectionBehavior GetBehavior();
        void CheckPriority(MeteredConnectionPriority priority);
    };
}
namespace winrt::HttpFilters::factory_implementation
{
    struct HttpMeteredConnectionFilter : HttpMeteredConnectionFilterT<HttpMeteredConnectionFilter, implementation::HttpMeteredConnectionFilter>
    {
    };
}
