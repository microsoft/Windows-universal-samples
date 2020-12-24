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
#include "Scenario05_PostStream.h"
#include "Scenario05_PostStream.g.cpp"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;

namespace
{
    IAsyncOperation<IRandomAccessStream> GenerateSampleStreamAsync(int contentLength)
    {
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        Buffer buffer(contentLength);
        memset(buffer.data(), '@', contentLength);

        InMemoryRandomAccessStream stream;
        co_await stream.WriteAsync(buffer);

        // Rewind the stream.
        stream.Seek(0);

        co_return stream;
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario05_PostStream::Scenario05_PostStream()
    {
        InitializeComponent();
    }

    void Scenario05_PostStream::OnNavigatedTo(NavigationEventArgs const&)
    {
        httpClient = Helpers::CreateHttpClientWithCustomHeaders();
    }

    void Scenario05_PostStream::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
    }

    fire_and_forget Scenario05_PostStream::Start_Click(IInspectable const&, RoutedEventArgs const&)
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

        Helpers::ScenarioStarted(StartButton(), CancelButton(), OutputField());
        rootPage.NotifyUser(L"In progress", NotifyType::StatusMessage);

        static constexpr int contentLength = 1000;
        IRandomAccessStream stream = co_await GenerateSampleStreamAsync(contentLength);

        // If the stream does not implement the IRandomAccessStream interface,
        // chunked transfer encoding is used during the request.
        HttpStreamContent streamContent(stream);

        HttpRequestMessage request(HttpMethod::Post(), resourceUri);
        request.Content(streamContent);

        // This sample uses a "try" in order to support cancellation.
        // If you don't need to support cancellation, then the "try" is not needed.
        try
        {
            pendingAction = CancelableRequestAsync(request);
            co_await pendingAction;
        }
        catch (hresult_canceled const&)
        {
            rootPage.NotifyUser(L"Request canceled.", NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(StartButton(), CancelButton());
    }

    IAsyncAction Scenario05_PostStream::CancelableRequestAsync(HttpRequestMessage request)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        // Do an asynchronous POST.
        HttpRequestResult result = co_await httpClient.TrySendRequestAsync(request);

        if (result.Succeeded())
        {
            co_await Helpers::DisplayTextResultAsync(result.ResponseMessage(), OutputField());

            rootPage.NotifyUser(L"Completed", NotifyType::StatusMessage);
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result.ExtendedError());
        }
    }

    void Scenario05_PostStream::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }
}
