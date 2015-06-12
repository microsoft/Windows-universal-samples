// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario4_ManyProviders.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
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

Scenario4_ManyProviders::Scenario4_ManyProviders() : rootPage(MainPage::Current)
{
    InitializeComponent();
    InitalizeAccountsControlDialog();
}

void Scenario4_ManyProviders::OnNavigatedFrom(NavigationEventArgs^ e)
{
    CleanupAccountsControlDialog();
    LogoffAndRemoveAccount();
}

void SDKTemplate::Scenario4_ManyProviders::Button_SignIn_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Inside OnAccountCommandsRequested we will determine that we need to show accounts
    // and providers eligible for sign in because there is no signed in account
    AccountsSettingsPane::Show();
}

void SDKTemplate::Scenario4_ManyProviders::Button_GetTokenSilently_Click(Object^ sender, RoutedEventArgs^ e)
{
    AuthenticateWithRequestTokenSilent(m_account->WebAccountProvider, (String^)MSA_SCOPE_REQUESTED, (String^)MSA_CLIENT_ID, m_account);
}

void SDKTemplate::Scenario4_ManyProviders::Button_ManageAccounts_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Inside OnAccountCommandsRequested we will determine that we need to show the
    // active account since are signed into an account right now
    AccountsSettingsPane::Show();
}

void SDKTemplate::Scenario4_ManyProviders::Button_Reset_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Resetting", NotifyType::StatusMessage);

    LogoffAndRemoveAccount();

    rootPage->NotifyUser("Done Resetting", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario4_ManyProviders::InitalizeAccountsControlDialog()
{
    // Add to the event AccountCommandsRequested to load our list of providers into the AccountSettingsPane
    m_accountCommandsRequestedRegistrationToken = AccountsSettingsPane::GetForCurrentView()->AccountCommandsRequested::add(
        ref new TypedEventHandler<AccountsSettingsPane^, AccountsSettingsPaneCommandsRequestedEventArgs^>(
            this, 
            &SDKTemplate::Scenario4_ManyProviders::OnAccountCommandsRequested)
        );

    // Populate our Map of providers (MSA, AAD, and 3rd Party)
    // and load our account if we have one saved.
    GetProvidersAndAccounts();
}

void SDKTemplate::Scenario4_ManyProviders::CleanupAccountsControlDialog()
{
    // Remove the event handler from the AccountCommandsRequested
    AccountsSettingsPane::GetForCurrentView()->AccountCommandsRequested::remove(m_accountCommandsRequestedRegistrationToken);
}

void SDKTemplate::Scenario4_ManyProviders::OnAccountCommandsRequested(
    AccountsSettingsPane^ sender,
    AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    // This scenario only supports a single account at one time.
    // If there already is an account, we do not include a provider in the list
    // This will prevent the add account button from showing up.
    if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_ACCOUNT_ID_KEY))
    {
        AddWebAccount(e);
    }
    else
    {
        AddWebAccountProviders(e);
    }

    AddLinksAndDescription(e);
}

// Function to create our Map of providers and load an account if an account id is saved
void SDKTemplate::Scenario4_ManyProviders::GetProvidersAndAccounts()
{
    //Instantiate the provider list
    m_providers = ref new Map<String^, WebAccountProvider^>();

    // Make tasks to get the MSA and AAD providers by their IDs
    auto getMsaProvider = concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync((String^)MICROSOFT_PROVIDER_ID, (String^)CONSUMER_AUTHORITY));
    auto getAadProvider = concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync((String^)MICROSOFT_PROVIDER_ID, (String^)ORGANIZATION_AUTHORITY));
    WebAccountProvider^ appSpecificProvider = ref new WebAccountProvider((String^)APP_SPECIFIC_PROVIDER_ID, (String^)APP_SPECIFIC_PROVIDER_NAME, ref new Uri(this->BaseUri->ToString(), "Assets/smallTile-sdk.png"));

    // When that task completes, save the respective providers found to the provider list
    getMsaProvider.
        then([this](WebAccountProvider^ foundMsaProvider) 
        {
            if (foundMsaProvider != nullptr)
            {
                m_providers->Insert(foundMsaProvider->Id + foundMsaProvider->Authority, foundMsaProvider);
            }
        });
    
    getAadProvider.
        then([this](WebAccountProvider^ foundAadProvider)
        {
            if (foundAadProvider != nullptr)
            {
                m_providers->Insert(foundAadProvider->Id + foundAadProvider->Authority, foundAadProvider);
            }
        });

    m_providers->Insert(appSpecificProvider->Id, appSpecificProvider);

    if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_ACCOUNT_ID_KEY))
    {
        LoadAccount();
    }
}

