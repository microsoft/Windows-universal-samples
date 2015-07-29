// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_DefaultAccount.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    // Important Note for running this sample:
    // The sample as-is will not be able to get tokens without having it's app manifest being 
    // modified to use the App Identity of a registered Microsoft Store/registered AAD app. 
    //
    // See 'Related Topics' in the README.md for instructions on how to register an app.

    /// <summary>
    /// The first scenario. Covers getting the default sign in account.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_DefaultAccount sealed
    {
    public:
        Scenario1_DefaultAccount();
    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e) override;
    private:
        MainPage^ rootPage;
        void Button_DefaultSignIn_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void Button_GetTokenSilently_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void Button_Reset_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void GetDefaultProvidersAndAccounts();
        void AuthenticateWithRequestToken(Windows::Security::Credentials::WebAccountProvider ^ Provider, Platform::String^ Scope, Platform::String^ ClientID);
        void AuthenticateWithRequestTokenSilent(Windows::Security::Credentials::WebAccountProvider ^ provider, Platform::String^ scope, Platform::String^ clientID, Windows::Security::Credentials::WebAccount^ account);
        void OutputTokenResult(Windows::Security::Authentication::Web::Core::WebTokenRequestResult ^ result);
        void SaveAccount(Windows::Security::Credentials::WebAccount ^ account);
        void LogoffAndRemoveAccount();

        Platform::String^ DEFAULT_ACCOUNT_PROVIDER_ID = "https://login.windows.local";
        Platform::String^ CONSUMER_AUTHORITY = "consumers";
        Platform::String^ ORGANIZATION_AUTHORITY = "organizations";
        Platform::String^ MSA_SCOPE_REQUESTED = "service::wl.basic::DELEGATION";
        Platform::String^ MSA_CLIENT_ID = "none";
        Platform::String^ AAD_SCOPE_REQUESTED = "";
        Platform::String^ AAD_CLIENT_ID = "c8542080-6873-47b5-bf71-d56378567ceb";
        Platform::String^ STORED_ACCOUNT_ID_KEY = "accountid";
        Platform::String^ STORED_ACCOUNT_AUTHORITY = "accountauthority";
        Windows::Security::Credentials::WebAccountProvider^ m_provider;
        Windows::Security::Credentials::WebAccount^ m_account;
        Windows::Foundation::EventRegistrationToken m_accountCommandsRequestedRegistrationToken;
    };
}