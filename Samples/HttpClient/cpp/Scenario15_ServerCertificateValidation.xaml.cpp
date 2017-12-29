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
#include "Scenario15_ServerCertificateValidation.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Cryptography::Certificates;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace concurrency;

Scenario15::Scenario15()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario15::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = MainPage::Current;
    // In this scenario we just create an HttpClient instance with default settings. I.e. no custom filters. 
    // For examples on how to use custom filters see other scenarios.
    filter = ref new HttpBaseProtocolFilter();
    httpClient = ref new HttpClient(filter);
    cancellationTokenSource = cancellation_token_source();
}

void Scenario15::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    Page::OnNavigatedFrom(e);
}

void Scenario15::Start_Click(Object^ sender, RoutedEventArgs^ e)
{
    Uri^ resourceAddress;

    // The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
    // valid, absolute URI, we'll notify the user about the incorrect input.
    if (!rootPage->TryGetUri(AddressField->Text, &resourceAddress))
    {
        rootPage->NotifyUser("Invalid URI.", NotifyType::ErrorMessage);
        return;
    }

    Helpers::ScenarioStarted(StartButton, CancelButton, nullptr);
    rootPage->NotifyUser("In progress", NotifyType::StatusMessage);

    EventRegistrationToken eventToken{};
    if (DefaultOSValidation->IsChecked->Value)
    {
        //Do nothing
    }
    else if (DefaultAndCustomValidation->IsChecked->Value)
    {
        // Add event handler to listen to the ServerCustomValidationRequested event. By default, OS validation
        // will be performed before the event is raised.
        eventToken = filter->ServerCustomValidationRequested += ref new TypedEventHandler<
            HttpBaseProtocolFilter^,
            HttpServerCustomValidationRequestedEventArgs^>(
                this,
                &SDKTemplate::Scenario15::MyCustomServerCertificateValidator);
    }
    else if (IgnoreErrorsAndCustomValidation->IsChecked->Value)
    {
        // ---------------------------------------------------------------------------
        // WARNING: Only test applications should ignore SSL errors.
        // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
        // attacks (while the connection is secure, the server is not authenticated).
        // Note that not all certificate validation errors can be ignored.
        // ---------------------------------------------------------------------------
        filter->IgnorableServerCertificateErrors->Append(ChainValidationResult::Untrusted);
        filter->IgnorableServerCertificateErrors->Append(ChainValidationResult::InvalidName);

        // Add event handler to listen to the ServerCustomValidationRequested event.
        // This event handler must implement the desired custom certificate validation logic.
        eventToken = filter->ServerCustomValidationRequested += ref new TypedEventHandler<
            HttpBaseProtocolFilter^,
            HttpServerCustomValidationRequestedEventArgs^>(
                this,
                &SDKTemplate::Scenario15::MyCustomServerCertificateValidator);
    }

    // Here, we turn off writing to and reading from the cache to ensure that each request actually 
    // hits the network and tries to establish an SSL/TLS connection with the server.
    filter->CacheControl->WriteBehavior = HttpCacheWriteBehavior::NoCache;
    filter->CacheControl->ReadBehavior = HttpCacheReadBehavior::NoCache;

    HttpRequestMessage^ request = ref new HttpRequestMessage(HttpMethod::Get, resourceAddress);

    create_task(httpClient->SendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead),
        cancellationTokenSource.get_token()).then(
            [this, eventToken](task<HttpResponseMessage^> previousTask)
    {
        try
        {
            // Check if the previous task threw an exception.
            previousTask.get();
            rootPage->NotifyUser("Success - response received from server. Server certificate was valid.", NotifyType::StatusMessage);
        }
        catch (const task_canceled&)
        {
            rootPage->NotifyUser("Request canceled.", NotifyType::ErrorMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser("Error: " + ex->Message, NotifyType::ErrorMessage);
        }

        // Make sure to unregister the event handler when we are done
        if (eventToken.Value)
        {
            filter->ServerCustomValidationRequested -= eventToken;
        }
       
        Helpers::ScenarioCompleted(StartButton, CancelButton);

    }, task_continuation_context::use_current());

}

void Scenario15::Cancel_Click(Object^ sender, RoutedEventArgs^ e)
{
    cancellationTokenSource.cancel();

    // Re-create the CancellationTokenSource.
    cancellationTokenSource = cancellation_token_source();
}

// This event handler for server certificate validation executes synchronously as part of the SSL/TLS handshake. 
// An app should not perform lengthy operations inside this handler. Otherwise, the remote server may terminate the connection abruptly.
void Scenario15::MyCustomServerCertificateValidator(HttpBaseProtocolFilter^ sender, HttpServerCustomValidationRequestedEventArgs^ args)
{
    // Get the server certificate and certificate chain from the args parameter.
    Certificate^ serverCert = args->ServerCertificate;
    IVectorView<Certificate^>^ serverCertChain = args->ServerIntermediateCertificates;

    // To illustrate the use of async APIs, we use the IsCertificateValidAsync method.
    // In order to call async APIs in this handler, you must first take a deferral and then
    // call Complete on it once you are done with the operation.
    Windows::Foundation::Deferral^ deferral = args->GetDeferral();

    create_task(IsCertificateValidAsync(serverCert).then(
        [this, args, deferral](task<bool> previousTask)
    {

        try
        {
            bool isCertificateValid = previousTask.get();
            if (!isCertificateValid)
            {
                args->Reject();
            }
        }
        catch (Exception^)
        {
            // If we get an exception from IsCertificateValidAsync, we reject the certificate
            // (secure by default).
            args->Reject();
        }
        deferral->Complete();

    }, cancellationTokenSource.get_token()));
}


task<bool> Scenario15::IsCertificateValidAsync(Certificate^ serverCert)
{
    // This is a placeholder call to simulate long-running async calls. Note that this code runs synchronously as part of the SSL/TLS handshake. 
    // Avoid performing lengthy operations here - else, the remote server may terminate the connection abruptly. 
    return task<bool>([serverCert]()->bool
    {
        Sleep(100);

        // In this sample, we compare the hash code of the certificate to a specific integer - this is purely 
        // for illustration purposes and should not be considered as a recommendation for certificate validation.
        const int trustedHashCode = 6044116;
        if (serverCert->GetHashCode().Equals(trustedHashCode))
        {
            // If certificate satisfies the criteria, return true.
            return true;
        }
        else
        {
            // If certificate does not meet the required criteria,return false.
            return false;
        }
    }); 
}


void Scenario15::DefaultOSValidation_Checked(Object^ sender, RoutedEventArgs^ e)
{
    AddressField->Text = "https://www.microsoft.com";
}

void Scenario15::DefaultAndCustomValidation_Checked(Object^ sender, RoutedEventArgs^ e)
{
    AddressField->Text = "https://www.microsoft.com";
}

void Scenario15::IgnoreErrorsAndCustomValidation_Checked(Object^ sender, RoutedEventArgs^ e)
{
    AddressField->Text = "https://localhost/HttpClientSample/default.aspx";
}