// We've found that we have some key information saved about a previous
// account, so we will call FindAccountAsync to grab the WebAccount and save it
void SDKTemplate::Scenario4_ManyProviders::LoadAccount() 
{
    String^ accountID = (String^)ApplicationData::Current->LocalSettings->Values->Lookup((String^) STORED_ACCOUNT_ID_KEY);
    String^ providerID = (String^)ApplicationData::Current->LocalSettings->Values->Lookup((String^) STORED_PROVIDER_ID_KEY);
    String^ authority = (String^)ApplicationData::Current->LocalSettings->Values->Lookup((String^) STORED_AUTHORITY_ID_KEY);

    if (accountID == nullptr || providerID == nullptr || authority == nullptr) 
    {
        return;
    }

    if (providerID == APP_SPECIFIC_PROVIDER_ID)
    {
        WebAccountProvider^ appSpecificProvider = ref new WebAccountProvider((String^)APP_SPECIFIC_PROVIDER_ID, (String^)APP_SPECIFIC_PROVIDER_NAME, ref new Uri(this->BaseUri->ToString(), "Assets/smallTile-sdk.png"));
        WebAccount^ account = ref new WebAccount(appSpecificProvider, accountID, WebAccountState::None);
        SaveAccount(account->WebAccountProvider, account);
    }
    else
    {
        concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync(providerID))
            .then([this, providerID, accountID, authority](WebAccountProvider^ provider)
            {
                concurrency::create_task(WebAuthenticationCoreManager::FindAccountAsync(provider, accountID))
                    .then([this](WebAccount^ foundAccount)
                    {
                        SaveAccount(foundAccount->WebAccountProvider, foundAccount);
                    });
            });
    }        
}

// Similar to Scenario 1, but with a loop for the different providers we now support and declared earlier
void SDKTemplate::Scenario4_ManyProviders::AddWebAccountProviders(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    e->WebAccountProviderCommands->Clear();

    for(auto pair : m_providers)
    {
        // Create a new WebAccountProviderCommandInvokedHandler for the event fired when a user clicks on a provider in the AccountSettingsPane
        WebAccountProviderCommandInvokedHandler^ handler = ref new WebAccountProviderCommandInvokedHandler(this, &SDKTemplate::Scenario4_ManyProviders::WebAccountProviderCommandInvoked);

        // Make a new command based on the provider and the click handler we just created
        WebAccountProviderCommand^ providerCommand = ref new WebAccountProviderCommand(pair->Value, handler);

        // Append that command to the WebAccountProviderCommands list for the AccountSettingsPane
        e->WebAccountProviderCommands->Append(providerCommand);
    }
}

//Creates a web account command object for our account, if we have one saved, and adds it to the AccountsControl list of web account commands
void SDKTemplate::Scenario4_ManyProviders::AddWebAccount(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    // Create a new WebAccountCommandInvokedHandler for the event fired when a user clicks on an account in the AccountSettingsPane
    WebAccountCommandInvokedHandler^ handler = ref new WebAccountCommandInvokedHandler(this, &SDKTemplate::Scenario4_ManyProviders::WebAccountCommandInvoked);

    // Make a new command based on the account and the click handler we just created
    WebAccountCommand^ accountCommand = ref new WebAccountCommand(SDKTemplate::Scenario4_ManyProviders::m_account, handler, SupportedWebAccountActions::Remove);

    // Append that command to the WebAccountProviderCommands list for the AccountSettingsPane
    e->WebAccountCommands->Append(accountCommand);
}

// You can add links and descriptive text such as privacy policy, help, general account settings
void SDKTemplate::Scenario4_ManyProviders::AddLinksAndDescription(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    e->HeaderText = "Describe what adding an account to your application will do for the user.";

    e->Commands->Append(ref new SettingsCommand("privacypolicy", "Privacy Policy", ref new UICommandInvokedHandler(this, &SDKTemplate::Scenario4_ManyProviders::PrivacyPolicyInvoked)));
    e->Commands->Append(ref new SettingsCommand("otherlink", "Other Link", ref new UICommandInvokedHandler(this, &SDKTemplate::Scenario4_ManyProviders::OtherLinkInvoked)));
}

void SDKTemplate::Scenario4_ManyProviders::WebAccountProviderCommandInvoked(WebAccountProviderCommand^ command)
{
    // The function that will be called with the WebAccountProviderCommand passed from AccountSettingsPane
    if (command->WebAccountProvider->Authority == "organizations")
    {
        AuthenticateWithRequestToken(command->WebAccountProvider, (String^)AAD_SCOPE_REQUESTED, (String^)AAD_CLIENT_ID);
    }
    else
    {
        AuthenticateWithRequestToken(command->WebAccountProvider, (String^)MSA_SCOPE_REQUESTED, (String^)MSA_CLIENT_ID);
    }
}

// The function that will be called from AccountSettingsPane with the WebAccount
// that was selected by the user.
void SDKTemplate::Scenario4_ManyProviders::WebAccountCommandInvoked(WebAccountCommand^ command, WebAccountInvokedArgs^ args)
{
    if (args->Action == WebAccountAction::Remove)
    {
        LogoffAndRemoveAccount();
    }
}

