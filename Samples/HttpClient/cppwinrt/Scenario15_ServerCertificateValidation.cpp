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
#include "Scenario15_ServerCertificateValidation.h"
#include "Scenario15_ServerCertificateValidation.g.cpp"
#include "Helpers.h"

using namespace std::literals::chrono_literals;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;


namespace winrt::SDKTemplate::implementation
{
    Scenario15_ServerCertificateValidation::Scenario15_ServerCertificateValidation()
    {
        InitializeComponent();
    }

    void Scenario15_ServerCertificateValidation::OnNavigatedTo(NavigationEventArgs const&)
    {
        filter = HttpBaseProtocolFilter();
        httpClient = HttpClient(filter);
    }

    void Scenario15_ServerCertificateValidation::OnNavigatedFrom(NavigationEventArgs const&)
    {
        httpClient.Close();
        filter.Close();
    }

    fire_and_forget Scenario15_ServerCertificateValidation::Start_Click(IInspectable const&, RoutedEventArgs const&)
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

        Helpers::ScenarioStarted(StartButton(), CancelButton(), nullptr);
        rootPage.NotifyUser(L"In progress", NotifyType::StatusMessage);

        event_token customValidationToken{};
        if (DefaultOSValidation().IsChecked().Value())
        {
            // Do nothing
        }
        else if (DefaultAndCustomValidation().IsChecked().Value())
        {
            // Add event handler to listen to the ServerCustomValidationRequested event. By default, OS validation
            // will be performed before the event is raised.
            customValidationToken = filter.ServerCustomValidationRequested({ get_weak(), &Scenario15_ServerCertificateValidation::MyCustomServerCertificateValidator });
        }
        else if (IgnoreErrorsAndCustomValidation().IsChecked().Value())
        {
            // ---------------------------------------------------------------------------
            // WARNING: Only test applications should ignore SSL errors.
            // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
            // attacks (while the connection is secure, the server is not authenticated).
            // Note that not all certificate validation errors can be ignored.
            // ---------------------------------------------------------------------------
            filter.IgnorableServerCertificateErrors().ReplaceAll({ ChainValidationResult::Untrusted, ChainValidationResult::InvalidName });

            // Add event handler to listen to the ServerCustomValidationRequested event.
            // This event handler must implement the desired custom certificate validation logic.
            customValidationToken = filter.ServerCustomValidationRequested({ get_weak(), &Scenario15_ServerCertificateValidation::MyCustomServerCertificateValidator });
        }

        // Here, we turn off writing to and reading from the cache to ensure that each request actually 
        // hits the network and tries to establish an SSL/TLS connection with the server.
        filter.CacheControl().WriteBehavior(HttpCacheWriteBehavior::NoCache);
        filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::NoCache);

        HttpRequestMessage request(HttpMethod::Get(), resourceUri);

        // This sample uses a "try" in order to support cancellation.
        // If you don't need to support cancellation, then the "try" is not needed.
        try
        {
            pendingAction = CancelableRequestAsync(request);
            co_await pendingAction;
        }
        catch (hresult_canceled const&)
        {
            rootPage.NotifyUser(L"Request canceled.", NotifyType::ErrorMessage);
        }

        if (customValidationToken)
        {
            // Unregister the event handler after we are done validating.
            filter.ServerCustomValidationRequested(customValidationToken);
        }

        Helpers::ScenarioCompleted(StartButton(), CancelButton());
    }

    IAsyncAction Scenario15_ServerCertificateValidation::CancelableRequestAsync(HttpRequestMessage request)
    {
        auto lifetime = get_strong();
        auto cancellation = co_await get_cancellation_token();
        cancellation.enable_propagation();

        HttpRequestResult result = co_await httpClient.TrySendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead);

        if (result.Succeeded())
        {
            rootPage.NotifyUser(L"Success - response received from server. Server certificate was valid.", NotifyType::StatusMessage);
        }
        else
        {
            Helpers::DisplayWebError(rootPage, result.ExtendedError());
        }
    }

    void Scenario15_ServerCertificateValidation::Cancel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (pendingAction)
        {
            pendingAction.Cancel();
        }
    }

    // This event handler for server certificate validation executes synchronously as part of the SSL/TLS handshake. 
    // An app should not perform lengthy operations inside this handler. Otherwise, the remote server may terminate the connection abruptly.
    fire_and_forget Scenario15_ServerCertificateValidation::MyCustomServerCertificateValidator(
        HttpBaseProtocolFilter const&, HttpServerCustomValidationRequestedEventArgs args)
    {
        auto lifetime = get_strong();

        // Get the server certificate and certificate chain from the args parameter.
        Certificate serverCert = args.ServerCertificate();
        IVectorView<Certificate> serverCertChain = args.ServerIntermediateCertificates();

        // To illustrate the use of async APIs, we use the IsCertificateValidAsync method.
        // In order to call async APIs in this handler, you must first take a deferral and then
        // release it once you are done with the operation. In C++/WinRT, the deferral
        // is released upon destruction, or you can hasten its release by calling the
        // Complete() method prior to destruction.
        Deferral deferral = args.GetDeferral();

        try
        {
            bool isCertificateValid = co_await IsCertificateValidAsync(serverCert);
            if (!isCertificateValid)
            {
                args.Reject();
            }
        }
        catch (hresult_error const&)
        {
            // If we get an exception from IsCertificateValidAsync, we reject the certificate
            // (secure by default).
            args.Reject();
        }
    }

    IAsyncOperation<bool> Scenario15_ServerCertificateValidation::IsCertificateValidAsync(Certificate serverCert)
    {
        auto lifetime = get_strong();

        // This is a placeholder call to simulate long-running async calls. Note that this code runs synchronously as part of the SSL/TLS handshake. 
        // Avoid performing lengthy operations here - else, the remote server may terminate the connection abruptly. 
        co_await resume_after(100ms);

        // In this sample, we compare the hash code of the certificate to a specific hash - this is purely 
        // for illustration purposes and should not be considered as a recommendation for certificate validation.
        static constexpr std::array<uint8_t, 20> trustedHash{
            0x28, 0xb8, 0x85, 0x04, 0xf6, 0x09, 0xf6, 0x85, 0xf1, 0x68,
            0xb9, 0xa4, 0x9c, 0x8f, 0x0e, 0xc4, 0x9e, 0xad, 0x8b, 0xc2
        };
        if (serverCert.GetHashValue() == array_view(trustedHash))
        {
            // If certificate satisfies the criteria, return true.
            co_return true;
        }
        else
        {
            // If certificate does not meet the required criteria, return false.
            co_return false;
        }
    }

    void Scenario15_ServerCertificateValidation::DefaultOSValidation_Checked(IInspectable const&, RoutedEventArgs const&)
    {
        AddressField().Text(L"https://www.microsoft.com");
    }

    void Scenario15_ServerCertificateValidation::DefaultAndCustomValidation_Checked(IInspectable const&, RoutedEventArgs const&)
    {
        AddressField().Text(L"https://www.microsoft.com");
    }

    void Scenario15_ServerCertificateValidation::IgnoreErrorsAndCustomValidation_Checked(IInspectable const&, RoutedEventArgs const&)
    {
        AddressField().Text(L"https://localhost/HttpClientSample/default.aspx");
    }
}
