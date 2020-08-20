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
#include "PlugInFilter.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct PlugInFilter : PlugInFilterT<PlugInFilter>
    {
        PlugInFilter(Windows::Web::Http::Filters::IHttpFilter const& innerFilter);
        Windows::Foundation::IAsyncOperationWithProgress<Windows::Web::Http::HttpResponseMessage, Windows::Web::Http::HttpProgress>
            SendRequestAsync(Windows::Web::Http::HttpRequestMessage request);
        void Close();

    private:
        Windows::Web::Http::Filters::IHttpFilter m_innerFilter;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct PlugInFilter : PlugInFilterT<PlugInFilter, implementation::PlugInFilter>
    {
    };
}
