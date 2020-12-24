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
#include "Scenario01_GetText.h"
#include "Scenario01_GetText.g.cpp"
#include "SampleConfiguration.h"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

namespace winrt::SDKTemplate::implementation
{
    Scenario01_GetText::Scenario01_GetText()
    {
        InitializeComponent();
    }

    void Scenario01_GetText::OnNavigatedTo(NavigationEventArgs const&)
    {
        // In this scenario we just create an HttpClient instance with default settings. I.e. no custom filters. 
        // For examples on how to use custom filters see other scenarios.
        filter = HttpBaseProtocolFilter();

        // Setting the starting state of the toggle based on the default HTTP version in the OS
        if (filter.MaxVersion() >= HttpVersion::Http20)
        {
            MaxHttpVersionToggle().IsOn(true);
        }
        else
        {
            MaxHttpVersionToggle().IsOn(false);
        }

        httpClient = HttpClient(filter);
        isFilterUsed = false;
    }

    void Scenario01_GetText::OnNavigatedFrom(NavigationEventArgs const&)
    {
        filter.Close();
        httpClient.Close();
    }

    fire_and_forget Scenario01_GetText::Start_Click(IInspectable const&, RoutedEventArgs const&)
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

        if (ReadDefaultRadio().IsChecked().Value())
        {
            filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::Default);
        }
        else if (ReadMostRecentRadio().IsChecked().Value())
        {
            filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::MostRecent);
        }
        else if (ReadOnlyFromCacheRadio().IsChecked().Value())
        {
            filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::OnlyFromCache);
        }
        else if (ReadNoCacheRadio().IsChecked().Value())
        {
            filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::NoCache);
        }

        if (WriteDefaultRadio().IsChecked().Value())
        {
            filter.CacheControl().WriteBehavior(HttpCacheWriteBehavior::Default);
        }
        else if (WriteNoCacheRadio().IsChecked().Value())
        {
            filter.CacheControl().WriteBehavior(HttpCacheWriteBehavior::NoCache);
        }

        // ---------------------------------------------------------------------------
        // WARNING: Only test applications should ignore SSL errors.
        // In real applications, ignoring server certificate errors can lead to MITM
        // attacks (while the connection is secure, the server is not authenticated).
        //
        // The SetupServer script included with this sample creates a server certificate that is self-signed
        // and issued to fabrikam.com, and hence we need to ignore these errors here. 
        // ---------------------------------------------------------------------------
        filter.IgnorableServerCertificateErrors().ReplaceAll({ ChainValidationResult::Untrusted, ChainValidationResult::InvalidName });

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

    IAsyncAction Scenario01_GetText::CancelableRequestAsync(Uri resourceUri)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        isFilterUsed = true;

        HttpRequestResult result = co_await httpClient.TryGetAsync(resourceUri);

        if (result.Succeeded())
        {
            co_await Helpers::DisplayTextResultAsync(result.ResponseMessage(), OutputField());

            rootPage.NotifyUser(
                L"Completed. Response came from " + to_hstring(result.ResponseMessage().Source()) + L". HTTP version used: " + to_hstring(result.ResponseMessage().Version()) + L".",
                NotifyType::StatusMessage);
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result.ExtendedError());
        }
    }


    void Scenario01_GetText::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }

    void Scenario01_GetText::MaxHttpVersionToggle_Toggled(IInspectable const&, RoutedEventArgs const&)
    {
        HttpVersion httpVersion = HttpVersion::Http11;
        if (MaxHttpVersionToggle().IsOn())
        {
            httpVersion = HttpVersion::Http20;
        }

        // In general, for best memory utilization, HttpBaseProtocolFilter and HttpClient objects should be reused for multiple requests.
        // However, if the user has already used the filter object to send at least one request, the HTTP version of the filter cannot be changed.
        // Hence, we need to create new client and filter objects.
        if (isFilterUsed)
        {
            filter.Close();
            httpClient.Close();
            filter = HttpBaseProtocolFilter();
            httpClient = HttpClient(filter);
            isFilterUsed = false;
        }
        filter.MaxVersion(httpVersion);
    }

}
