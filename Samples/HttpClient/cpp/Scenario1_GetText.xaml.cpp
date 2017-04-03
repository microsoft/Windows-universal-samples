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
#include "Scenario1_GetText.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Security::Cryptography::Certificates;

Scenario1::Scenario1()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    filter = ref new HttpBaseProtocolFilter();

    // Setting the starting state of the toggle based on the default HTTP version in the OS
    if (filter->MaxVersion == HttpVersion::Http20)
    {
        MaxHttpVersionToggle->IsOn = true;
    }
    else
    {
        MaxHttpVersionToggle->IsOn = false;
    }

    httpClient = ref new HttpClient(filter);
    cancellationTokenSource = cancellation_token_source();
    isFilterUsed = false;
}

void Scenario1::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    Page::OnNavigatedFrom(e);
}

void Scenario1::Start_Click(Object^ sender, RoutedEventArgs^ e)
{
    Uri^ uri;
    // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
    // valid, absolute URI, we'll notify the user about the incorrect input.
    if (!rootPage->TryGetUri(AddressField->Text, &uri))
    {
        rootPage->NotifyUser("Invalid URI.", NotifyType::ErrorMessage);
        return;
    }

    Helpers::ScenarioStarted(StartButton, CancelButton, OutputField);
    rootPage->NotifyUser("In progress", NotifyType::StatusMessage);

    if (ReadDefaultRadio->IsChecked->Value)
    {
        filter->CacheControl->ReadBehavior = HttpCacheReadBehavior::Default;
    }
    else if (ReadMostRecentRadio->IsChecked->Value)
    {
        filter->CacheControl->ReadBehavior = HttpCacheReadBehavior::MostRecent;
    }
    else if (ReadOnlyFromCacheRadio->IsChecked->Value)
    {
        filter->CacheControl->ReadBehavior = HttpCacheReadBehavior::OnlyFromCache;
    }
    else if (ReadNoCacheRadio->IsChecked->Value)
    {
        filter->CacheControl->ReadBehavior = HttpCacheReadBehavior::NoCache;
    }

    if (WriteDefaultRadio->IsChecked->Value)
    {
        filter->CacheControl->WriteBehavior = HttpCacheWriteBehavior::Default;
    }
    else if (WriteNoCacheRadio->IsChecked->Value)
    {
        filter->CacheControl->WriteBehavior = HttpCacheWriteBehavior::NoCache;
    }

    // ---------------------------------------------------------------------------
    // WARNING: Only test applications should ignore SSL errors.
    // In real applications, ignoring server certificate errors can lead to MITM
    // attacks (while the connection is secure, the server is not authenticated).
    //
    // The SetupServer script included with this sample creates a server certificate that is self-signed
    // and issued to fabrikam.com, and hence we need to ignore these errors here. 
    // ---------------------------------------------------------------------------
    filter->IgnorableServerCertificateErrors->Append(ChainValidationResult::Untrusted);
    filter->IgnorableServerCertificateErrors->Append(ChainValidationResult::InvalidName);


    // Do an asynchronous GET. We need to use use_current() with the continuations since the tasks are completed on
    // background threads and we need to run on the UI thread to update the UI.
    create_task(httpClient->GetAsync(uri), cancellationTokenSource.get_token()).then([=](HttpResponseMessage^ response)
    {
        isFilterUsed = true;
        return Helpers::DisplayTextResultAsync(response, OutputField, cancellationTokenSource.get_token());
    }, task_continuation_context::use_current()).then([=](task<HttpResponseMessage^> previousTask)
    {
        try
        {
            // Check if any previous task threw an exception.
            HttpResponseMessage^ response = previousTask.get();

            rootPage->NotifyUser(
                "Completed. Response came from " + response->Source.ToString() +". HTTP version used: " + response->Version.ToString() + ".",
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

        Helpers::ScenarioCompleted(StartButton, CancelButton);
    });
}

void Scenario1::Cancel_Click(Object^ sender, RoutedEventArgs^ e)
{
    cancellationTokenSource.cancel();

    // Re-create the CancellationTokenSource.
    cancellationTokenSource = cancellation_token_source();
}


void Scenario1::MaxHttpVersionToggle_Toggled(Object^ sender, RoutedEventArgs^ e)
{
    HttpVersion httpVersion = HttpVersion::Http11;
    if (MaxHttpVersionToggle->IsOn)
    {
        httpVersion = HttpVersion::Http20;
    }

    // In general, for best memory utilization, HttpBaseProtocolFilter and HttpClient objects should be reused for multiple requests.
    // However, if the user has already used the filter object to send at least one request, the HTTP version of the filter cannot be changed.
    // Hence, we need to create new client and filter objects.
    if (isFilterUsed)
    {
        filter = nullptr; 
        httpClient = nullptr;
        filter = ref new HttpBaseProtocolFilter();
        httpClient = ref new HttpClient(filter);
        isFilterUsed = false;
    }
    filter->MaxVersion = httpVersion;
}
