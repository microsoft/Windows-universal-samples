// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_RequestConsent.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Security::Credentials::UI;
using namespace Windows::UI::Xaml;

Scenario2_RequestConsent::Scenario2_RequestConsent()
{
    InitializeComponent();
}

// Requests consent from the current user.
void Scenario2_RequestConsent::RequestConsent_Click(Object^ sender, RoutedEventArgs^ e)
{
    RequestConsentButton->IsEnabled = false;

    // Request the user's consent using Windows Hello via biometric verification or a PIN.
    String^ message = L"Please confirm your identity to complete this (pretend) in-app purchase.";
    create_task(Windows::Security::Credentials::UI::UserConsentVerifier::RequestVerificationAsync(message))
        .then([this](UserConsentVerificationResult consentResult)
    {
        switch (consentResult)
        {
        case UserConsentVerificationResult::Verified:
            MainPage::Current->NotifyUser("Pretend in-app purchase was successful.", NotifyType::StatusMessage);
            break;

        case UserConsentVerificationResult::DeviceNotPresent:
            MainPage::Current->NotifyUser("No PIN or biometric device found, please set one up.", NotifyType::ErrorMessage);
            break;

        case UserConsentVerificationResult::Canceled:
            MainPage::Current->NotifyUser("User consent verification canceled.", NotifyType::ErrorMessage);
            break;

        default:
            MainPage::Current->NotifyUser("User consent verification is currently unavailable.", NotifyType::ErrorMessage);
            break;
        }
        RequestConsentButton->IsEnabled = true;
    });
}
