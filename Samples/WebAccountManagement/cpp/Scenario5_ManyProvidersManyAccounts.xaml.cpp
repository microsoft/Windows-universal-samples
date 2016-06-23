// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario5_ManyProvidersManyAccounts.xaml.h"

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

Scenario5_ManyProvidersManyAccounts::Scenario5_ManyProvidersManyAccounts() : rootPage(MainPage::Current)
{
    InitializeComponent();
    InitalizeAccountsControlDialog();
}

void Scenario5_ManyProvidersManyAccounts::OnNavigatedFrom(NavigationEventArgs^ e)
{
    CleanupAccountsControlDialog();
    LogoffAndRemoveAllAccounts();
}

void SDKTemplate::Scenario5_ManyProvidersManyAccounts::Button_AddNewAccount_Click(Object^ sender, RoutedEventArgs^ e)
{
    m_selectedAccountAction = AccountAction::AddAccount;
    AccountsSettingsPane::Show();
}

void SDKTemplate::Scenario5_ManyProvidersManyAccounts::Button_ManageSignedInAccounts_Click(Object^ sender, RoutedEventArgs^ e)
{
    m_selectedAccountAction = AccountAction::ManageAccount;
    AccountsSettingsPane::Show();
}

void SDKTemplate::Scenario5_ManyProvidersManyAccounts::Button_Reset_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Resetting", NotifyType::StatusMessage);

    LogoffAndRemoveAllAccounts();

    rootPage->NotifyUser("Done Resetting", NotifyType::StatusMessage);
}

// Create local storage containers for the ID strings. These will be used to recreate
// WebAccounts.
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::CreateLocalDataContainers()
{
    ApplicationData::Current->LocalSettings->CreateContainer(ACCOUNTS_CONTAINER, ApplicationDataCreateDisposition::Always);
    ApplicationData::Current->LocalSettings->Containers->Lookup(ACCOUNTS_CONTAINER)->CreateContainer(PROVIDER_ID_SUBCONTAINER, ApplicationDataCreateDisposition::Always);
    ApplicationData::Current->LocalSettings->Containers->Lookup(ACCOUNTS_CONTAINER)->CreateContainer(AUTHORITY_SUBCONTAINER, ApplicationDataCreateDisposition::Always);
}

void SDKTemplate::Scenario5_ManyProvidersManyAccounts::InitalizeAccountsControlDialog()
{
    // Add to the event AccountCommandsRequested to load our list of providers into the AccountSettingsPane
    m_accountCommandsRequestedRegistrationToken = AccountsSettingsPane::GetForCurrentView()->AccountCommandsRequested +=
        ref new TypedEventHandler<AccountsSettingsPane^, AccountsSettingsPaneCommandsRequestedEventArgs^>(
            this, &SDKTemplate::Scenario5_ManyProvidersManyAccounts::OnAccountCommandsRequested);

    CreateLocalDataContainers();
    GetProviders();
    GetAccounts();
}

void SDKTemplate::Scenario5_ManyProvidersManyAccounts::CleanupAccountsControlDialog()
{
    // Remove the event handler from the AccountCommandsRequested
    AccountsSettingsPane::GetForCurrentView()->AccountCommandsRequested -= m_accountCommandsRequestedRegistrationToken;
}

void SDKTemplate::Scenario5_ManyProvidersManyAccounts::OnAccountCommandsRequested(
    AccountsSettingsPane^ sender,
    AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    // Add all the accounts, providers, and descriptive elements to the AccountControl picker
    if (m_selectedAccountAction == AccountAction::AddAccount)
    {
        AddWebAccountProviders(e);
    }
    else
    {
        AddWebAccounts(e);
    }
    AddLinksAndDescription(e);
}

void SDKTemplate::Scenario5_ManyProvidersManyAccounts::GetProviders()
{
    // Instantiate the provider list
    m_providers = ref new Map<String^, WebAccountProvider^>();

    // Make tasks to get the MSA and AAD providers by their IDs
    auto appSpecificProvider = ref new WebAccountProvider( APP_SPECIFIC_PROVIDER_ID, APP_SPECIFIC_PROVIDER_NAME, 
        ref new Uri(this->BaseUri->ToString(), "Assets/smallTile-sdk.png"));

    // When that task completes, save the providers found to the provider list
    concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync(MICROSOFT_PROVIDER_ID, CONSUMER_AUTHORITY)).
        then([this](WebAccountProvider^ foundMsaProvider)
    {
        if (foundMsaProvider)
        {
            m_providers->Insert(foundMsaProvider->Id + foundMsaProvider->Authority, foundMsaProvider);
        }
    });

    concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync(MICROSOFT_PROVIDER_ID, ORGANIZATION_AUTHORITY)).
        then([this](WebAccountProvider^ foundAadProvider)
    {
        if (foundAadProvider)
        {
            m_providers->Insert(foundAadProvider->Id + foundAadProvider->Authority, foundAadProvider);
        }
    });

    m_providers->Insert(appSpecificProvider->Id, appSpecificProvider);
}

