// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_DefaultAccount.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Authentication::Web::Core;
using namespace Windows::Security::Credentials;
using namespace Windows::Storage;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ApplicationSettings;

// The first scenario covers getting the default account.
//
// First you must get the default account provider by passing the id "https://login.windows.local"
// into WebAuthenticationCoreManager::FindAccountProviderAsync.
//
// It's important to note whether this account is MSA or AAD depending on the authority that the
// provider has.
//    MSA: authority == "consumers"
//    AAD: authority == "organizations"
// These will determine what strings you pass into the token request.
//
// Then you can call WebAuthenticationCoreManager::RequestTokenSilentlyAsync or 
// WebAuthenticationCoreManager::RequestTokenAsync to get a token for the user.
//
// The main difference between these APIs is that if the user needs to enter login
// credentials before a token can be returned, RequestTokenAsync will result in 
// a UI popping up while RequestTokenSilentlyAsync will just return a result of
// UserInteractionRequired. 
//
// If you hit UserInteractionRequired, then you will need to call the regular
// RequestTokenAsync.

Scenario1_DefaultAccount::Scenario1_DefaultAccount() : rootPage(MainPage::Current)
{
    InitializeComponent();
	GetDefaultProvidersAndAccounts();
}

void Scenario1_DefaultAccount::OnNavigatedFrom(NavigationEventArgs^ e)
{
	// Clean up any account that may still be logged in
	LogoffAndRemoveAccount();
}

void SDKTemplate::Scenario1_DefaultAccount::Button_DefaultSignIn_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (m_provider == nullptr)
    {
        rootPage->NotifyUser("There are no connected accounts to be used for default sign in.", NotifyType::ErrorMessage);
        return;
    }

	// Since we only asked for the default provider, we need to make sure we pass the right
	// scope and client ids based on whether this is AAD or MSA
	if (m_provider->Authority == CONSUMER_AUTHORITY)
		AuthenticateWithRequestToken(m_provider, (String^)MSA_SCOPE_REQUESTED, (String^)MSA_CLIENT_ID);
	else
		AuthenticateWithRequestToken(m_provider, (String^)AAD_SCOPE_REQUESTED, (String^)AAD_CLIENT_ID);
}

void SDKTemplate::Scenario1_DefaultAccount::Button_GetTokenSilently_Click(Object^ sender, RoutedEventArgs^ e)
{
	// Since we only asked for the default provider, we need to make sure we pass the right
	// scope and client ids based on whether this is AAD or MSA
	if(m_provider->Authority == CONSUMER_AUTHORITY)
		AuthenticateWithRequestTokenSilent(m_account->WebAccountProvider, (String^)MSA_SCOPE_REQUESTED, (String^)MSA_CLIENT_ID, m_account);
	else
		AuthenticateWithRequestTokenSilent(m_account->WebAccountProvider, (String^)AAD_SCOPE_REQUESTED, (String^)AAD_CLIENT_ID, m_account);
}

void SDKTemplate::Scenario1_DefaultAccount::Button_Reset_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Resetting", NotifyType::StatusMessage);

    LogoffAndRemoveAccount();

    rootPage->NotifyUser("Done Resetting", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario1_DefaultAccount::GetDefaultProvidersAndAccounts()
{
    // Make a task to get the Default Provider by it's ID
    auto getDefaultProvider = concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync((String^) DEFAULT_ACCOUNT_PROVIDER_ID));

    // When that task completes, save the default provider found along with it's authority to know if it's MSA or AAD
    getDefaultProvider.then([this](WebAccountProvider^ foundDefaultProvider) {
        if (foundDefaultProvider != nullptr)
        {
            m_provider = foundDefaultProvider;
            ApplicationData::Current->LocalSettings->Values->Insert((String^) STORED_ACCOUNT_AUTHORITY, m_provider->Authority);
        }
        else
        {
            rootPage->NotifyUser("Warning: Default provider not found for id: " + (String^) DEFAULT_ACCOUNT_PROVIDER_ID, NotifyType::StatusMessage);
        }
    })
        // Then check to see if there is a stored account id in local settings, 
        // if so then get that account and save that as well
        .then([this]()
    {
        if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_ACCOUNT_ID_KEY))
        {
            String^ accountID = (String^)ApplicationData::Current->LocalSettings->Values->Lookup((String^) STORED_ACCOUNT_ID_KEY);

            auto getDefaultAccount = concurrency::create_task(WebAuthenticationCoreManager::FindAccountAsync(m_provider, accountID));

            getDefaultAccount.then([this](WebAccount^ foundDefaultAccount)
            {
                if (foundDefaultAccount != nullptr)
                {
                    SaveAccount(foundDefaultAccount);
                }
            });
        }
    });
}

