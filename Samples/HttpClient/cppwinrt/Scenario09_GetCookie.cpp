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
#include "Scenario09_GetCookie.h"
#include "Scenario09_GetCookie.g.cpp"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

namespace winrt::SDKTemplate::implementation
{
    Scenario09_GetCookie::Scenario09_GetCookie()
    {
        InitializeComponent();
    }

    void Scenario09_GetCookie::OnNavigatedTo(NavigationEventArgs const&)
    {
        httpClient = Helpers::CreateHttpClientWithCustomHeaders();
    }

    void Scenario09_GetCookie::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
    }

    void Scenario09_GetCookie::GetCookies_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Uri resourceUri = Helpers::TryParseHttpUri(AddressField().Text());
        if (!resourceUri)
        {
            rootPage.NotifyUser(L"Invalid URI.", NotifyType::ErrorMessage);
            return;
        }
        try
        {
            HttpBaseProtocolFilter filter;
            HttpCookieCollection cookieCollection = filter.CookieManager().GetCookies(resourceUri);

            std::wostringstream result;

            result << cookieCollection.Size() << L" cookies found.\n";

            for (HttpCookie cookie : cookieCollection)
            {
                result
                    << L"--------------------\n"
                    << L"Name: " << std::wstring_view(cookie.Name()) << L"\n"
                    << L"Domain: " << std::wstring_view(cookie.Domain()) << L"\n"
                    << L"Path: " << std::wstring_view(cookie.Path()) << L"\n"
                    << L"Value: " << std::wstring_view(cookie.Value()) << L"\n";
                if (auto expires = cookie.Expires())
                {
                    result << L"Expires: " << std::wstring_view(Helpers::ToSimpleString(expires.Value())) << L"\n";
                }
                result
                    << L"Secure: " << cookie.Secure() << L"\n"
                    << L"HttpOnly: " << cookie.HttpOnly() << L"\n";
            }
            OutputField().Text(result.str());
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"Error: " + ex.message(), NotifyType::ErrorMessage);
        }
    }

    fire_and_forget Scenario09_GetCookie::SendHttpGetButton_Click(IInspectable const&, RoutedEventArgs const&)
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

        Helpers::ScenarioStarted(SendHttpGetButton(), CancelButton(), OutputField());
        rootPage.NotifyUser(L"In progress", NotifyType::StatusMessage);

        // This sample uses a "try" in order to support cancellation.
        // If you don't need to support cancellation, then the "try" is not needed.
        try
        {
            pendingAction = CancelableRequestAsync(resourceUri);
            co_await pendingAction;
        }
        catch (hresult_canceled const&)
        {
            rootPage.NotifyUser(L"Request canceled.", NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(SendHttpGetButton(), CancelButton());
    }

    IAsyncAction Scenario09_GetCookie::CancelableRequestAsync(Uri resourceUri)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        HttpRequestResult result = co_await httpClient.TryGetAsync(resourceUri);

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

    void Scenario09_GetCookie::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }
}
