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
#include "Scenario12_DisableCookies.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace HttpFilters;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

Scenario12::Scenario12()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario12::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    filter = ref new HttpBaseProtocolFilter();
    httpClient = ref new HttpClient(filter);
    cancellationTokenSource = cancellation_token_source();
}


void Scenario12::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    Page::OnNavigatedFrom(e);
}

void Scenario12::SendInitialGetButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    Uri^ resourceAddress;

    // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
    // valid, absolute URI, we'll notify the user about the incorrect input.
    if (!rootPage->TryGetUri(AddressField->Text, &resourceAddress))
    {
        rootPage->NotifyUser("Invalid URI.", NotifyType::ErrorMessage);
        return;
    }

    Helpers::ScenarioStarted(SendInitialGetButton, CancelButton, OutputField);
    rootPage->NotifyUser("In progress", NotifyType::StatusMessage);

    create_task(httpClient->GetAsync(resourceAddress), cancellationTokenSource.get_token()).then(
        [=](task<HttpResponseMessage^> previousTask)
    {
        try
        {
            HttpResponseMessage^ response = previousTask.get();

            HttpCookieCollection^ cookieCollection = filter->CookieManager->GetCookies(resourceAddress);
            OutputField->Text = cookieCollection->Size + " cookies found.\r\n";
            for (HttpCookie^ cookie : cookieCollection)
            {
                OutputField->Text += "--------------------\r\n";
                OutputField->Text += "Name: " + cookie->Name + "\r\n";
                OutputField->Text += "Domain: " + cookie->Domain + "\r\n";
                OutputField->Text += "Path: " + cookie->Path + "\r\n";
                OutputField->Text += "Value: " + cookie->Value + "\r\n";

                String^ expirationTimeString = "";
                if (cookie->Expires)
                {
                    DateTimeFormatter^ dateFormatter = ref new DateTimeFormatter("shortdate longtime");
                    expirationTimeString = dateFormatter->Format(cookie->Expires->Value);
                }

                OutputField->Text += "Expires: " + expirationTimeString + "\r\n";
                OutputField->Text += "Secure: " + cookie->Secure + "\r\n";
                OutputField->Text += "HttpOnly: " + cookie->HttpOnly + "\r\n";
            }

            rootPage->NotifyUser("Completed", NotifyType::StatusMessage);
            SendNextRequestButton->IsEnabled = true;
        }
        catch (const task_canceled&)
        {
            rootPage->NotifyUser("Request canceled.", NotifyType::ErrorMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser("Error: " + ex->Message, NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(SendInitialGetButton, CancelButton);

    }, task_continuation_context::use_current());

}

void Scenario12::SendNextRequestButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    Uri^ resourceAddress;

    // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
    // valid, absolute URI, we'll notify the user about the incorrect input.
    if (!rootPage->TryGetUri(AddressField->Text, &resourceAddress))
    {
        rootPage->NotifyUser("Invalid URI.", NotifyType::ErrorMessage);
        return;
    }

    Helpers::ScenarioStarted(SendNextRequestButton, CancelButton, OutputField);
    rootPage->NotifyUser("In progress", NotifyType::StatusMessage);
    if (CookieUsageBehaviorToggle->IsOn)
    {
        filter->CookieUsageBehavior = HttpCookieUsageBehavior::Default;
    }
    else
    {
        // Disable cookie handling - this will do two things: no cookies are attached to outgoing request, and
        // cookies contained in incoming server responses are not added to the CookieManager of the HttpBaseProtocolFilter.
        filter->CookieUsageBehavior = HttpCookieUsageBehavior::NoCookies;
    }


    create_task(httpClient->GetAsync(resourceAddress), cancellationTokenSource.get_token()).then(
        [=](HttpResponseMessage^ response)
    {
        return Helpers::DisplayTextResultAsync(response, OutputField, cancellationTokenSource.get_token());
    }, task_continuation_context::use_current()).then([=](task<HttpResponseMessage^> previousTask)
    {
        try
        {
            HttpResponseMessage^ response = previousTask.get();
            rootPage->NotifyUser(
                "Completed. Response came from " + response->Source.ToString() + ".",
                NotifyType::StatusMessage);
        }
        catch (const task_canceled&)
        {
            rootPage->NotifyUser("Request canceled.", NotifyType::ErrorMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser("Error: " + ex->Message, NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(SendNextRequestButton, CancelButton);

    }, task_continuation_context::use_current());
}

void Scenario12::Cancel_Click(Object^ sender, RoutedEventArgs^ e)
{
    cancellationTokenSource.cancel();

    // Re-create the CancellationTokenSource.
    cancellationTokenSource = cancellation_token_source();
}
