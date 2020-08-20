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

#pragma once

#include "Scenario15_ServerCertificateValidation.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario15_ServerCertificateValidation : Scenario15_ServerCertificateValidationT<Scenario15_ServerCertificateValidation>
    {
        Scenario15_ServerCertificateValidation();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget Start_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void Cancel_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void DefaultOSValidation_Checked(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void DefaultAndCustomValidation_Checked(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void IgnoreErrorsAndCustomValidation_Checked(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Web::Http::Filters::HttpBaseProtocolFilter filter{ nullptr };
        Windows::Web::Http::HttpClient httpClient{ nullptr };
        Windows::Foundation::IAsyncAction pendingAction{ nullptr };

        Windows::Foundation::IAsyncAction CancelableRequestAsync(Windows::Web::Http::HttpRequestMessage request);
        fire_and_forget MyCustomServerCertificateValidator(
            Windows::Web::Http::Filters::HttpBaseProtocolFilter const& sender,
            Windows::Web::Http::Filters::HttpServerCustomValidationRequestedEventArgs args);
        Windows::Foundation::IAsyncOperation<bool> IsCertificateValidAsync(Windows::Security::Cryptography::Certificates::Certificate serverCert);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario15_ServerCertificateValidation : Scenario15_ServerCertificateValidationT<Scenario15_ServerCertificateValidation, implementation::Scenario15_ServerCertificateValidation>
    {
    };
}
