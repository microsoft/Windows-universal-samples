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
#include "Scenario7_PostStreamWithProgress.xaml.h"
#include "SlowInputStream.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;

Scenario7::Scenario7()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario7::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    httpClient = ref new HttpClient();
    cancellationTokenSource = cancellation_token_source();
    UpdateAddressField();
}

void Scenario7::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    Page::OnNavigatedFrom(e);
}

void Scenario7::UpdateAddressField()
{
    // Tell the server we want a transfer-encoding chunked response.
    String^ queryString = "";
    if (ChunkedResponseToggle->IsOn)
    {
        queryString = "?chunkedResponse=1";
    }

    Helpers::ReplaceQueryString(AddressField, queryString);
}

void Scenario7::ChunkedResponseToggle_Toggled(Object^ sender, RoutedEventArgs^ e)
{
    UpdateAddressField();
}

void Scenario7::Start_Click(Object^ sender, RoutedEventArgs^ e)
{
    Helpers::ScenarioStarted(StartButton, CancelButton, nullptr);
    ResetFields();
    rootPage->NotifyUser("In progress", NotifyType::StatusMessage);

    Uri^ resourceAddress;

    // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
    // valid, absolute URI, we'll notify the user about the incorrect input.
    if (!rootPage->TryGetUri(AddressField->Text, &resourceAddress))
    {
        rootPage->NotifyUser("Invalid URI.", NotifyType::ErrorMessage);
        return;
    }

    const unsigned long long streamLength = 100000;
    HttpStreamContent^ streamContent = ref new HttpStreamContent(ref new SlowInputStream(streamLength));

    // If stream length is unknown, the request is chunked transfer encoded.
    if (!ChunkedRequestToggle->IsOn)
    {
        streamContent->Headers->ContentLength = streamLength;
    }

    IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ operation = httpClient->PostAsync(resourceAddress, streamContent);
    operation->Progress = ref new AsyncOperationProgressHandler<HttpResponseMessage^, HttpProgress>([=](
        IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ asyncInfo,
        HttpProgress progress)
    {
        rootPage->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([=]()
        {
            StageField->Text = progress.Stage.ToString();
            RetriesField->Text = progress.Retries.ToString();
            BytesSentField->Text = progress.BytesSent.ToString();
            BytesReceivedField->Text = progress.BytesReceived.ToString();

            unsigned long long totalBytesToSend = 0;
            if (progress.TotalBytesToSend != nullptr)
            {
                totalBytesToSend = progress.TotalBytesToSend->Value;
                TotalBytesToSendField->Text = totalBytesToSend.ToString();
            }
            else
            {
                TotalBytesToSendField->Text = "unknown";
            }

            unsigned long long totalBytesToReceive = 0;
            if (progress.TotalBytesToReceive != nullptr)
            {
                totalBytesToReceive = progress.TotalBytesToReceive->Value;
                TotalBytesToReceiveField->Text = totalBytesToReceive.ToString();
            }
            else
            {
                TotalBytesToReceiveField->Text = "unknown";
            }

            unsigned long long requestProgress = 0;
            if (progress.Stage == HttpProgressStage::SendingContent && totalBytesToSend > 0)
            {
                requestProgress = progress.BytesSent * 50 / totalBytesToSend;
            }
            else if (progress.Stage == HttpProgressStage::ReceivingContent)
            {
                // Start with 50 percent, request content was already sent.
                requestProgress += 50;

                if (totalBytesToReceive > 0)
                {
                    requestProgress += progress.BytesReceived * 50 / totalBytesToReceive;
                }
            }
            else
            {
                return;
            }

            RequestProgressBar->Value = static_cast<double>(requestProgress);
        }));
    });

    // Do an asynchronous POST.  We need to use use_current() with the continuations since the tasks are completed on
    // background threads and we need to run on the UI thread to update the UI.
    create_task(operation, cancellationTokenSource.get_token()).then([this](task<HttpResponseMessage^> previousTask)
    {
        try
        {
            // Check if any previous task threw an exception.
            previousTask.get();

            rootPage->NotifyUser("Completed", NotifyType::StatusMessage);
        }
        catch (const task_canceled&)
        {
            rootPage->NotifyUser("Request canceled.", NotifyType::ErrorMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser("Error: " + ex->Message, NotifyType::ErrorMessage);
        }

        RequestProgressBar->Value = 100;
        Helpers::ScenarioCompleted(StartButton, CancelButton);

    }, task_continuation_context::use_current());
}

void Scenario7::Cancel_Click(Object^ sender, RoutedEventArgs^ e)
{
    cancellationTokenSource.cancel();

    // Re-create the CancellationTokenSource.
    cancellationTokenSource = cancellation_token_source();
}

void Scenario7::ResetFields()
{
    StageField->Text = "";
    RetriesField->Text = "0";
    BytesSentField->Text = "0";
    TotalBytesToSendField->Text = "0";
    BytesReceivedField->Text = "0";
    TotalBytesToReceiveField->Text = "0";
    RequestProgressBar->Value = 0;
}
