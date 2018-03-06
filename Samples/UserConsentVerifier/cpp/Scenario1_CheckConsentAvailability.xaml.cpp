// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_CheckConsentAvailability.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Security::Credentials::UI;
using namespace Windows::UI::Xaml;

Scenario1_CheckConsentAvailability::Scenario1_CheckConsentAvailability()
{
    InitializeComponent();
}

// Check the availability of Windows Hello authentication through User Consent Verifier.
void Scenario1_CheckConsentAvailability::CheckAvailability_Click(Object^ sender, RoutedEventArgs^ e)
{
    CheckAvailabilityButton->IsEnabled = false;

    create_task(UserConsentVerifier::CheckAvailabilityAsync())
        .then([this](UserConsentVerifierAvailability consentAvailability)
    {
        switch (consentAvailability)
        {
        case UserConsentVerifierAvailability::Available:
            MainPage::Current->NotifyUser("User consent verification is available.", NotifyType::StatusMessage);
            break;

        case UserConsentVerifierAvailability::DeviceNotPresent:
            MainPage::Current->NotifyUser("No PIN or biometric device found, please set one up.", NotifyType::ErrorMessage);
            break;

        default:
            MainPage::Current->NotifyUser("User consent verification is currently unavailable.", NotifyType::ErrorMessage);
            break;
        }
        CheckAvailabilityButton->IsEnabled = true;
    });
}

