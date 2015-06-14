// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_Aad.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
	/// <summary>
	/// The third scenario. Covers getting an AAD account and getting a token using that account.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario3_Aad sealed
	{
	public:
		Scenario3_Aad();
	protected:
		void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e) override;
	private:
		MainPage^ rootPage;
		void Button_SignIn_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
		void Button_GetTokenSilently_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
		void Button_ManageAccounts_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
		void Button_Reset_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
		void InitalizeAccountsControlDialog();
		void CleanupAccountsControlDialog();
		void OnAccountCommandsRequested(Windows::UI::ApplicationSettings::AccountsSettingsPane^ sender, Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs^ e);
		void GetProvidersAndAccounts();
		void AddWebAccountProvider(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
		void AddWebAccount(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
		void AddLinks(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
		void WebAccountProviderCommandInvoked(Windows::UI::ApplicationSettings::WebAccountProviderCommand^ command);
		void WebAccountCommandInvoked(Windows::UI::ApplicationSettings::WebAccountCommand ^ command, Windows::UI::ApplicationSettings::WebAccountInvokedArgs^ args);
		void PrivacyPolicyInvoked(Windows::UI::Popups::IUICommand^ command);
		void OtherLinkInvoked(Windows::UI::Popups::IUICommand ^ command);
		void AuthenticateWithRequestToken(Windows::Security::Credentials::WebAccountProvider ^ provider, Platform::String^ scope, Platform::String^ clientID);
		void AuthenticateWithRequestTokenSilent(Windows::Security::Credentials::WebAccountProvider ^ provider, Platform::String^ scope, Platform::String^ clientID, Windows::Security::Credentials::WebAccount^ account);
		void OutputTokenResult(Windows::Security::Authentication::Web::Core::WebTokenRequestResult ^ result);
		void SaveAccount(Windows::Security::Credentials::WebAccount ^ account);
		void LogoffAndRemoveAccount();

		const Platform::String^ MICROSOFT_PROVIDER_ID = "https://login.microsoft.com";
		const Platform::String^ ORGANIZATION_AUTHORITY = "organizations";
		const Platform::String^ AAD_SCOPE_REQUESTED = "";
		const Platform::String^ AAD_CLIENT_ID = "c8542080-6873-47b5-bf71-d56378567ceb";
		const Platform::String^ STORED_ACCOUNT_ID_KEY = "accountid";
		Windows::Security::Credentials::WebAccountProvider^ m_provider;
		Windows::Security::Credentials::WebAccount^ m_account;
		Windows::Foundation::EventRegistrationToken m_accountCommandsRequestedRegistrationToken;
	};
}