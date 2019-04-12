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
#include <assert.h>
#include <robuffer.h>
#include "Scenario5_PostStream.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;

Scenario5::Scenario5()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario5::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    httpClient = Helpers::CreateHttpClient();
    cancellationTokenSource = cancellation_token_source();
}

void Scenario5::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    Page::OnNavigatedFrom(e);
}

void Scenario5::Start_Click(Object^ sender, RoutedEventArgs^ e)
{
    Helpers::ScenarioStarted(StartButton, CancelButton, OutputField);
    rootPage->NotifyUser("In progress", NotifyType::StatusMessage);

    Uri^ resourceAddress;

    // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
    // valid, absolute URI, we'll notify the user about the incorrect input.
    if (!rootPage->TryGetUri(AddressField->Text, &resourceAddress))
    {
        rootPage->NotifyUser("Invalid URI.", NotifyType::ErrorMessage);
        return;
    }

    const unsigned int contentLength = 1000;

    // Create a sample stream.
    create_task(GenerateSampleStreamAsync(contentLength, cancellationTokenSource.get_token())).then(
        [=](IRandomAccessStream^ stream)
    {
        // If the stream does not implement the IRandomAccessStream interface,
        // chunked transfer encoding is used during the request.
        HttpStreamContent^ streamContent = ref new HttpStreamContent(stream);

        HttpRequestMessage^ request = ref new HttpRequestMessage(HttpMethod::Post, resourceAddress);
        request->Content = streamContent;

        // Do an asynchronous POST.
        return create_task(httpClient->TrySendRequestAsync(request), cancellationTokenSource.get_token());

        // The above lines could be replaced by the following lines if you know the content-length in advance
        // and chunked transfer encoding will not be used during the request.
        // streamContent->Headers->ContentLength = ref new Box<UINT64>(contentLength);
        // return create_task(
        //     httpClient->TryPostAsync(resourceAddress, streamContent),
        //     cancellationTokenSource.get_token());
    }).then([this](HttpRequestResult^ result)
    {
        if (result->Succeeded)
        {
            return Helpers::DisplayTextResultAsync(result->ResponseMessage, OutputField, cancellationTokenSource.get_token())
                .then([=]()
            {
                rootPage->NotifyUser("Completed", NotifyType::StatusMessage);
            });
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result->ExtendedError);
            return task_from_result();
        }
    }).then([=](task<void> previousTask)
    {
        // This sample uses a "try" in order to support cancellation.
        // If you don't need to support cancellation, then the "try" is not needed.
        try
        {
            // Check if the task was canceled.
            previousTask.get();

        }
        catch (const task_canceled&)
        {
            rootPage->NotifyUser("Request canceled.", NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(StartButton, CancelButton);
    });
}

task<IRandomAccessStream^> Scenario5::GenerateSampleStreamAsync(
    unsigned int size,
    cancellation_token token)
{
    IRandomAccessStream^ stream = ref new InMemoryRandomAccessStream();

    // Create an IAsyncAction-based task so it remains UI-thread-aware.
    return create_task(create_async([] {})).then([=]()
    {
        Buffer^ buffer = ref new Buffer(size);
        buffer->Length = size;

        ComPtr<IBufferByteAccess> bufferByteAccess;
        HRESULT hr = reinterpret_cast<IUnknown*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));
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

        // Generate sample data.
        for (unsigned int i = 0; i < size; i++)
        {
            rawBuffer[i] = '@';
        }

        return create_task(stream->WriteAsync(buffer), token);
    }).then([=](unsigned bytesWritten)
    {
        assert(bytesWritten == size);

        // Rewind stream.
        stream->Seek(0);

        return stream;
    });
}

void Scenario5::Cancel_Click(Object^ sender, RoutedEventArgs^ e)
{
    cancellationTokenSource.cancel();

    // Re-create the CancellationTokenSource.
    cancellationTokenSource = cancellation_token_source();
}
