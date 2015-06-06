// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_CheckCompliance.xaml.h"

using namespace EAS;
using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::ExchangeActiveSyncProvisioning;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_CheckCompliance::Scenario2_CheckCompliance() : rootPage(MainPage::Current)
{
	InitializeComponent();
}

void EAS::Scenario2_CheckCompliance::Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	try
	{
		EasClientSecurityPolicy^ RequestedPolicy = ref new EasClientSecurityPolicy;

		if (RequireEncryptionValue == nullptr)
		{
			RequestedPolicy->RequireEncryption = false;
		}
		else
		{
			if (RequireEncryptionValue->SelectedIndex == 1)
			{
				RequestedPolicy->RequireEncryption = true;
			}
			else
			{
				RequestedPolicy->RequireEncryption = false;
			}

		}

		if (RequireEncryptionValue == nullptr || MinPasswordLengthValue->Text == "")
		{
			RequestedPolicy->MinPasswordLength = 0;
		}
		else
		{
			RequestedPolicy->MinPasswordLength = _wtoi(MinPasswordLengthValue->Text->Data());
		}

		if (DisallowConvenienceLogonValue == nullptr)
		{
			RequestedPolicy->DisallowConvenienceLogon = false;
		}
		else
		{
			if (DisallowConvenienceLogonValue->SelectedIndex == 1)
			{
				RequestedPolicy->DisallowConvenienceLogon = true;
			}
			else
			{
				RequestedPolicy->DisallowConvenienceLogon = false;
			}

		}

		if (MinPasswordComplexCharactersValue == nullptr || MinPasswordComplexCharactersValue->Text == "")
		{
			RequestedPolicy->MinPasswordComplexCharacters = 0;
		}
		else
		{
			RequestedPolicy->MinPasswordComplexCharacters = _wtoi(MinPasswordComplexCharactersValue->Text->Data());
		}

		TimeSpan ExpirationDays;
		if (PasswordExpirationValue == nullptr || PasswordExpirationValue->Text == "")
		{
			ExpirationDays.Duration = 0;
			RequestedPolicy->PasswordExpiration = ExpirationDays;
		}
		else
		{
			ExpirationDays.Duration = _wtoi(PasswordExpirationValue->Text->Data()) * 86400000;
			RequestedPolicy->PasswordExpiration = ExpirationDays;
		}

		if (PasswordHistoryValue == nullptr || PasswordHistoryValue->Text == "")
		{
			RequestedPolicy->PasswordHistory = 0;
		}
		else
		{
			RequestedPolicy->PasswordHistory = _wtoi(PasswordHistoryValue->Text->Data());
		}

		if (MaxPasswordFailedAttemptsValue == nullptr || MaxPasswordFailedAttemptsValue->Text == "")
		{
			RequestedPolicy->MaxPasswordFailedAttempts = 0;
		}
		else
		{
			RequestedPolicy->MaxPasswordFailedAttempts = _wtoi(MaxPasswordFailedAttemptsValue->Text->Data());
		}

		TimeSpan Inactiveseconds;
		if (MaxInactivityTimeLockValue == nullptr || MaxInactivityTimeLockValue->Text == "")
		{
			Inactiveseconds.Duration = 0;
			RequestedPolicy->MaxInactivityTimeLock = Inactiveseconds;
		}
		else
		{
			Inactiveseconds.Duration = _wtoi(MaxInactivityTimeLockValue->Text->Data()) * 1000;
			RequestedPolicy->MaxInactivityTimeLock = Inactiveseconds;
		}

		EasComplianceResults^ CheckResult = RequestedPolicy->CheckCompliance();

		RequireEncryptionResult->Text = CheckResult->RequireEncryptionResult.ToString();
		EncryptionProviderTypeResult->Text = CheckResult->EncryptionProviderType.ToString();
		MinPasswordLengthResult->Text = CheckResult->MinPasswordLengthResult.ToString();
		DisallowConvenienceLogonResult->Text = CheckResult->DisallowConvenienceLogonResult.ToString();
		MinPasswordComplexCharactersResult->Text = CheckResult->MinPasswordComplexCharactersResult.ToString();
		PasswordExpirationResult->Text = CheckResult->PasswordExpirationResult.ToString();
		PasswordHistoryResult->Text = CheckResult->PasswordHistoryResult.ToString();
		MaxPasswordFailedAttemptsResult->Text = CheckResult->MaxPasswordFailedAttemptsResult.ToString();
		MaxInactivityTimeLockResult->Text = CheckResult->MaxInactivityTimeLockResult.ToString();
	}

	catch (Platform::Exception^ ex)
	{
		MainPage::Current->NotifyUser(ex->Message, NotifyType::ErrorMessage);
	}
}

void EAS::Scenario2_CheckCompliance::Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	try
	{
		RequireEncryptionValue->SelectedIndex = 0;
		MinPasswordLengthValue->Text = "";
		DisallowConvenienceLogonValue->SelectedIndex = 0;
		MinPasswordComplexCharactersValue->Text = "";
		PasswordExpirationValue->Text = "";
		PasswordHistoryValue->Text = "";
		MaxPasswordFailedAttemptsValue->Text = "";
		MaxInactivityTimeLockValue->Text = "";

		RequireEncryptionResult->Text = "";
		EncryptionProviderTypeResult->Text = "";
		MinPasswordLengthResult->Text = "";
		DisallowConvenienceLogonResult->Text = "";
		MinPasswordComplexCharactersResult->Text = "";
		PasswordExpirationResult->Text = "";
		PasswordHistoryResult->Text = "";
		MaxPasswordFailedAttemptsResult->Text = "";
		MaxInactivityTimeLockResult->Text = "";
	}
	catch (Platform::Exception^ ex)
	{
		MainPage::Current->NotifyUser(ex->Message, NotifyType::ErrorMessage);
	}
}

