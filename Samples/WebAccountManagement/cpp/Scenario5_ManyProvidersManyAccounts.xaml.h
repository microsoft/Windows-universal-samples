// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario5_ManyProvidersManyAccounts.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// The fifth scenario. Covers keeping and managing multiple active accounts and multiple account provider types.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario5_ManyProvidersManyAccounts sealed
    {
    public:
        Scenario5_ManyProvidersManyAccounts();
    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e) override;
    private:
        MainPage^ rootPage;
        void Button_AddNewAccount_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void Button_ManageSignedInAccounts_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void Button_Reset_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void CreateLocalDataContainers();
        void InitalizeAccountsControlDialog();
        void CleanupAccountsControlDialog();
        void OnAccountCommandsRequested(Windows::UI::ApplicationSettings::AccountsSettingsPane^ sender, Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs^ e);
        void GetProviders();
        void GetAccounts();
        void LoadAccounts();
        void AddWebAccountProviders(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
        void AddWebAccounts(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
        void AddLinksAndDescription(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
        void WebAccountProviderCommandInvoked(Windows::UI::ApplicationSettings::WebAccountProviderCommand^ command);
        void WebAccountCommandInvoked(Windows::UI::ApplicationSettings::WebAccountCommand ^ command, Windows::UI::ApplicationSettings::WebAccountInvokedArgs^ args);
        void PrivacyPolicyInvoked(Windows::UI::Popups::IUICommand^ command);
        void OtherLinkInvoked(Windows::UI::Popups::IUICommand ^ command);
        void AuthenticateWithRequestToken(Windows::Security::Credentials::WebAccountProvider ^ provider, Platform::String^ scope, Platform::String^ clientID);
        void OutputTokenResult(Windows::Security::Authentication::Web::Core::WebTokenRequestResult ^ result);
        void SaveAccount(Windows::Security::Credentials::WebAccount^ account);
        Windows::Foundation::IAsyncAction^ LogoffAndRemoveAccount(Windows::Security::Credentials::WebAccount ^ account);
        void LogoffAndRemoveAllAccounts();

        Platform::String^ MICROSOFT_PROVIDER_ID = "https://login.microsoft.com";
        Platform::String^ CONSUMER_AUTHORITY = "consumers";
        Platform::String^ ORGANIZATION_AUTHORITY = "organizations";
        Platform::String^ MSA_SCOPE_REQUESTED = "wl.basic";
        Platform::String^ MSA_CLIENT_ID = "none";
        Platform::String^ AAD_SCOPE_REQUESTED = "";
        Platform::String^ AAD_CLIENT_ID = "c8542080-6873-47b5-bf71-d56378567ceb";
        Platform::String^ ACCOUNTS_CONTAINER = "accountssettingscontainer";
        Platform::String^ PROVIDER_ID_SUBCONTAINER = "providers";
        Platform::String^ AUTHORITY_SUBCONTAINER = "authorities";
        Platform::String^ APP_SPECIFIC_PROVIDER_ID = "myproviderid";
        Platform::String^ APP_SPECIFIC_PROVIDER_NAME = "App specific provider";
        Platform::Collections::Map<Platform::String^, Windows::Security::Credentials::WebAccountProvider^>^ m_providers;
        Platform::Collections::Map<Platform::String^, Windows::Security::Credentials::WebAccount^>^ m_accounts;
        Windows::Foundation::EventRegistrationToken m_accountCommandsRequestedRegistrationToken;
        enum AccountAction { AddAccount, ManageAccount };
        AccountAction m_selectedAccountAction;
    };
}
