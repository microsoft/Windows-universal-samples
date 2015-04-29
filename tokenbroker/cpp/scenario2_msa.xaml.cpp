// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_Msa.xaml.h"

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
using namespace Windows::ApplicationModel::Background;
using namespace Windows::UI::Xaml::Media::Imaging;

Scenario2_Msa::Scenario2_Msa() : rootPage(MainPage::Current)
{
    InitializeComponent();
    InitalizeAccountsControlDialog();
}

void Scenario2_Msa::OnNavigatedFrom(NavigationEventArgs^ e)
{
    LogoffAndRemoveAccount();
    CleanupAccountsControlDialog();
}

void SDKTemplate::Scenario2_Msa::Button_SignIn_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Inside OnAccountCommandsRequested we will determine that we need to show accounts
    // and providers eligible for sign in because there is no signed in account
    AccountsSettingsPane::Show();
}

void SDKTemplate::Scenario2_Msa::Button_GetTokenSilently_Click(Object^ sender, RoutedEventArgs^ e)
{
    AuthenticateWithRequestTokenSilent(m_provider, (String^)MSA_SCOPE_REQUESTED, (String^)MSA_CLIENT_ID, m_account);
}

void SDKTemplate::Scenario2_Msa::Button_ManageAccounts_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Inside OnAccountCommandsRequested we will determine that we need to show the
    // active account since are signed into an account right now
    AccountsSettingsPane::Show();
}

void SDKTemplate::Scenario2_Msa::Button_Reset_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Resetting", NotifyType::StatusMessage);

    LogoffAndRemoveAccount();

    rootPage->NotifyUser("Done Resetting", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario2_Msa::InitalizeAccountsControlDialog() 
{
    // Add to the event AccountCommandsRequested to load our list of providers into the AccountSettingsPane
    m_accountCommandsRequestedRegistrationToken = AccountsSettingsPane::GetForCurrentView()->AccountCommandsRequested::add(
        ref new TypedEventHandler<AccountsSettingsPane^, AccountsSettingsPaneCommandsRequestedEventArgs^>(
            this,
            &SDKTemplate::Scenario2_Msa::OnAccountCommandsRequested)
    );

    // Populate our list of providers with the MSA provider, and attempt to find any accounts saved.
    GetProvidersAndAccounts();
}

void SDKTemplate::Scenario2_Msa::CleanupAccountsControlDialog() 
{
    // Remove the event handler from the AccountCommandsRequested
    AccountsSettingsPane::GetForCurrentView()->AccountCommandsRequested::remove(m_accountCommandsRequestedRegistrationToken);
}

void SDKTemplate::Scenario2_Msa::OnAccountCommandsRequested(
    AccountsSettingsPane^ sender,
    AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{    
    // This scenario only supports a single account at one time.
    // If there already is an account, we do not include a provider in the list
    if (ApplicationData::Current->LocalSettings->Values->HasKey((String^)STORED_ACCOUNT_ID_KEY))
    {
        AddWebAccount(e);
    }
    else
    {
        AddWebAccountProvider(e);
    }

    AddLinks(e);
}

void SDKTemplate::Scenario2_Msa::GetProvidersAndAccounts() 
{
    // Make a task to get the MSAProvider by it's ID
    auto getMsaProvider = concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync((String^) MICROSOFT_PROVIDER_ID,(String^) CONSUMER_AUTHORITY));

    // When that task completes, save the MSA provider found
    getMsaProvider.then([this](WebAccountProvider^ foundMsaProvider){
        if (foundMsaProvider != nullptr)
        {
            m_provider = foundMsaProvider;
        }
        else
        {
            rootPage->NotifyUser("Provider not found for id: " + (String^) MICROSOFT_PROVIDER_ID + " and authority: " + (String^) CONSUMER_AUTHORITY, NotifyType::StatusMessage);
        }
    })
    // Then check to see if there is a stored account id in local settings, 
    // if so then get that account and save that as well
    .then([this]()
    {
        if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_ACCOUNT_ID_KEY))
        {
            String^ accountID = (String^)ApplicationData::Current->LocalSettings->Values->Lookup((String^) STORED_ACCOUNT_ID_KEY);

            auto getMsaAccount = concurrency::create_task(WebAuthenticationCoreManager::FindAccountAsync(m_provider, accountID));

            getMsaAccount.then([this](WebAccount^ foundMsaAccount)
            {
                if (foundMsaAccount != nullptr)
                {
                    SaveAccount(foundMsaAccount);
                }
            });
        }
    });
}

//Creates a provider command object for our provider and adds it to the AccountsControl list of provider commands
void SDKTemplate::Scenario2_Msa::AddWebAccountProvider(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    e->HeaderText = "Describe what adding an account to your application will do for the user.";

    // Create a new WebAccountProviderCommandInvokedHandler for the event fired when a user clicks on a provider in the AccountSettingsPane
    WebAccountProviderCommandInvokedHandler^ handler = ref new WebAccountProviderCommandInvokedHandler(this, &SDKTemplate::Scenario2_Msa::WebAccountProviderCommandInvoked);

    // Make a new command based on the MSA provider and the click handler we just created
    WebAccountProviderCommand^ msaProviderCommand = ref new WebAccountProviderCommand(SDKTemplate::Scenario2_Msa::m_provider, handler);

    // Append that command to the WebAccountProviderCommands list for the AccountSettingsPane
    e->WebAccountProviderCommands->Append(msaProviderCommand);
}

