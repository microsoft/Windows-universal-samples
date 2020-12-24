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
#include "Scenario14_MeteredConnectionFilter.h"
#include "Scenario14_MeteredConnectionFilter.g.cpp"
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
    Scenario14_MeteredConnectionFilter::Scenario14_MeteredConnectionFilter()
    {
        InitializeComponent();
    }

    void Scenario14_MeteredConnectionFilter::OnNavigatedTo(NavigationEventArgs const&)
    {
        HttpBaseProtocolFilter baseProtocolFilter;
        meteredConnectionFilter = HttpMeteredConnectionFilter(baseProtocolFilter);
        httpClient = HttpClient(meteredConnectionFilter);
    }

    void Scenario14_MeteredConnectionFilter::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
        meteredConnectionFilter.Close();
    }

    void Scenario14_MeteredConnectionFilter::OptInSwitch_Toggled(IInspectable const&, RoutedEventArgs const&)
    {
        meteredConnectionFilter.OptIn(OptInSwitch().IsOn());
    }

    fire_and_forget Scenario14_MeteredConnectionFilter::Start_Click(IInspectable const&, RoutedEventArgs const&)
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

    IAsyncAction Scenario14_MeteredConnectionFilter::CancelableRequestAsync(Uri resourceUri)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        HttpRequestMessage request(HttpMethod::Get(), resourceUri);

        MeteredConnectionPriority priority = MeteredConnectionPriority::Low;
        if (MediumRadio().IsChecked().Value())
        {
            priority = MeteredConnectionPriority::Medium;
        }
        else if (HighRadio().IsChecked().Value())
        {
            priority = MeteredConnectionPriority::High;
        }
        request.Properties().Insert(HttpMeteredConnectionFilter::MeteredConnectionPriorityPropertyName(), box_value(priority));

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

    void Scenario14_MeteredConnectionFilter::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }
}
