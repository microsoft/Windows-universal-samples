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
#include "Scenario1_CheckConsentAvailability.h"
#include "Scenario1_CheckConsentAvailability.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Security::Credentials::UI;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_CheckConsentAvailability::Scenario1_CheckConsentAvailability()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_CheckConsentAvailability::CheckAvailability_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        CheckAvailabilityButton().IsEnabled(false);

        // Check the availability of Windows Hello authentication through User Consent Verifier.
        UserConsentVerifierAvailability consentAvailability = co_await UserConsentVerifier::CheckAvailabilityAsync();
        switch (consentAvailability)
        {
        case UserConsentVerifierAvailability::Available:
            rootPage.NotifyUser(L"User consent verification available!", NotifyType::StatusMessage);
            break;

        case UserConsentVerifierAvailability::DeviceNotPresent:
            rootPage.NotifyUser(L"No PIN or biometric found, please set one up.", NotifyType::ErrorMessage);
            break;

        default:
            rootPage.NotifyUser(L"User consent verification is currently unavailable.", NotifyType::ErrorMessage);
            break;
        }

        CheckAvailabilityButton().IsEnabled(true);
    }
}
