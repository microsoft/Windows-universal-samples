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
#include "Scenario13_RetryFilter.h"
#include "Scenario13_RetryFilter.g.cpp"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::HttpFilters;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

namespace winrt::SDKTemplate::implementation
{
    Scenario13_RetryFilter::Scenario13_RetryFilter()
    {
        InitializeComponent();
    }

    void Scenario13_RetryFilter::OnNavigatedTo(NavigationEventArgs const&)
    {
        HttpBaseProtocolFilter baseProtocolFilter;
        HttpRetryFilter retryFilter(baseProtocolFilter);
        httpClient = HttpClient(retryFilter);
        UpdateAddressField();
    }

    void Scenario13_RetryFilter::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
    }

    void Scenario13_RetryFilter::UpdateAddressField()
    {
        // Tell the server which type of Retry-After header we would like to receive.
        hstring queryString;
        if (RetryAfterSwitch().IsOn())
        {
            queryString = L"?retryAfter=deltaSeconds";
        }
        else
        {
            queryString = L"?retryAfter=httpDate";
        }

        Helpers::ReplaceQueryString(AddressField(), queryString);
    }

    void Scenario13_RetryFilter::RetryAfterSwitch_Toggled(IInspectable const&, RoutedEventArgs const&)
    {
        UpdateAddressField();
    }

    fire_and_forget Scenario13_RetryFilter::Start_Click(IInspectable const&, RoutedEventArgs const&)
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

        Helpers::ScenarioCompleted(StartButton(), CancelButton());
    }

    IAsyncAction Scenario13_RetryFilter::CancelableRequestAsync(Uri resourceUri)
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

    void Scenario13_RetryFilter::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }
}