// Create a new WebAccountManager WebTokenRequest based on the Provider, Scope, ClientID and then create a task to send 
// that request asynchronously to WebAccountManager's RequestTokenAsync API.
//
// We've gotten the default provider which will
// 
void SDKTemplate::Scenario1_DefaultAccount::AuthenticateWithRequestToken(WebAccountProvider^ passedProvider, String^ scope, String^ clientID)
{
    rootPage->NotifyUser("Requested " + passedProvider->DisplayName + " from AccountsManager dialog.", NotifyType::StatusMessage);

    WebTokenRequest^ wtr = ref new WebTokenRequest(passedProvider, scope, clientID);
    auto getWebTokenRequestResult = concurrency::create_task(WebAuthenticationCoreManager::RequestTokenAsync(wtr));

    // When our task finishes it will return result of the operation, and if successful it will contain a token
    // and WebAccount. We save the WebAccount as the "active" account.
    getWebTokenRequestResult.then([this](WebTokenRequestResult^ webTokenRequestResult)
    {
        if (webTokenRequestResult->ResponseStatus == WebTokenRequestStatus::Success)
        {
            if (webTokenRequestResult->ResponseData->GetAt(0)->WebAccount != nullptr)
            {
                if (ApplicationData::Current->LocalSettings->Values->HasKey((String^)STORED_ACCOUNT_ID_KEY))
                {
                    ApplicationData::Current->LocalSettings->Values->Remove((String^)STORED_ACCOUNT_ID_KEY);
                }

                WebAccount^ account = webTokenRequestResult->ResponseData->GetAt(0)->WebAccount;
                SaveAccount(account);
            }
        }

        OutputTokenResult(webTokenRequestResult);
    });
}

// Create a new WebAccountManager WebTokenRequest based on the Provider, Scope, ClientID and then create a task to send 
// that request and the account to get the token for asynchronously to WebAccountManager's GetTokenSilentlyAsync API
//
// WebAccountManager's GetTokenSilentlyAsync will then try :
//        (1): Check it's local cache to see if it has a valid token
//        (2): Try to silently request a new token from the MSA service
//        (3): Return a status of UserInteractionRequired if we need the user credentials
//
// Because of WebAccountManager's ability to cache tokens, you should only need to call WebAccountManager when making token
// based requests and not require the ability to store a cached token within your app.
void SDKTemplate::Scenario1_DefaultAccount::AuthenticateWithRequestTokenSilent(WebAccountProvider^ provider, String^ scope, String^ clientID, WebAccount^ account)
{
	rootPage->NotifyUser("Requested " + provider->DisplayName + " from AccountsManager dialog.", NotifyType::StatusMessage);

	WebTokenRequest^ wtr = ref new WebTokenRequest(provider, scope, clientID);
	auto getWebTokenRequestResult = concurrency::create_task(WebAuthenticationCoreManager::GetTokenSilentlyAsync(wtr, account));

	// When our task finishes it will return result of the operation, and if successful it will contain a token
	// and WebAccount. We save the WebAccount as the "active" account.
	getWebTokenRequestResult.then([this](WebTokenRequestResult^ webTokenRequestResult)
	{
		if (webTokenRequestResult->ResponseStatus == WebTokenRequestStatus::Success)
		{
			// Perform an operation with the token you just got
		}

		OutputTokenResult(webTokenRequestResult);
	});
}

