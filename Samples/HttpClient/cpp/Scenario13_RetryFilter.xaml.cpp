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
#include "Scenario13_RetryFilter.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace HttpFilters;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

Scenario13::Scenario13()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario13::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    HttpBaseProtocolFilter^ baseProtocolFilter = ref new HttpBaseProtocolFilter();
    HttpRetryFilter^ retryFilter = ref new HttpRetryFilter(baseProtocolFilter);
    httpClient = ref new HttpClient(retryFilter);
    cancellationTokenSource = cancellation_token_source();
    UpdateAddressField();
}

void Scenario13::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    Page::OnNavigatedFrom(e);
}

void Scenario13::UpdateAddressField()
{
    // Tell the server which type of Retry-After header we would like to receive.
    String^ queryString = "";
    if (RetryAfterSwitch->IsOn)
    {
        queryString += "?retryAfter=deltaSeconds";
    }
    else
    {
        queryString += "?retryAfter=httpDate";
    }

    Helpers::ReplaceQueryString(AddressField, queryString);
}

void Scenario13::RetryAfterSwitch_Toggled(Object^ sender, RoutedEventArgs^ e)
{
    UpdateAddressField();
}

void Scenario13::Start_Click(Object^ sender, RoutedEventArgs^ e)
{
    Uri^ resourceAddress;

    // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
    // valid, absolute URI, we'll notify the user about the incorrect input.
    if (!rootPage->TryGetUri(AddressField->Text, &resourceAddress))
    {
        rootPage->NotifyUser("Invalid URI.", NotifyType::ErrorMessage);
        return;
    }

    Helpers::ScenarioStarted(StartButton, CancelButton, OutputField);
    rootPage->NotifyUser("In progress", NotifyType::StatusMessage);

    // Do an asynchronous GET.  We need to use use_current() with the continuations since the tasks are completed on
    // background threads and we need to run on the UI thread to update the UI.
    create_task(
        httpClient->GetAsync(resourceAddress),
        cancellationTokenSource.get_token()).then([this](HttpResponseMessage^ response)
    {
        return Helpers::DisplayTextResultAsync(response, OutputField, cancellationTokenSource.get_token());
    }, task_continuation_context::use_current()).then([= ](task<HttpResponseMessage^> previousTask)
    {
        try
        {
            // Check if any previous task threw an exception.
            previousTask.get();

            rootPage->NotifyUser("Completed", NotifyType::StatusMessage);
        }
        catch (const task_canceled&)
        {
            rootPage->NotifyUser("Request canceled.", NotifyType::ErrorMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser("Error: " + ex->Message, NotifyType::ErrorMessage);
        }

        Helpers::ScenarioCompleted(StartButton, CancelButton);

    }, task_continuation_context::use_current());
}

void Scenario13::Cancel_Click(Object^ sender, RoutedEventArgs^ e)
{
    cancellationTokenSource.cancel();

    // Re-create the CancellationTokenSource.
    cancellationTokenSource = cancellation_token_source();
}
