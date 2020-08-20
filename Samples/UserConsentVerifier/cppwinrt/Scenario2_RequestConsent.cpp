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
#include "Scenario2_RequestConsent.h"
#include "Scenario2_RequestConsent.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Security::Credentials::UI;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_RequestConsent::Scenario2_RequestConsent()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario2_RequestConsent::RequestConsent_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        RequestConsentButton().IsEnabled(false);

        // Request the user's consent using Windows Hello via biometric verification or a PIN.
        hstring message = L"Please confirm your identity to complete this (pretend) in-app purchase.";
        UserConsentVerificationResult consentResult = co_await UserConsentVerifier::RequestVerificationAsync(message);
        switch (consentResult)
        {
        case UserConsentVerificationResult::Verified:
            rootPage.NotifyUser(L"Pretend in-app purchase was successful.", NotifyType::StatusMessage);
            break;

        case UserConsentVerificationResult::DeviceNotPresent:
            rootPage.NotifyUser(L"No PIN or biometric found, please set one up.", NotifyType::ErrorMessage);
            break;

        case UserConsentVerificationResult::Canceled:
            rootPage.NotifyUser(L"User consent verification canceled.", NotifyType::ErrorMessage);
            break;

        default:
            rootPage.NotifyUser(L"User consent verification is currently unavailable.", NotifyType::ErrorMessage);
            break;
        }

        RequestConsentButton().IsEnabled(true);
    }
}