//Creates a web account command object for our account, if we have one saved, and adds it to the AccountsControl list of web account commands
void SDKTemplate::Scenario2_Msa::AddWebAccount(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    e->HeaderText = "Describe what removing an account to your application will do.";

    // Create a new WebAccountCommandInvokedHandler for the event fired when a user clicks on an account in the AccountSettingsPane
    WebAccountCommandInvokedHandler^ handler = ref new WebAccountCommandInvokedHandler(this, &SDKTemplate::Scenario2_Msa::WebAccountCommandInvoked);

    // Make a new command based on the MSA account and the click handler we just created
    WebAccountCommand^ msaAccountCommand = ref new WebAccountCommand(m_account, handler, SupportedWebAccountActions::Remove);

    // Append that command to the WebAccountProviderCommands list for the AccountSettingsPane
    e->WebAccountCommands->Append(msaAccountCommand);
}

// Add optional descriptive text and buttons/commands to the AccountControl
void SDKTemplate::Scenario2_Msa::AddLinks(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    // You can add links such as privacy policy, help, general account settings
    e->Commands->Append(ref new SettingsCommand("privacypolicy", "Privacy Policy", ref new UICommandInvokedHandler(this, &SDKTemplate::Scenario2_Msa::PrivacyPolicyInvoked)));
    e->Commands->Append(ref new SettingsCommand("otherlink", "Other Link", ref new UICommandInvokedHandler(this, &SDKTemplate::Scenario2_Msa::OtherLinkInvoked)));
}

// The function that will be called from AccountSettingsPane with the WebAccountProvider
// that was clicked by the user
void SDKTemplate::Scenario2_Msa::WebAccountProviderCommandInvoked(WebAccountProviderCommand^ command)
{
    AuthenticateWithRequestToken(command->WebAccountProvider, (String^) MSA_SCOPE_REQUESTED, (String^) MSA_CLIENT_ID);
}

// The function that will be called from AccountSettingsPane with the WebAccount
// that was selected by the user. 
void SDKTemplate::Scenario2_Msa::WebAccountCommandInvoked(WebAccountCommand^ command, WebAccountInvokedArgs^ args)
{
    if (args->Action == WebAccountAction::Remove)
    {
        LogoffAndRemoveAccount();
    }
}

// Displays privacy policy message on clicking it in the AccountsControl
void SDKTemplate::Scenario2_Msa::PrivacyPolicyInvoked(IUICommand^ command)
{
    rootPage->NotifyUser("Privacy policy clicked by user", NotifyType::StatusMessage);
}

// Displays other link message on clicking it in the AccountsControl
void SDKTemplate::Scenario2_Msa::OtherLinkInvoked(IUICommand^ command)
{
    rootPage->NotifyUser("Other link pressed by user", NotifyType::StatusMessage);
}

// Create a new WebAccountManager WebTokenRequest based on the Provider, Scope, ClientID and then create a task to send 
// that request asynchronously to WebAccountManager's RequestTokenAsync API
//
// WebAccountManager will then try three steps, in order:
//        (1): Check it's local cache to see if it has a valid token
//        (2): Try to silently request a new token from the MSA service
//        (3): Show the MSA UI for user interaction required (user credentials) before it may return a token.
//
// Because of WebAccountManager's ability to cache tokens, you should only need to call WebAccountManager when making token
// based requests and not require the ability to store a cached token within your app.
void SDKTemplate::Scenario2_Msa::AuthenticateWithRequestToken(WebAccountProvider^ passedProvider, String^ scope, String^ clientID)
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
void SDKTemplate::Scenario2_Msa::AuthenticateWithRequestTokenSilent(WebAccountProvider^ provider, String^ scope, String^ clientID, WebAccount^ account)
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
void SDKTemplate::Scenario2_Msa::OutputTokenResult(WebTokenRequestResult^ result)
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
void SDKTemplate::Scenario2_Msa::SaveAccount(WebAccount^ account)
{
    ApplicationData::Current->LocalSettings->Values->Insert((String^) STORED_ACCOUNT_ID_KEY, account->Id);
        m_account = account;

        //Update the UI
        button_SignIn->IsEnabled = false;
        button_GetTokenSilently->IsEnabled = true;
        button_ManageAccounts->IsEnabled = true;
        textblock_SignedInStatus->Text = "Signed in with:";
        textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Green);
        listview_SignedInAccounts->Items->Append(account->Id);
}

// Signs out the account using the SignOutAsync WebAccountManager API
// and removes our saved AccountId as it won't be valid when SignOutAsync finishes.
void SDKTemplate::Scenario2_Msa::LogoffAndRemoveAccount()
{
    if (m_account != nullptr)
    {
        //concurrency::create_task(m_account->SignOutAsync((String^)MSA_SCOPE_REQUESTED));
        m_account = nullptr;
    }

    if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_ACCOUNT_ID_KEY))
    {
        rootPage->NotifyUser("Signed out account id: " + ApplicationData::Current->LocalSettings->Values->Lookup((String^)STORED_ACCOUNT_ID_KEY), NotifyType::StatusMessage);
        ApplicationData::Current->LocalSettings->Values->Remove((String^) STORED_ACCOUNT_ID_KEY);

        //Update UI
        button_SignIn->IsEnabled = true;
        button_GetTokenSilently->IsEnabled = false;
        button_ManageAccounts->IsEnabled = false;
        textblock_SignedInStatus->Text = "Not signed in.";
        textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
        listview_SignedInAccounts->Items->Clear();
    }
}
