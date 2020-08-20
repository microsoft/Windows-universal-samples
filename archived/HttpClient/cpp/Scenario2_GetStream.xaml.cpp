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
#include "Scenario2_GetStream.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;

Scenario2::Scenario2()
{
    InitializeComponent();

    this->readBuffer = ref new Buffer(1000);
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    httpClient = Helpers::CreateHttpClient();
    cancellationTokenSource = cancellation_token_source();
}

void Scenario2::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    Page::OnNavigatedFrom(e);
}

void Scenario2::Start_Click(Object^ sender, RoutedEventArgs^ e)
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
    HttpRequestMessage^ request = ref new HttpRequestMessage(HttpMethod::Get, resourceAddress);

    // Do an asynchronous GET, do not buffer the response.
    create_task(
        httpClient->TrySendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead),
        cancellationTokenSource.get_token()).then([this](HttpRequestResult^ result)
    {
        if (result->Succeeded)
        {
            HttpResponseMessage^ response = result->ResponseMessage;
            OutputField->Text += Helpers::SerializeHeaders(response);
            return create_task(response->Content->ReadAsInputStreamAsync(), cancellationTokenSource.get_token())
                .then([=](IInputStream^ stream)
            {
                return Scenario2ReadData(stream);
            }).then([=](task<IBuffer^> /* buffer */)
            {
                rootPage->NotifyUser("Completed", NotifyType::StatusMessage);
            });
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result->ExtendedError);
        }

    }).then([this](task<void> previousTask)
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

task<IBuffer^> Scenario2::Scenario2ReadData(IInputStream^ stream)
{
    // Do an asynchronous read.
    return create_task(
        stream->ReadAsync(readBuffer, readBuffer->Capacity, InputStreamOptions::Partial),
        cancellationTokenSource.get_token()).then([=](task<IBuffer^> readTask)
    {
        IBuffer^ buffer = readTask.get();

        OutputField->Text += "Bytes read from stream: " + buffer->Length + "\n"; 

        // Use the buffer contents for something.  We can't safely display it as a string though, since encodings
        // like UTF-8 and UTF-16 have a variable number of bytes per character and so the last bytes in the buffer
        // may not contain a whole character.   Instead, we'll convert the bytes to hex and display the result.
        String^ responseBodyAsText = CryptographicBuffer::EncodeToHexString(buffer);
        OutputField->Text += responseBodyAsText + "\n";

        // Continue reading until the response is complete.  When done, return previousTask that is complete.
        return buffer->Length ? Scenario2ReadData(stream) : readTask;
    });
}

void Scenario2::Cancel_Click(Object^ sender, RoutedEventArgs^ e)
{
    cancellationTokenSource.cancel();

    // Re-create the CancellationTokenSource.
    cancellationTokenSource = cancellation_token_source();
}
