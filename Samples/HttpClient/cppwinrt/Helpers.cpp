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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::Web;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;
using namespace winrt::Windows::Web::Http::Headers;

namespace winrt::SDKTemplate::Helpers
{
    hstring ToSimpleString(Windows::Foundation::DateTime dateTime)
    {
        auto time = clock::to_time_t(dateTime);
        char buffer[26];
        ctime_s(buffer, ARRAYSIZE(buffer), &time);
        buffer[25] = 0; // remove the trailing newline
        return to_hstring(buffer);
    }

    IAsyncAction DisplayTextResultAsync(HttpResponseMessage response, TextBox output)
    {
        output.Text(output.Text() + SerializeHeaders(response));

        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        std::wstring text{ co_await response.Content().ReadAsStringAsync() };

        // Change <BR> to newlines.
        for (auto pos = text.find(L"<br>"); pos != std::wstring::npos; pos = text.find(L"<br>", pos + 1))
        {
            text.replace(pos, 4, L"\n");
        }

        output.Text(output.Text() + hstring(text));
    }

    hstring SerializeHeaders(HttpResponseMessage const& response)
    {
        std::wostringstream output;

        auto SerializeHeaderCollection = [&](IIterable<IKeyValuePair<hstring, hstring>> const& headers)
        {
            for (auto [key, value] : headers)
            {
                output << std::wstring_view(key) << L": " << std::wstring_view(value) << L"\n";
            }
        };

        // We cast the StatusCode to an int so we display the numeric value (e.g., "200") rather than the
        // name of the enum (e.g., "OK") which would often be redundant with the ReasonPhrase.

        output << static_cast<int32_t>(response.StatusCode()) << L" " << std::wstring_view(response.ReasonPhrase()) << L"\n";

        SerializeHeaderCollection(response.Headers());
        SerializeHeaderCollection(response.Content().Headers());
        output << L"\n";
        return hstring(output.str());
    }

    void DisplayWebError(MainPage const& rootPage, hresult const& error)
    {
        WebErrorStatus webErrorStatus = WebError::GetStatus(error);
        if (webErrorStatus == WebErrorStatus::Unknown)
        {
            rootPage.NotifyUser(L"Unknown Error: " + hresult_error(error).message(), NotifyType::ErrorMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Web Error: " + to_hstring(webErrorStatus), NotifyType::ErrorMessage);
        }

    }

    HttpClient CreateHttpClientWithCustomHeaders()
    {
        // HttpClient functionality can be extended by plugging multiple filters together and providing
        // HttpClient with the configured filter pipeline.
        IHttpFilter filter = HttpBaseProtocolFilter();
        filter = PlugInFilter(filter); // Adds a custom header to every request and response message.
        HttpClient httpClient(filter);

        // The following line sets a "User-Agent" request header as a default header on the HttpClient instance.
        // Default headers will be sent with every request sent from this HttpClient instance.
        httpClient.DefaultRequestHeaders().UserAgent().Append(HttpProductInfoHeaderValue(L"Sample", L"v8"));

        return httpClient;
    }

    void ScenarioStarted(Button const& startButton, Button const& cancelButton, TextBox const& outputField)
    {
        startButton.IsEnabled(false);
        cancelButton.IsEnabled(true);
        if (outputField != nullptr)
        {
            outputField.Text(L"");
        }
    }

    void ScenarioCompleted(Button const& startButton, Button const& cancelButton)
    {
        startButton.IsEnabled(true);
        cancelButton.IsEnabled(false);
    }

    void ReplaceQueryString(TextBox const& addressField, hstring const& newQueryString)
    {
        hstring resourceAddress = addressField.Text();
        std::wstring_view view(resourceAddress);

        // Remove previous query string.
        auto questionMarkPos = view.find(L'?');
        if (questionMarkPos != std::wstring_view::npos)
        {
            view = view.substr(0, questionMarkPos);
        }

        addressField.Text(view + newQueryString);
    }

    Uri TryParseHttpUri(hstring const& uriString)
    {
        // Note that this app has both "Internet (Client)" and "Home and Work Networking" capabilities set,
        // since the user may provide URIs for servers located on the internet or intranet. If apps only
        // communicate with servers on the internet, only the "Internet (Client)" capability should be set.
        // Similarly if an app is only intended to communicate on the intranet, only the "Home and Work
        // Networking" capability should be set.
        try
        {
            Uri uri(uriString);
            hstring scheme = uri.SchemeName();
            if (scheme == L"http" || scheme == L"https")
            {
                return uri;
            }
        }
        catch (hresult_error const& ex)
        {
            if (ex.code() != E_POINTER && ex.code() != E_INVALIDARG)
            {
                throw;
            }
        }
        return nullptr;
    }
}
