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
#include "Scenario02_GetStream.h"
#include "Scenario02_GetStream.g.cpp"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;

namespace winrt::SDKTemplate::implementation
{
    Scenario02_GetStream::Scenario02_GetStream()
    {
        InitializeComponent();
    }

    void Scenario02_GetStream::OnNavigatedTo(NavigationEventArgs const&)
    {
        httpClient = Helpers::CreateHttpClientWithCustomHeaders();
    }

    void Scenario02_GetStream::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
    }

    fire_and_forget Scenario02_GetStream::Start_Click(IInspectable const&, RoutedEventArgs const&)
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

        HttpRequestMessage request(HttpMethod::Get(), resourceUri);

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

    IAsyncAction Scenario02_GetStream::CancelableRequestAsync(HttpRequestMessage request)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        // Do not buffer the response.
        HttpRequestResult result = co_await httpClient.TrySendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead);

        if (result.Succeeded())
        {
            OutputField().Text(OutputField().Text() + Helpers::SerializeHeaders(result.ResponseMessage()));

            std::wostringstream responseBody;
            IInputStream responseStream = co_await result.ResponseMessage().Content().ReadAsInputStreamAsync();
            Buffer readBuffer(1000);
            IBuffer resultBuffer;
            do
            {
                resultBuffer = co_await responseStream.ReadAsync(readBuffer, readBuffer.Capacity(), InputStreamOptions::Partial);

                responseBody << L"Bytes read from stream: " << resultBuffer.Length() << L"\n";

                // Use the buffer contents for something. We can't safely display it as a string though, since encodings
                // like UTF-8 and UTF-16 have a variable number of bytes per character and so the last bytes in the buffer
                // may not contain a whole character. Instead, we'll convert the bytes to hex and display the result.
                hstring resultText = CryptographicBuffer::EncodeToHexString(resultBuffer);
                responseBody << std::wstring_view(resultText) << L"\n";
            } while (resultBuffer.Length() > 0);
            responseStream.Close();

            OutputField().Text(OutputField().Text() + responseBody.str());

            rootPage.NotifyUser(L"Completed", NotifyType::StatusMessage);
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result.ExtendedError());
        }
    }

    void Scenario02_GetStream::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }
}
