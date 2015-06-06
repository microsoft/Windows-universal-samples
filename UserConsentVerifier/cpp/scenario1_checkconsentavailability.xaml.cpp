// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_CheckConsentAvailability.xaml.h"

using namespace UserConsentVerifier;
using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Credentials::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_CheckConsentAvailability::Scenario1_CheckConsentAvailability()
{
    InitializeComponent();
}

// Checks the availability of User consent requisition via registered fingerprints.
void Scenario1_CheckConsentAvailability::CheckAvailability_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button^ checkAvailabilityButton = dynamic_cast<Button^>(sender);
	checkAvailabilityButton->IsEnabled = false;

	try
	{
		// Check the availability of User Consent with fingerprints facility
		create_task(Windows::Security::Credentials::UI::UserConsentVerifier::CheckAvailabilityAsync())
			.then([checkAvailabilityButton](UserConsentVerifierAvailability consentAvailability)
		{
			switch (consentAvailability)
			{
			case UserConsentVerifierAvailability::Available:
			{
				MainPage::Current->NotifyUser("User consent requisition facility is available.", NotifyType::StatusMessage);
				break;
			}

			case UserConsentVerifierAvailability::DeviceBusy:
			{
				MainPage::Current->NotifyUser("Biometric device is busy.", NotifyType::ErrorMessage);
				break;
			}

			case UserConsentVerifierAvailability::DeviceNotPresent:
			{
				MainPage::Current->NotifyUser("No biometric device found.", NotifyType::ErrorMessage);
				break;
			}

			case UserConsentVerifierAvailability::DisabledByPolicy:
			{
				MainPage::Current->NotifyUser("Biometrics is disabled by policy.", NotifyType::ErrorMessage);
				break;
			}

			case UserConsentVerifierAvailability::NotConfiguredForUser:
			{
				MainPage::Current->NotifyUser("User has no fingeprints registered.", NotifyType::ErrorMessage);
				break;
			}

			default:
			{
				MainPage::Current->NotifyUser("Consent verification with fingerprints is currently unavailable.", NotifyType::ErrorMessage);
				break;
			}
			}
			checkAvailabilityButton->IsEnabled = true;
		});
	}
	catch (Exception ^ex)
	{
		MainPage::Current->NotifyUser("Checking the availability of Consent feature failed with exception. Operation: CheckAvailabilityAsync, Exception: " + ex->ToString(), NotifyType::ErrorMessage);
		checkAvailabilityButton->IsEnabled = true;
	}
}