// Displays the result of requesting a token asynchronously to the main page
void SDKTemplate::Scenario1_DefaultAccount::OutputTokenResult(WebTokenRequestResult^ result)
{
    if (result->ResponseStatus == WebTokenRequestStatus::Success)
    {
        rootPage->NotifyUser(result->ResponseStatus.ToString() + "!\nUser: " + result->ResponseData->GetAt(0)->WebAccount->Id + "\n    Token returned was: " + result->ResponseData->GetAt(0)->Token, NotifyType::StatusMessage);
    }
    else if (result->ResponseStatus == WebTokenRequestStatus::ProviderError)
    {
		// The account provider has passed us an error and we should handle it.
		rootPage->NotifyUser(result->ResponseStatus.ToString() + " " + result->ResponseError->ErrorCode + ": " + result->ResponseError->ErrorMessage, NotifyType::ErrorMessage);
    }
    else if (result->ResponseStatus == WebTokenRequestStatus::AccountProviderNotAvailable)
    {
		// The account provider is unavailable, this is a temporary error.
		rootPage->NotifyUser(result->ResponseStatus.ToString(), NotifyType::ErrorMessage);
    }
    else if (result->ResponseStatus == WebTokenRequestStatus::UserInteractionRequired)
    {
		// The account provider needs to display a UI, since we called request token silently we should call it
		// with RequestTokenAsync.
		rootPage->NotifyUser(result->ResponseStatus.ToString(), NotifyType::StatusMessage);

		if (m_provider->Authority == CONSUMER_AUTHORITY)
			AuthenticateWithRequestToken(m_account->WebAccountProvider, (String^)MSA_SCOPE_REQUESTED, (String^)MSA_CLIENT_ID);
		else
			AuthenticateWithRequestToken(m_account->WebAccountProvider, (String^)AAD_SCOPE_REQUESTED, (String^)AAD_CLIENT_ID);
    }
    else if (result->ResponseStatus == WebTokenRequestStatus::UserCancel)
    {
		// The user cancelled the sign in process for the account provider, handle
		// that how you need to.
		rootPage->NotifyUser(result->ResponseStatus.ToString(), NotifyType::StatusMessage);
    }
    else
    {
		// An unexpected error was encountered
        rootPage->NotifyUser(result->ResponseStatus.ToString() + " " + result->ResponseError->ErrorCode + ": " + result->ResponseError->ErrorMessage, NotifyType::ErrorMessage);
    }
}

// Saves the AccountId in LocalSettings and keeps an instance
// of the WebAccount saved
void SDKTemplate::Scenario1_DefaultAccount::SaveAccount(WebAccount^ account)
{
    ApplicationData::Current->LocalSettings->Values->Insert((String^) STORED_ACCOUNT_ID_KEY, account);

    m_account = account;

	//Update the UI
	button_SignIn->IsEnabled = false;
	button_GetTokenSilently->IsEnabled = true;
	textblock_SignedInStatus->Text = "Signed in with:";
	textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Green);
	listview_SignedInAccounts->Items->Append(account->Id);
}

// Signs out the account using the SignOutAsync WebAccountManager API
// and removes our saved AccountId as it won't be valid when SignOutAsync finishes.
void SDKTemplate::Scenario1_DefaultAccount::LogoffAndRemoveAccount()
{
    if (m_account != nullptr)
    {
        //concurrency::create_task(account->SignOutAsync((String^) MSA_SCOPE_REQUESTED));
    }

    if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_ACCOUNT_ID_KEY))
    {
        ApplicationData::Current->LocalSettings->Values->Remove((String^) STORED_ACCOUNT_ID_KEY);
    }

	//Update the UI
	button_SignIn->IsEnabled = true;
	button_GetTokenSilently->IsEnabled = false;
	textblock_SignedInStatus->Text = "Not signed in.";
	textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
	listview_SignedInAccounts->Items->Clear();
}