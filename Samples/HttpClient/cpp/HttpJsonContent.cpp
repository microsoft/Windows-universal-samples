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
#include <ppltasks.h>

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http::Headers;

HttpJsonContent::HttpJsonContent(IJsonValue^ jsonValue)
{
    if (jsonValue == nullptr)
    {
        throw ref new InvalidArgumentException("jsonValue");
    }
    this->jsonValue = jsonValue;
    headers = ref new HttpContentHeaderCollection();
    headers->ContentType = ref new HttpMediaTypeHeaderValue("application/json");
    headers->ContentType->CharSet = "UTF-8";
}

HttpJsonContent::~HttpJsonContent(void)
{
}

IAsyncOperationWithProgress<unsigned long long, unsigned long long>^ HttpJsonContent::BufferAllAsync()
{
    return create_async([=](progress_reporter<unsigned long long> reporter, cancellation_token token)
    {
        unsigned long long length = GetLength();

        // Report progress.
        reporter.report(length);

        // Just return the size in bytes.
        return length;
    });
}

IAsyncOperationWithProgress<IBuffer^, unsigned long long>^ HttpJsonContent::ReadAsBufferAsync()
{
    return create_async([=](progress_reporter<unsigned long long> reporter, cancellation_token token)
    {
        DataWriter^ writer = ref new DataWriter();
        writer->WriteString(jsonValue->Stringify());

        // Make sure that the DataWriter destructor does not free the buffer.
        IBuffer^ buffer = writer->DetachBuffer();

        // Report progress.
        reporter.report(buffer->Length);

        return buffer;
    });
}

IAsyncOperationWithProgress<IInputStream^, unsigned long long>^ HttpJsonContent::ReadAsInputStreamAsync()
{
    return create_async([=](progress_reporter<unsigned long long> reporter, cancellation_token token)
    {
        InMemoryRandomAccessStream^ randomAccessStream = ref new InMemoryRandomAccessStream();
        DataWriter^ writer = ref new DataWriter(randomAccessStream);
        writer->WriteString(jsonValue->Stringify());

        return create_task(writer->StoreAsync(), token).then([=](unsigned int)
        {
            // Make sure that the DataWriter destructor does not close the stream.
            writer->DetachStream();

            // Report progress.
            reporter.report(randomAccessStream->Size);

            return randomAccessStream->GetInputStreamAt(0);
        }, token);
    });
}

IAsyncOperationWithProgress<String^, unsigned long long>^ HttpJsonContent::ReadAsStringAsync()
{
    return create_async([=](progress_reporter<unsigned long long> reporter, cancellation_token token)
    {
        String^ jsonString = jsonValue->Stringify();

        // Report progress (length of string)
        reporter.report(jsonString->Length());

        return jsonString;
    });
}

bool HttpJsonContent::TryComputeLength(unsigned long long* length)
{
    *length = GetLength();
    return true;
}

IAsyncOperationWithProgress<
    unsigned long long,
    unsigned long long>^ HttpJsonContent::WriteToStreamAsync(IOutputStream^ outputStream)
{
    return create_async([=](progress_reporter<unsigned long long> reporter, cancellation_token token)
    {
        DataWriter^ writer = ref new DataWriter(outputStream);
        writer->WriteString(jsonValue->Stringify());

        return create_task(writer->StoreAsync(), token).then([=](unsigned int bytesStored)
        {
            // Make sure that DataWriter destructor does not close the stream.
            writer->DetachStream();

            // Report progress.
            reporter.report(bytesStored);

            return static_cast<unsigned long long>(bytesStored);
        }, token);
    });
}

HttpContentHeaderCollection^ HttpJsonContent::Headers::get()
{
    return headers;
}

unsigned long long HttpJsonContent::GetLength()
{
    DataWriter^ writer = ref new DataWriter();
    writer->WriteString(jsonValue->Stringify());

    IBuffer^ buffer = writer->DetachBuffer();
    return static_cast<unsigned long long>(buffer->Length);
}