// Iterate through our saved Account Ids and create WebAccounts for each of them,
// then save them in our list of WebAccounts
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::GetAccounts()
{
    m_accounts = ref new Map<String^, WebAccount^>();

    auto accountListContainer = ApplicationData::Current->LocalSettings->Containers->Lookup(ACCOUNTS_CONTAINER);

    for (Object^ value : accountListContainer->Containers->Lookup(PROVIDER_ID_SUBCONTAINER)->Values)
    {
        auto accountID = safe_cast<String^>(value);
        auto providerID = safe_cast<String^>(accountListContainer->Containers->Lookup(PROVIDER_ID_SUBCONTAINER)->Values->Lookup(accountID));
        auto authority = safe_cast<String^>(accountListContainer->Containers->Lookup(AUTHORITY_SUBCONTAINER)->Values->Lookup(accountID));

        if (accountID == nullptr || providerID == nullptr || authority == nullptr) 
        {
            continue;
        }

        concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync(providerID, authority))
            .then([this, accountID](WebAccountProvider^ provider)
        {
            if (provider->Id == APP_SPECIFIC_PROVIDER_ID)
            {
                SaveAccount(ref new WebAccount(provider, accountID, WebAccountState::None));
            }
            else
            {
                concurrency::create_task(WebAuthenticationCoreManager::FindAccountAsync(provider, accountID))
                    .then([this](WebAccount^ foundAccount)
                {
                    SaveAccount(foundAccount);
                });
            }
        });
    }
}

// Similar to Scenario 1, but with a loop for the different providers we now support and declared earlier
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::AddWebAccountProviders(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    for (const auto& pair : m_providers)
    {
        // Create a new WebAccountProviderCommandInvokedHandler for the event fired when a user clicks on a provider in the AccountSettingsPane
        auto handler = ref new WebAccountProviderCommandInvokedHandler(this, &SDKTemplate::Scenario5_ManyProvidersManyAccounts::WebAccountProviderCommandInvoked);

        // Make a new command based on the provider and the click handler we just created
        auto providerCommand = ref new WebAccountProviderCommand(pair->Value, handler);

        // Append that command to the WebAccountProviderCommands list for the AccountSettingsPane
        e->WebAccountProviderCommands->Append(providerCommand);
    }
}

// Iterate through the web accounts we have and add a WebAccountsCommand for each to the AccountControl's list of web accounts
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::AddWebAccounts(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    for (const auto& account : m_accounts)
    {
        auto handler = ref new WebAccountCommandInvokedHandler(this, &SDKTemplate::Scenario5_ManyProvidersManyAccounts::WebAccountCommandInvoked);
        auto command = ref new WebAccountCommand(account->Value, handler, SupportedWebAccountActions::Remove | SupportedWebAccountActions::Manage);
        e->WebAccountCommands->Append(command);
    }
}

// You can add links and descriptive text such as privacy policy, help, general account settings
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::AddLinksAndDescription(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    e->HeaderText = "Describe what adding an account to your application will do for the user.";

    e->Commands->Append(ref new SettingsCommand("privacypolicy", "Privacy Policy", ref new UICommandInvokedHandler(this, &SDKTemplate::Scenario5_ManyProvidersManyAccounts::PrivacyPolicyInvoked)));
    e->Commands->Append(ref new SettingsCommand("otherlink", "Other Link", ref new UICommandInvokedHandler(this, &SDKTemplate::Scenario5_ManyProvidersManyAccounts::OtherLinkInvoked)));
}

// The function that will be called with the WebAccountProviderCommand passed from AccountSettingsPane
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::WebAccountProviderCommandInvoked(WebAccountProviderCommand^ command)
{ 
    if (command->WebAccountProvider->Authority == "organizations")
    {
        AuthenticateWithRequestToken(command->WebAccountProvider, AAD_SCOPE_REQUESTED, AAD_CLIENT_ID);
    }
    else
    {
        AuthenticateWithRequestToken(command->WebAccountProvider, MSA_SCOPE_REQUESTED, MSA_CLIENT_ID);
    }
}

// Handler function for when users select different commands for accounts in the AccountControl
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::WebAccountCommandInvoked(WebAccountCommand^ command, WebAccountInvokedArgs^ args)
{
    if (args->Action == WebAccountAction::Remove)
    {
        LogoffAndRemoveAccount(command->WebAccount);
    }
    else if (args->Action == WebAccountAction::Manage)
    {
        // Display user management UI for this account
        rootPage->NotifyUser("Manage invoked for user: " + command->WebAccount->UserName, NotifyType::StatusMessage);
    }
}

// Displays privacy policy message on clicking it in the AccountsControl
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::PrivacyPolicyInvoked(IUICommand^ command)
{
    rootPage->NotifyUser("Privacy policy clicked by user", NotifyType::StatusMessage);
}

