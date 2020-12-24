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
#include "Scenario07_PostStreamWithProgress.h"
#include "Scenario07_PostStreamWithProgress.g.cpp"
#include "SampleConfiguration.h"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;

namespace winrt::SDKTemplate::implementation
{
    Scenario07_PostStreamWithProgress::Scenario07_PostStreamWithProgress()
    {
        InitializeComponent();
    }

    void Scenario07_PostStreamWithProgress::OnNavigatedTo(NavigationEventArgs const&)
    {
        httpClient = Helpers::CreateHttpClientWithCustomHeaders();
    }

    void Scenario07_PostStreamWithProgress::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
    }

    void Scenario07_PostStreamWithProgress::UpdateAddressField()
    {
        // Tell the server we want a transfer-encoding chunked response.
        hstring queryString{};
        if (ChunkedResponseToggle().IsOn())
        {
            queryString = L"?chunkedResponse=1";
        }

        Helpers::ReplaceQueryString(AddressField(), queryString);
    }

    void Scenario07_PostStreamWithProgress::ChunkedResponseToggle_Toggled(IInspectable const&, RoutedEventArgs const&)
    {
        UpdateAddressField();
    }

    fire_and_forget Scenario07_PostStreamWithProgress::Start_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
        // valid, absolute URI, we'll notify the user about the incorrect input.
        Uri resourceUri = Helpers::TryParseHttpUri(AddressField().Text());
        if (!resourceUri)
        {
            rootPage.NotifyUser(L"Invalid URI.", NotifyType::ErrorMessage);
            return;
        }

        Helpers::ScenarioStarted(StartButton(), CancelButton(), nullptr);
        ResetFields();
        rootPage.NotifyUser(L"In progress", NotifyType::StatusMessage);

        static constexpr uint32_t streamLength = 100000;
        HttpStreamContent streamContent{ SlowInputStream(streamLength) };

        // If stream length is unknown, the request is chunked transfer encoded.
        if (!ChunkedRequestToggle().IsOn())
        {
            streamContent.Headers().ContentLength(streamLength);
        }

        // This sample uses a "try" in order to support cancellation.
        // If you don't need to support cancellation, then the "try" is not needed.
        try
        {
            pendingAction = CancelableRequestAsync(resourceUri, streamContent);
            co_await pendingAction;
        }
        catch (hresult_canceled const&)
        {
            rootPage.NotifyUser(L"Request canceled.", NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(StartButton(), CancelButton());
    }

    IAsyncAction Scenario07_PostStreamWithProgress::CancelableRequestAsync(Uri resourceUri, IHttpContent content)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        IAsyncOperationWithProgress<HttpRequestResult, HttpProgress> operation = httpClient.TryPostAsync(resourceUri, content);
        operation.Progress({ get_weak(), &Scenario07_PostStreamWithProgress::ProgressHandler });

        HttpRequestResult result = co_await operation;

        if (result.Succeeded())
        {
            rootPage.NotifyUser(L"Completed", NotifyType::StatusMessage);
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result.ExtendedError());
        }
    }

    void Scenario07_PostStreamWithProgress::ResetFields()
    {
        StageField().Text(L"");
        RetriesField().Text(L"0");
        BytesSentField().Text(L"0");
        TotalBytesToSendField().Text(L"0");
        BytesReceivedField().Text(L"0");
        TotalBytesToReceiveField().Text(L"0");
        RequestProgressBar().Value(0);
    }

    fire_and_forget Scenario07_PostStreamWithProgress::ProgressHandler(
        IAsyncOperationWithProgress<HttpRequestResult, HttpProgress> const&,
        HttpProgress progress)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(Dispatcher());

        StageField().Text(to_hstring(progress.Stage));
        RetriesField().Text(to_hstring(progress.Retries));
        BytesSentField().Text(to_hstring(progress.BytesSent));
        BytesReceivedField().Text(to_hstring(progress.BytesReceived));

        uint64_t totalBytesToSend = 0;
        if (progress.TotalBytesToSend != nullptr)
        {
            totalBytesToSend = progress.TotalBytesToSend.Value();
            TotalBytesToSendField().Text(to_hstring(totalBytesToSend));
        }
        else
        {
            TotalBytesToSendField().Text(L"unknown");
        }

        uint64_t totalBytesToReceive = 0;
        if (progress.TotalBytesToReceive != nullptr)
        {
            totalBytesToReceive = progress.TotalBytesToReceive.Value();
            TotalBytesToReceiveField().Text(to_hstring(totalBytesToReceive));
        }
        else
        {
            TotalBytesToReceiveField().Text(L"unknown");
        }

        double requestProgress = 0.0;
        if (progress.Stage == HttpProgressStage::SendingContent && totalBytesToSend > 0)
        {
            requestProgress = progress.BytesSent * 50.0 / totalBytesToSend;
        }
        else if (progress.Stage == HttpProgressStage::ReceivingContent)
        {
            // Start with 50 percent, request content was already sent.
            requestProgress += 50.0;

            if (totalBytesToReceive > 0)
            {
                requestProgress += progress.BytesReceived * 50.0 / totalBytesToReceive;
            }
        }
        else
        {
            return;
        }

        RequestProgressBar().Value(requestProgress);
    }

    void Scenario07_PostStreamWithProgress::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }
}
