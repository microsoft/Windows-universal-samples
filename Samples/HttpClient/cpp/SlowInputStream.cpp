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
#include <ppltasks.h>
#include <robuffer.h>

using namespace Concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;

SlowInputStream::SlowInputStream(unsigned long long length)
{
    this->length = length;
    position = 0;
}

SlowInputStream::~SlowInputStream(void)
{
}

IAsyncOperationWithProgress<IBuffer^, unsigned int>^ SlowInputStream::ReadAsync(
    IBuffer^ buffer,
    unsigned int count,
    InputStreamOptions)
{
    if (buffer == nullptr)
    {
        throw ref new Exception(E_INVALIDARG, "buffer");
    }

    if (count > buffer->Capacity)
    {
        throw ref new Exception(E_INVALIDARG, "count");
    }

    if (length - position < count)
    {
        count = static_cast<unsigned int>(length - position);
    }

    ComPtr<IBufferByteAccess> bufferByteAccess;
    HRESULT hr = reinterpret_cast<IUnknown*>(buffer) ->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));
    if (FAILED(hr))
    {
        throw ref new Exception(hr);
    }

    byte* rawBuffer;
    hr = bufferByteAccess->Buffer(&rawBuffer);
    if (FAILED(hr))
    {
        throw ref new Exception(hr);
    }

    return create_async([=](progress_reporter<unsigned int> reporter, cancellation_token token)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            rawBuffer[i] = 64;
        }
        buffer->Length = count;

        if (token.is_canceled())
        {
            cancel_current_task();
        }

        // Introduce a 1 second delay.
        Sleep(1000);

        position += count;
        reporter.report(count);

        return buffer;
    });
}
