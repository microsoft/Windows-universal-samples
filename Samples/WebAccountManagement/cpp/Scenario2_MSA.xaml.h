// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_Msa.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// The second scenario. Covers getting an MSA account and getting a token using that account.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Msa sealed
    {
    public:
        Scenario2_Msa();
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

        Platform::String^ MICROSOFT_PROVIDER_ID = "https://login.microsoft.com";
        Platform::String^ CONSUMER_AUTHORITY = "consumers";
        Platform::String^ MSA_SCOPE_REQUESTED = "wl.basic";
        Platform::String^ MSA_CLIENT_ID = "none";
        Platform::String^ STORED_ACCOUNT_ID_KEY = "accountid";
        Windows::Security::Credentials::WebAccountProvider^ m_provider;
        Windows::Security::Credentials::WebAccount^ m_account;
        Windows::Foundation::EventRegistrationToken m_accountCommandsRequestedRegistrationToken;
    };
}