// Displays privacy policy message on clicking it in the AccountsControl
void SDKTemplate::Scenario4_ManyProviders::PrivacyPolicyInvoked(IUICommand^ command)
{
    rootPage->NotifyUser("Privacy policy clicked by user", NotifyType::StatusMessage);
}

// Displays other link message on clicking it in the AccountsControl
void SDKTemplate::Scenario4_ManyProviders::OtherLinkInvoked(IUICommand^ command)
{
    rootPage->NotifyUser("Other link pressed by user", NotifyType::StatusMessage);
}

// Create a new WebAccountManager WebTokenRequest based on the Provider, Scope, ClientID and then create a task to send 
// that request asynchronously to WebAccountManager's RequestTokenAsync
//
// WebAccountManager will then try three steps, in order:
//        (1): Check it's local cache to see if it has a valid token
//        (2): Try to silently request a new token from the MSA service
//        (3): Show the MSA UI for user interaction required (user credentials) before it may return a token.
//
// Because of WebAccountManager's ability to cache tokens, you should only need to call WebAccountManager when making token
// based requests and not require the ability to store a cached token within your app.
void SDKTemplate::Scenario4_ManyProviders::AuthenticateWithRequestToken(WebAccountProvider^ provider, String^ scope, String^ clientID)
{
    rootPage->NotifyUser("Requested " + provider->DisplayName + " from AccountsManager dialog.", NotifyType::StatusMessage);

    WebTokenRequest^ wtr = ref new WebTokenRequest(provider, scope, clientID);
    wtr->Properties->Insert("authority", "https://login.windows.net/common");
    wtr->Properties->Insert("resource", "https://sharepoint.com");
    auto getWebTokenRequestResult = concurrency::create_task(WebAuthenticationCoreManager::RequestTokenAsync(wtr));

    // When our task finishes it will return result of the operation, and if successful it will contain a token
    // and WebAccount. We save the WebAccount as the "active" account.
    getWebTokenRequestResult.then([this, provider](WebTokenRequestResult^ webTokenRequestResult)
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
                SaveAccount(provider, account);
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
void SDKTemplate::Scenario4_ManyProviders::AuthenticateWithRequestTokenSilent(WebAccountProvider^ provider, String^ scope, String^ clientID, WebAccount^ account)
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
void SDKTemplate::Scenario4_ManyProviders::OutputTokenResult(WebTokenRequestResult^ result)
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

// Saves the ProviderId, Authority, and AccountId in LocalSettings as we can handle
// different types of account providers in Scenario 2.
void SDKTemplate::Scenario4_ManyProviders::SaveAccount(WebAccountProvider^ provider, WebAccount^ account)
{
    m_account = account;
    ApplicationData::Current->LocalSettings->Values->Insert((String^) STORED_PROVIDER_ID_KEY, m_account->WebAccountProvider->Id);
    ApplicationData::Current->LocalSettings->Values->Insert((String^) STORED_AUTHORITY_ID_KEY, m_account->WebAccountProvider->Authority);
    ApplicationData::Current->LocalSettings->Values->Insert((String^) STORED_ACCOUNT_ID_KEY, m_account->Id);

    //Update the UI
    button_SignIn->IsEnabled = false;
    button_GetTokenSilently->IsEnabled = true;
    button_ManageAccounts->IsEnabled = true;
    textblock_SignedInStatus->Text = "Signed in with:";
    textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Green);
    listview_SignedInAccounts->Items->Append(account->Id);
}

// When we sign out, then remove all our saved keys as well.
void SDKTemplate::Scenario4_ManyProviders::LogoffAndRemoveAccount()
{
    if (m_account != nullptr)
    {
        //concurrency::create_task(account->SignOutAsync((String^) MSA_SCOPE_REQUESTED));
    }

    if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_ACCOUNT_ID_KEY))
    {
        ApplicationData::Current->LocalSettings->Values->Remove((String^) STORED_ACCOUNT_ID_KEY);
    }

    if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_PROVIDER_ID_KEY))
    {
        ApplicationData::Current->LocalSettings->Values->Remove((String^) STORED_PROVIDER_ID_KEY);
    }

    if (ApplicationData::Current->LocalSettings->Values->HasKey((String^) STORED_AUTHORITY_ID_KEY))
    {
        ApplicationData::Current->LocalSettings->Values->Remove((String^) STORED_AUTHORITY_ID_KEY);
    }

    //Update UI
    button_SignIn->IsEnabled = true;
    button_GetTokenSilently->IsEnabled = false;
    button_ManageAccounts->IsEnabled = false;
    textblock_SignedInStatus->Text = "Not signed in.";
    textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
    listview_SignedInAccounts->Items->Clear();
}
