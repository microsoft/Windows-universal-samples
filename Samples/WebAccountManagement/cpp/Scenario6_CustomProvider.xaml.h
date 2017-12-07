// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario6_CustomProvider.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// The sixth scenario. This scenario illustrates adding a custom provider to use with Token Broker APIs.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario6_CustomProvider sealed
    {
    public:
        Scenario6_CustomProvider();
    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e) override;
    private:
        MainPage^ rootPage;
        void Button_SignIn_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void Button_GetTokenSilently_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void Button_ManageAccount_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void Button_Reset_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void Button_AddProvider_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void InitalizeAccountsControlDialog();
        void CleanupAccountsControlDialog();
        void OnAccountCommandsRequested(Windows::UI::ApplicationSettings::AccountsSettingsPane^ sender, Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs^ e);
        void GetProvidersAndAccounts();
        void LoadAccount();
        void AddCustomProvider(Platform::String ^ providerId, Platform::String ^ authority);
        void AddWebAccountProviders(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
        void AddWebAccount(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
        void AddLinksAndDescription(Windows::UI::ApplicationSettings::AccountsSettingsPaneCommandsRequestedEventArgs ^ e);
        void WebAccountProviderCommandInvoked(Windows::UI::ApplicationSettings::WebAccountProviderCommand^ command);
        void WebAccountCommandInvoked(Windows::UI::ApplicationSettings::WebAccountCommand ^ command, Windows::UI::ApplicationSettings::WebAccountInvokedArgs^ args);
        void PrivacyPolicyInvoked(Windows::UI::Popups::IUICommand^ command);
        void OtherLinkInvoked(Windows::UI::Popups::IUICommand ^ command);
        void AuthenticateWithRequestToken(Windows::Security::Credentials::WebAccountProvider ^ provider, Platform::String^ scope, Platform::String^ clientID);
        void AuthenticateWithRequestTokenSilent(Windows::Security::Credentials::WebAccountProvider ^ provider, Platform::String^ scope, Platform::String^ clientID, Windows::Security::Credentials::WebAccount^ account);
        void OutputTokenResult(Windows::Security::Authentication::Web::Core::WebTokenRequestResult ^ result);
        void SaveAccount(Windows::Security::Credentials::WebAccount ^ account);
        void LogoffAndRemoveAccount();
        void RemoveCustomProviders();

        Platform::String^ MICROSOFT_PROVIDER_ID = "https://login.microsoft.com";
        Platform::String^ CONSUMER_AUTHORITY = "consumers";
        Platform::String^ ORGANIZATION_AUTHORITY = "organizations";
        Platform::String^ MSA_SCOPE_REQUESTED = "wl.basic";
        Platform::String^ MSA_CLIENT_ID = "none";
        Platform::String^ AAD_SCOPE_REQUESTED = "";
        Platform::String^ AAD_CLIENT_ID = "c8542080-6873-47b5-bf71-d56378567ceb";
        Platform::String^ STORED_ACCOUNT_ID_KEY = "accountid";
        Platform::String^ STORED_PROVIDER_ID_KEY = "providerid";
        Platform::String^ STORED_AUTHORITY_ID_KEY = "authority";
        Platform::Collections::Map<Platform::String^, Windows::Security::Credentials::WebAccountProvider^>^ m_providers;
        Windows::Security::Credentials::WebAccount^ m_account;
        Windows::Foundation::EventRegistrationToken m_accountCommandsRequestedRegistrationToken;
    };
}