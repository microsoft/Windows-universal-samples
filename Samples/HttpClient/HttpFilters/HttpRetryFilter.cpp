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

// Class1.cpp
#include "pch.h"
#include "HttpRetryFilter.h"
#include <agents.h>
#include <winerror.h>
#include <ppltasks.h>
#include <memory>

using namespace Concurrency;
using namespace HttpFilters;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

String^ HttpRetryFilter::RetriesPropertyName = ref new String(L"retries");
long long const HttpRetryFilter::TicksPerSecond = 10000000; //  100-nanoseconds in one second.

HttpRetryFilter::HttpRetryFilter(IHttpFilter^ innerFilter)
{
    if (innerFilter == nullptr)
    {
        throw ref new Exception(E_INVALIDARG, "innerFilter cannot be null.");
    }
    this->innerFilter = innerFilter;
}

HttpRetryFilter::~HttpRetryFilter()
{
}

IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ HttpRetryFilter::SendRequestAsync(
    HttpRequestMessage^ request)
{
    return create_async([=](progress_reporter<HttpProgress> reporter, cancellation_token token)
    {
        // Propagate progress. Increment HttpProgress.Retries by the number of times we have resent the request.
        unsigned int retries = GetRetriesProperty(request);
        IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ operation = innerFilter->SendRequestAsync(request);
        operation->Progress = ref new AsyncOperationProgressHandler<HttpResponseMessage^, HttpProgress>([=](
            IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ asyncInfo,
            HttpProgress progress)
        {
            progress.Retries += retries;
            reporter.report(progress);
        });

        return create_task(operation, token).then([=](task<HttpResponseMessage^> sendRequestTask)
        {
            HttpResponseMessage^ response = sendRequestTask.get();

            // Check if response has a 503 status code.
            if (response->StatusCode != HttpStatusCode::ServiceUnavailable)
            {
                return sendRequestTask;
            }

            // Check if response has a Retry-After header.
            if (!response->Headers->RetryAfter)
            {
                return sendRequestTask;
            }

            long long deltaSeconds;
            if (response->Headers->RetryAfter->Delta)
            {
                TimeSpan delta = response->Headers->RetryAfter->Delta->Value;
                deltaSeconds = delta.Duration / TicksPerSecond;
            }
            else if (response->Headers->RetryAfter->Date)
            {
                DateTime retryDateTime = response->Headers->RetryAfter->Date->Value;
                DateTime currentDateTime = GetCurrentUtcDateTime();

                deltaSeconds = (retryDateTime.UniversalTime - currentDateTime.UniversalTime) / TicksPerSecond;
            }
            else
            {
                return sendRequestTask;
            }

            // If the difference is negative, it means the Retry-After date was in the past.
            if (deltaSeconds < 0)
            {
                deltaSeconds = 0;
            }

            if (deltaSeconds > INT_MAX)
            {
                deltaSeconds = INT_MAX;
            }

            return DelayAsync(static_cast<unsigned int>(deltaSeconds), response).then(
                [=](task<HttpResponseMessage^> delayTask)
            {
                HttpRequestMessage^ requestCopy = CopyRequestMessage(request);
                return create_task(SendRequestAsync(requestCopy), token);
            }, token);
        }, token);
    });
}

Concurrency::task<HttpResponseMessage^> HttpRetryFilter::DelayAsync(
    unsigned int deltaSeconds,
    HttpResponseMessage^ response)
{
    // A task completion event that is set when a timer fires.
    task_completion_event<HttpResponseMessage^> tce;

    // Create a non-repeating timer.
    std::shared_ptr<timer<int>> fire_once(new timer<int>(deltaSeconds * 1000, 0, nullptr, false));

    // Create a call object that sets the completion event after the timer fires.
    std::shared_ptr<call<int>> callback(new call<int>([=](int)
    {
        tce.set(response);
    }));

    // Connect the timer to the callback and start the timer.
    fire_once->link_target(callback.get());
    fire_once->start();

    // Create a task that completes after the completion event is set.
    task<HttpResponseMessage^> event_set(tce);

    // Create a continuation task that cleans up resources and return that continuation task.
    return event_set.then([callback, fire_once](HttpResponseMessage^ response)
    {
        return response;
    });
}

DateTime HttpRetryFilter::GetCurrentUtcDateTime()
{
    SYSTEMTIME currentSystemTime;
    GetSystemTime(&currentSystemTime);

    FILETIME currentFileTime;
    if (!SystemTimeToFileTime(&currentSystemTime, &currentFileTime))
    {
        throw ref new Exception(HRESULT_FROM_WIN32(GetLastError()));
    }

    ULARGE_INTEGER currentLargeInteger = {currentFileTime.dwLowDateTime, currentFileTime.dwHighDateTime};

    DateTime currentDateTime;
    currentDateTime.UniversalTime = currentLargeInteger.QuadPart;

    return currentDateTime;
}

HttpRequestMessage^ HttpRetryFilter::CopyRequestMessage(HttpRequestMessage^ request)
{
    HttpRequestMessage^ copy = ref new HttpRequestMessage(request->Method, request->RequestUri);

    IIterator<IKeyValuePair<String^, String^>^>^ headerIterator = request->Headers->First();
    while (headerIterator->HasCurrent)
    {
        IKeyValuePair<String^, String^>^ header = headerIterator->Current;
        String^ key = header->Key;
        String^ value = header->Value;
        bool result = copy->Headers->TryAppendWithoutValidation(key, value);
        if (!result)
        {
            throw ref new Exception(E_FAIL, "Unable to copy headers.");
        }
        headerIterator->MoveNext();
    }

    IIterator<IKeyValuePair<String^, Object^>^>^ propertiesIterator = request->Properties->First();
    while (propertiesIterator->HasCurrent)
    {
        IKeyValuePair<String^, Object^>^ header = propertiesIterator->Current;
        String^ key = header->Key;
        Object^ value = header->Value;
        copy->Properties->Insert(key, value);
        propertiesIterator->MoveNext();
    }

    // Increment retries property by one.
    copy->Properties->Insert(RetriesPropertyName, GetRetriesProperty(copy) + 1);

    return copy;
}

unsigned int HttpRetryFilter::GetRetriesProperty(Windows::Web::Http::HttpRequestMessage^ request)
{
    // Retries property keeps the number of times the request has been resent.
    unsigned int retries = 0;
    if (request->Properties->HasKey(RetriesPropertyName))
    {
        retries = static_cast<unsigned int>(request->Properties->Lookup(RetriesPropertyName));
    }
    return retries;
}