// Displays other link message on clicking it in the AccountsControl
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::OtherLinkInvoked(IUICommand^ command)
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
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::AuthenticateWithRequestToken(WebAccountProvider^ provider, String^ scope, String^ clientID)
{
    rootPage->NotifyUser("Requested " + provider->DisplayName + " from AccountsManager dialog.", NotifyType::StatusMessage);

    auto wtr = ref new WebTokenRequest(provider, scope, clientID);

    // When our task finishes it will return result of the operation, and if successful it will contain a token
    // and WebAccount. We save the WebAccount as the "active" account.
    concurrency::create_task(WebAuthenticationCoreManager::RequestTokenAsync(wtr)).then([this](WebTokenRequestResult^ webTokenRequestResult)
    {
        if (webTokenRequestResult->ResponseStatus == WebTokenRequestStatus::Success)
        {
            if (webTokenRequestResult->ResponseData->GetAt(0)->WebAccount)
            {
                SaveAccount(webTokenRequestResult->ResponseData->GetAt(0)->WebAccount);
            }
        }

        OutputTokenResult(webTokenRequestResult);
    });
}

// Displays the result of requesting a token asynchronously to the main page
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::OutputTokenResult(WebTokenRequestResult^ result)
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
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::SaveAccount(WebAccount^ account)
{
    ApplicationDataContainer^ accountsContainer = ApplicationData::Current->LocalSettings->Containers->Lookup(ACCOUNTS_CONTAINER);

    // If ID isn't empty then this is an account based on either MSA or AAD and we need to save the account id and provider information. Otherwise
    // it is an app-specific provider.
    if (account->Id != "")
    {
        accountsContainer->Containers->Lookup(PROVIDER_ID_SUBCONTAINER)->Values->Insert(account->Id, account->WebAccountProvider->Id);
        accountsContainer->Containers->Lookup(AUTHORITY_SUBCONTAINER)->Values->Insert(account->Id, account->WebAccountProvider->Authority);
        m_accounts->Insert(account->UserName, account);
    }
    else
    {
        accountsContainer->Containers->Lookup(PROVIDER_ID_SUBCONTAINER)->Values->Insert(account->UserName, account->WebAccountProvider->Id);
        m_accounts->Insert(account->Id, account);
    }

    // Update UI
    button_ManageSignedInAccounts->IsEnabled = true;
    textblock_SignedInStatus->Text = "Signed in with:";
    textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Green);
    listview_SignedInAccounts->Items->Append(account->Id);
}

// When we sign out, then remove all our saved keys as well.
IAsyncAction^ SDKTemplate::Scenario5_ManyProvidersManyAccounts::LogoffAndRemoveAccount(WebAccount^ account)
{
    ApplicationDataContainer^ accountsContainer = ApplicationData::Current->LocalSettings->Containers->Lookup(ACCOUNTS_CONTAINER);

    unsigned int indexOfAccount = 0;

    // For the MSA and AAD providers we need to remove the account id, otherwise we need to remove the app-specific account username.
    if (account->WebAccountProvider->Id != APP_SPECIFIC_PROVIDER_ID)
    {
        //concurrency::create_task(account->SignOutAsync( MSA_SCOPE_REQUESTED));

        accountsContainer->Containers->Lookup(PROVIDER_ID_SUBCONTAINER)->Values->Remove(account->Id);
        accountsContainer->Containers->Lookup(AUTHORITY_SUBCONTAINER)->Values->Remove(account->Id);

        // Find the index of the account Id in the list of signed in accounts, if indexOf is true then remove it.
        if (listview_SignedInAccounts->Items->IndexOf(account->Id, &indexOfAccount))
        {
            listview_SignedInAccounts->Items->RemoveAt(indexOfAccount);
        }
    }
    else
    {
        // perform any actions needed to log off the app specific account
        accountsContainer->Containers->Lookup(PROVIDER_ID_SUBCONTAINER)->Values->Remove(account->UserName);

        // Find the index of the account username in the list of signed in accounts, if indexOf is true then remove it.
        if (listview_SignedInAccounts->Items->IndexOf(account->UserName, &indexOfAccount))
        {
            listview_SignedInAccounts->Items->RemoveAt(indexOfAccount);
        }
    }

    if (listview_SignedInAccounts->Items->Size == 0)
    {
        // Update UI
        button_AddAccount->IsEnabled = true;
        button_ManageSignedInAccounts->IsEnabled = false;
        textblock_SignedInStatus->Text = "Not signed in.";
        textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
    }

    return nullptr;
}

// When we sign out, then remove all our saved keys as well.
void SDKTemplate::Scenario5_ManyProvidersManyAccounts::LogoffAndRemoveAllAccounts()
{
    for (const auto& account : m_accounts)
    {
       //concurrency::create_task(LogoffAndRemoveAccount(account->Value));
    }

    m_accounts->Clear();

    // Update UI
    button_AddAccount->IsEnabled = true;
    button_ManageSignedInAccounts->IsEnabled = false;
    textblock_SignedInStatus->Text = "Not signed in.";
    textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
    listview_SignedInAccounts->Items->Clear();
}
