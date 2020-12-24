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
#include "Scenario03_GetList.h"
#include "Scenario03_GetList.g.cpp"
#include "Helpers.h"

using namespace winrt;
using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;

namespace winrt::SDKTemplate::implementation
{
    Scenario03_GetList::Scenario03_GetList()
    {
        InitializeComponent();
    }

    void Scenario03_GetList::OnNavigatedTo(NavigationEventArgs const&)
    {
        httpClient = Helpers::CreateHttpClientWithCustomHeaders();
    }

    void Scenario03_GetList::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
    }

    fire_and_forget Scenario03_GetList::Start_Click(IInspectable const&, RoutedEventArgs const&)
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
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"Error: " + ex.message() , NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(StartButton(), CancelButton());
    }

    IAsyncAction Scenario03_GetList::CancelableRequestAsync(Uri resourceUri)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        HttpRequestResult result = co_await httpClient.TryGetAsync(resourceUri);

        if (result.Succeeded())
        {
            co_await Helpers::DisplayTextResultAsync(result.ResponseMessage(), OutputField());
            hstring contentString = co_await result.ResponseMessage().Content().ReadAsStringAsync();

            XmlDocument xmlDocument;
            try
            {
                xmlDocument.LoadXml(contentString);
            }
            catch (hresult_error const&)
            {
                // Invalid XML.
            }

            // Create a collection to bind to the view.
            std::vector<hstring> items;
            for (IXmlNode node : xmlDocument.GetElementsByTagName(L"item"))
            {
                items.push_back(node.as<XmlElement>().GetAttribute(L"name"));
            }
            OutputList().ItemsSource(single_threaded_observable_vector(std::move(items)));

            rootPage.NotifyUser(L"Completed", NotifyType::StatusMessage);
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result.ExtendedError());
        }
    }

    void Scenario03_GetList::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }
}
