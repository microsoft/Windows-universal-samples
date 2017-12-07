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

namespace SDKTemplate
{
    public ref class PlugInFilter sealed : public Windows::Web::Http::Filters::IHttpFilter
    {
    public:
        PlugInFilter(Windows::Web::Http::Filters::IHttpFilter^ innerFilter);
        virtual ~PlugInFilter(void);
        virtual Windows::Foundation::IAsyncOperationWithProgress<
            Windows::Web::Http::HttpResponseMessage^,
            Windows::Web::Http::HttpProgress>^ SendRequestAsync(Windows::Web::Http::HttpRequestMessage^ request);

    private:
        Windows::Web::Http::Filters::IHttpFilter^ innerFilter;
    };
}
