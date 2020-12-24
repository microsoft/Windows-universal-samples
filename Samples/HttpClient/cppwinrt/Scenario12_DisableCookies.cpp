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
#include "Scenario12_DisableCookies.h"
#include "Scenario12_DisableCookies.g.cpp"
#include "SampleConfiguration.h"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

namespace winrt::SDKTemplate::implementation
{
    Scenario12_DisableCookies::Scenario12_DisableCookies()
    {
        InitializeComponent();
    }

    void Scenario12_DisableCookies::OnNavigatedTo(NavigationEventArgs const&)
    {
        filter = HttpBaseProtocolFilter();
        httpClient = HttpClient(filter);
    }

    void Scenario12_DisableCookies::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
        filter.Close();
    }

    fire_and_forget Scenario12_DisableCookies::SendInitialGetButton_Click(IInspectable const&, RoutedEventArgs const&)
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

        Helpers::ScenarioStarted(SendInitialGetButton(), CancelButton(), OutputField());
        rootPage.NotifyUser(L"In progress", NotifyType::StatusMessage);

        // This sample uses a "try" in order to support cancellation.
        // If you don't need to support cancellation, then the "try" is not needed.
        try
        {
            pendingAction = CancelableInitialRequestAsync(resourceUri);
            co_await pendingAction;
        }
        catch (hresult_canceled const&)
        {
            rootPage.NotifyUser(L"Request canceled.", NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(SendInitialGetButton(), CancelButton());
    }

    IAsyncAction Scenario12_DisableCookies::CancelableInitialRequestAsync(Uri resourceUri)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        HttpRequestResult result = co_await httpClient.TryGetAsync(resourceUri);

        if (result.Succeeded())
        {
            std::wostringstream output;

            HttpCookieCollection cookieCollection = filter.CookieManager().GetCookies(resourceUri);
            output << cookieCollection.Size() << L" cookies found.\n";
            for (HttpCookie cookie : cookieCollection)
            {
                output
                    << L"--------------------\n"
                    << L"Name: " << std::wstring_view(cookie.Name()) << L"\n"
                    << L"Domain: " << std::wstring_view(cookie.Domain()) << L"\n"
                    << L"Path: " << std::wstring_view(cookie.Path()) << L"\n"
                    << L"Value: " << std::wstring_view(cookie.Value()) << L"\n";
                if (auto expires = cookie.Expires())
                {
                    output << L"Expires: " << std::wstring_view(Helpers::ToSimpleString(expires.Value())) << L"\n";
                }
                output
                    << L"Secure: " << cookie.Secure() << L"\n"
                    << L"HttpOnly: " << cookie.HttpOnly() << L"\n";
            }
            OutputField().Text(output.str());

            SendNextRequestButton().IsEnabled(true);
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result.ExtendedError());
        }
    }

    fire_and_forget Scenario12_DisableCookies::SendNextRequestButton_Click(IInspectable const&, RoutedEventArgs const&)
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

        Helpers::ScenarioStarted(SendNextRequestButton(), CancelButton(), OutputField());
        rootPage.NotifyUser(L"In progress", NotifyType::StatusMessage);

        if (CookieUsageBehaviorToggle().IsOn())
        {
            filter.CookieUsageBehavior(HttpCookieUsageBehavior::Default);
        }
        else
        {
            // Disable cookie handling - this will do two things: no cookies are attached to outgoing request, and
            // cookies contained in incoming server responses are not added to the CookieManager of the HttpBaseProtocolFilter.
            filter.CookieUsageBehavior(HttpCookieUsageBehavior::NoCookies);
        }

        // This sample uses a "try" in order to support cancellation.
        // If you don't need to support cancellation, then the "try" is not needed.
        try
        {
            pendingAction = CancelableNextRequestAsync(resourceUri);
            co_await pendingAction;
        }
        catch (hresult_canceled const&)
        {
            rootPage.NotifyUser(L"Request canceled.", NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(SendNextRequestButton(), CancelButton());
    }

    IAsyncAction Scenario12_DisableCookies::CancelableNextRequestAsync(Uri resourceUri)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        HttpRequestResult result = co_await httpClient.TryGetAsync(resourceUri);

        if (result.Succeeded())
        {
            co_await Helpers::DisplayTextResultAsync(result.ResponseMessage(), OutputField());

            rootPage.NotifyUser(L"Completed. Response came from " +
                to_hstring(result.ResponseMessage().Source()) + L".",
                NotifyType::StatusMessage);            
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result.ExtendedError());
        }
    }

    void Scenario12_DisableCookies::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }
}
