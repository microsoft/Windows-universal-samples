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
#include "HttpJsonContent.h"
#include "HttpJsonContent.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Headers;

namespace winrt::SDKTemplate::implementation
{
    HttpJsonContent::HttpJsonContent(IJsonValue const& jsonValue) : m_jsonValue(jsonValue)
    {
        if (jsonValue == nullptr)
        {
            throw hresult_invalid_argument(L"jsonValue");
        }

        HttpMediaTypeHeaderValue headerValue(L"application/json");
        headerValue.CharSet(L"UTF-8");
        m_headers.ContentType(headerValue);
    }

    IAsyncOperationWithProgress<uint64_t, uint64_t> HttpJsonContent::BufferAllAsync()
    {
        auto progress = co_await get_progress_token();
        uint64_t length = GetLength();

        // Report progress.
        progress(length);

        // Just return the size in bytes.
        co_return length;
    }

    IAsyncOperationWithProgress<IBuffer, uint64_t> HttpJsonContent::ReadAsBufferAsync()
    {
        co_return ToBuffer();
    }

    IAsyncOperationWithProgress<IInputStream, uint64_t> HttpJsonContent::ReadAsInputStreamAsync()
    {
        auto lifetime = get_strong();
        auto progress = co_await get_progress_token();

        InMemoryRandomAccessStream randomAccessStream;
        co_await randomAccessStream.WriteAsync(ToBuffer());

        // Report progress.
        progress(randomAccessStream.Size());

        co_return randomAccessStream.GetInputStreamAt(0);
    }

    IAsyncOperationWithProgress<hstring, uint64_t> HttpJsonContent::ReadAsStringAsync()
    {
        auto lifetime = get_strong();
        auto progress = co_await get_progress_token();

        hstring jsonString = m_jsonValue.Stringify();

        // Report progress (length of string)
        progress(jsonString.size());

        co_return jsonString;
    }

    bool HttpJsonContent::TryComputeLength(uint64_t& length)
    {
        length = GetLength();
        return true;
    }

    IAsyncOperationWithProgress<uint64_t, uint64_t> HttpJsonContent::WriteToStreamAsync(IOutputStream outputStream)
    {
        auto lifetime = get_strong();
        auto progress = co_await get_progress_token();

        auto bytesStored = co_await outputStream.WriteAsync(ToBuffer());

        // Report progress.
        progress(bytesStored);

        co_return bytesStored;
    }

    uint64_t HttpJsonContent::GetLength()
    {
        return ToBuffer().Length();
    }

    IBuffer HttpJsonContent::ToBuffer()
    {
        return CryptographicBuffer::ConvertStringToBinary(m_jsonValue.Stringify(), BinaryStringEncoding::Utf8);
    }
}
