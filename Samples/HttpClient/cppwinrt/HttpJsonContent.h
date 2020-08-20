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
#include "HttpJsonContent.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct HttpJsonContent : HttpJsonContentT<HttpJsonContent>
    {
        HttpJsonContent(Windows::Data::Json::IJsonValue const& jsonValue);
        Windows::Web::Http::Headers::HttpContentHeaderCollection Headers() { return m_headers; }
        Windows::Foundation::IAsyncOperationWithProgress<uint64_t, uint64_t> BufferAllAsync();
        Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer, uint64_t> ReadAsBufferAsync();
        Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IInputStream, uint64_t> ReadAsInputStreamAsync();
        Windows::Foundation::IAsyncOperationWithProgress<hstring, uint64_t> ReadAsStringAsync();
        bool TryComputeLength(uint64_t& length);
        Windows::Foundation::IAsyncOperationWithProgress<uint64_t, uint64_t> WriteToStreamAsync(Windows::Storage::Streams::IOutputStream outputStream);
        void Close() {}

    private:
        Windows::Data::Json::IJsonValue const m_jsonValue;
        Windows::Web::Http::Headers::HttpContentHeaderCollection const m_headers;

        Windows::Storage::Streams::IBuffer ToBuffer();
        uint64_t GetLength();
    };
}
namespace winrt::SDKTemplate::factory_implementation
{
    struct HttpJsonContent : HttpJsonContentT<HttpJsonContent, implementation::HttpJsonContent>
    {
    };
}
