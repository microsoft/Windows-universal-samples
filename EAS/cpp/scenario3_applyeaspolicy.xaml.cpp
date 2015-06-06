// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario3_ApplyEASPolicy.xaml.h"

using namespace EAS;
using namespace SDKTemplate;

using namespace Platform;
using namespace concurrency;
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

Scenario3_ApplyEASPolicy::Scenario3_ApplyEASPolicy() : rootPage(MainPage::Current)
{
	InitializeComponent();
}

void EAS::Scenario3_ApplyEASPolicy::Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

		if (MinPasswordLengthValue == nullptr || MinPasswordLengthValue->Text == "")
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

		task<EasComplianceResults^> ApplyOp(RequestedPolicy->ApplyAsync());
		ApplyOp.then([=](task<EasComplianceResults^> resultTask)
		{
			try
			{
				auto results = resultTask.get();

				RequireEncryptionResult->Text = results->RequireEncryptionResult.ToString();
				MinPasswordLengthResult->Text = results->MinPasswordLengthResult.ToString();
				DisallowConvenienceLogonResult->Text = results->DisallowConvenienceLogonResult.ToString();
				MinPasswordComplexCharactersResult->Text = results->MinPasswordComplexCharactersResult.ToString();
				PasswordExpirationResult->Text = results->PasswordExpirationResult.ToString();
				PasswordHistoryResult->Text = results->PasswordHistoryResult.ToString();
				MaxPasswordFailedAttemptsResult->Text = results->MaxPasswordFailedAttemptsResult.ToString();
				MaxInactivityTimeLockResult->Text = results->MaxInactivityTimeLockResult.ToString();
			}
			catch (Platform::Exception^ ex)
			{
				// ignore that 0x800704C7 exception  
				if (ex->HResult != 0x800704C7)
				{
					MainPage::Current->NotifyUser(ex->Message, NotifyType::ErrorMessage);
				}
			}
		});
	}

	catch (Platform::Exception^ ex)
	{
		MainPage::Current->NotifyUser(ex->Message, NotifyType::ErrorMessage);
	}
}

void EAS::Scenario3_ApplyEASPolicy::Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

