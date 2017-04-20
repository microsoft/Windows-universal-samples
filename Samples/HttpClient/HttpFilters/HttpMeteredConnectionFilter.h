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


namespace HttpFilters
{
    public enum class MeteredConnectionPriority 
    {
        Low = 0,
        Medium,
        High
    };

    private enum  class MeteredConnectionBehavior
    {
        None = 0, // Used when there is no Internet profile available.
        Normal,
        Conservative,
        OptIn
    };

    public ref class HttpMeteredConnectionFilter sealed : public Windows::Web::Http::Filters::IHttpFilter
    {
    public:
        HttpMeteredConnectionFilter(Windows::Web::Http::Filters::IHttpFilter^ innerFilter);
        virtual ~HttpMeteredConnectionFilter();
        virtual Windows::Foundation::IAsyncOperationWithProgress<
            Windows::Web::Http::HttpResponseMessage^,
            Windows::Web::Http::HttpProgress>^ SendRequestAsync(Windows::Web::Http::HttpRequestMessage^ request);

        property bool OptIn
        {
            bool get();
            void set(bool value);
        }

        private:
            Windows::Web::Http::Filters::IHttpFilter^ innerFilter;
            bool ValidatePriority(MeteredConnectionPriority priority);
            MeteredConnectionBehavior GetBehavior();

            bool optIn;
            static Platform::String^ PriorityPropertyName;
    };
}
