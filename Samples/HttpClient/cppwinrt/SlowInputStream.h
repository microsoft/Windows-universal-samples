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
#include "SlowInputStream.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct SlowInputStream : SlowInputStreamT<SlowInputStream>
    {
        SlowInputStream(uint32_t length) : m_length(length) { }
        Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer, uint32_t>
            ReadAsync(Windows::Storage::Streams::IBuffer buffer, uint32_t count, Windows::Storage::Streams::InputStreamOptions options);
        void Close() { }

    private:
        uint32_t const m_length;
        std::atomic<uint32_t> m_position = 0;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct SlowInputStream : SlowInputStreamT<SlowInputStream, implementation::SlowInputStream>
    {
    };
}
