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
#include "SlowInputStream.h"
#include "SlowInputStream.g.cpp"

using namespace std::literals::chrono_literals;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage::Streams;

namespace winrt::SDKTemplate::implementation
{
    IAsyncOperationWithProgress<IBuffer, uint32_t> SlowInputStream::ReadAsync(IBuffer buffer, uint32_t count, InputStreamOptions options)
    {
        auto lifetime = get_strong();

        // Introduce a 1 second delay.
        co_await resume_after(1s);

        uint32_t capacity = buffer.Capacity();
        if (count > capacity)
        {
            count = capacity;
        }

        uint32_t available = m_length - m_position;
        if (count > available)
        {
            count = available;
        }

        m_position += count;

        memset(buffer.data(), '@', count);
        buffer.Length(count);

        (co_await get_progress_token())(count);

        co_return buffer;
    }
}
