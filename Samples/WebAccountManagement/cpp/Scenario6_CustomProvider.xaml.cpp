// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario6_CustomProvider.xaml.h"

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

Scenario6_CustomProvider::Scenario6_CustomProvider() : rootPage(MainPage::Current)
{
    InitializeComponent();
    InitalizeAccountsControlDialog();
}

void Scenario6_CustomProvider::OnNavigatedFrom(NavigationEventArgs^ e)
{
    CleanupAccountsControlDialog();
}

void SDKTemplate::Scenario6_CustomProvider::Button_SignIn_Click(Object^ sender, RoutedEventArgs^ e)
{
    AccountsSettingsPane::Show();
}

void SDKTemplate::Scenario6_CustomProvider::Button_GetTokenSilently_Click(Object^ sender, RoutedEventArgs^ e)
{
    AuthenticateWithRequestTokenSilent(m_account->WebAccountProvider, MSA_SCOPE_REQUESTED, MSA_CLIENT_ID, m_account);
}

void SDKTemplate::Scenario6_CustomProvider::Button_ManageAccount_Click(Object^ sender, RoutedEventArgs^ e)
{
    AccountsSettingsPane::Show();
}

void SDKTemplate::Scenario6_CustomProvider::Button_Reset_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Resetting", NotifyType::StatusMessage);

    RemoveCustomProviders();
    LogoffAndRemoveAccount();

    rootPage->NotifyUser("Done Resetting", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario6_CustomProvider::Button_AddProvider_Click(Object^ sender, RoutedEventArgs^ e)
{
    AddCustomProvider(textBox_ProviderId->Text, textBox_Authority->Text);
}

void SDKTemplate::Scenario6_CustomProvider::InitalizeAccountsControlDialog()
{
    // Add to the event AccountCommandsRequested to load our list of providers into the AccountSettingsPane
    m_accountCommandsRequestedRegistrationToken = AccountsSettingsPane::GetForCurrentView()->AccountCommandsRequested::add(
        ref new TypedEventHandler<AccountsSettingsPane^, AccountsSettingsPaneCommandsRequestedEventArgs^>(
        this,
        &SDKTemplate::Scenario6_CustomProvider::OnAccountCommandsRequested)
        );

    // Populate our list of providers with the MSA provider, and attempt to find any accounts saved.
    GetProvidersAndAccounts();
}

void SDKTemplate::Scenario6_CustomProvider::CleanupAccountsControlDialog()
{
    // Remove the event handler from the AccountCommandsRequested
    AccountsSettingsPane::GetForCurrentView()->AccountCommandsRequested::remove(m_accountCommandsRequestedRegistrationToken);

    // Clean up any account that may still be logged in
    LogoffAndRemoveAccount();
}

void SDKTemplate::Scenario6_CustomProvider::OnAccountCommandsRequested(
    AccountsSettingsPane^ sender,
    AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    // This scenario only supports a single account at one time.
    // If there already is an account, we do not include a provider in the list
    // This will prevent the add account button from showing up.
    if (ApplicationData::Current->LocalSettings->Values->HasKey(STORED_ACCOUNT_ID_KEY))
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
void SDKTemplate::Scenario6_CustomProvider::GetProvidersAndAccounts()
{
    //Instantiate the provider list
    m_providers = ref new Map<String^, WebAccountProvider^>();

    if (ApplicationData::Current->LocalSettings->Values->HasKey(STORED_ACCOUNT_ID_KEY))
    {
        LoadAccount();
    }
}

// We've found that we have some key information saved about a previous
// account, so we will call FindAccountAsync to grab the WebAccount and save it
void SDKTemplate::Scenario6_CustomProvider::LoadAccount()
{
    auto accountID = safe_cast<String^>(ApplicationData::Current->LocalSettings->Values->Lookup(STORED_ACCOUNT_ID_KEY));
    auto providerID = safe_cast<String^>(ApplicationData::Current->LocalSettings->Values->Lookup(STORED_PROVIDER_ID_KEY));
    auto authority = safe_cast<String^>(ApplicationData::Current->LocalSettings->Values->Lookup(STORED_AUTHORITY_ID_KEY));

    if (accountID == nullptr || providerID == nullptr || authority == nullptr) 
    {
        return;
    }

    concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync(providerID))
        .then([this, providerID, accountID, authority](WebAccountProvider^ provider)
    {
        concurrency::create_task(WebAuthenticationCoreManager::FindAccountAsync(provider, accountID))
            .then([this](WebAccount^ foundAccount)
        {
            if (foundAccount)
            {
                SaveAccount(foundAccount);
            }
        });
    });
}

// This function takes the passed in strings from the textboxes for adding a custom provider.
// It will check to make sure the provider is found, then add the provider to our list of providers and
// clear the textboxes.
void SDKTemplate::Scenario6_CustomProvider::AddCustomProvider(String^ providerId, String^ authority)
{
    if (providerId)
    {
        (authority ? 
            concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync(providerId, authority)) :
            concurrency::create_task(WebAuthenticationCoreManager::FindAccountProviderAsync(providerId)))
            .then([this, providerId](WebAccountProvider^ provider)
        {
            if (provider)
            {
                m_providers->Insert(provider->Id, provider);
                textBox_ProviderId->Text = "";
                textBox_Authority->Text = "";
                rootPage->NotifyUser("Provider was found and added for id: " + providerId + ".", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Provider was not found for that provider id: " + providerId + ".", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("A provided must be specified.", NotifyType::StatusMessage);
    }
}

// Similar to Scenario 1, but with a loop for the different providers we now support and declared earlier
void SDKTemplate::Scenario6_CustomProvider::AddWebAccountProviders(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    for (auto pair : m_providers)
    {
        // Create a new WebAccountProviderCommandInvokedHandler for the event fired when a user clicks on a provider in the AccountSettingsPane
        WebAccountProviderCommandInvokedHandler^ handler = ref new WebAccountProviderCommandInvokedHandler(this, &SDKTemplate::Scenario6_CustomProvider::WebAccountProviderCommandInvoked);

        // Make a new command based on the provider and the click handler we just created
        WebAccountProviderCommand^ ProviderCommand = ref new WebAccountProviderCommand(pair->Value, handler);

        // Append that command to the WebAccountProviderCommands list for the AccountSettingsPane
        e->WebAccountProviderCommands->Append(ProviderCommand);
    }
}

//Creates a web account command object for our account, if we have one saved, and adds it to the AccountsControl list of web account commands
void SDKTemplate::Scenario6_CustomProvider::AddWebAccount(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    // Create a new WebAccountCommandInvokedHandler for the event fired when a user clicks on an account in the AccountSettingsPane
    WebAccountCommandInvokedHandler^ handler = ref new WebAccountCommandInvokedHandler(this, &SDKTemplate::Scenario6_CustomProvider::WebAccountCommandInvoked);

    // Make a new command based on the account and the click handler we just created
    WebAccountCommand^ AccountCommand = ref new WebAccountCommand(SDKTemplate::Scenario6_CustomProvider::m_account, handler, SupportedWebAccountActions::Remove);

    // Append that command to the WebAccountProviderCommands list for the AccountSettingsPane
    e->WebAccountCommands->Append(AccountCommand);
}

// You can add links and descriptive text such as privacy policy, help, general account settings
void SDKTemplate::Scenario6_CustomProvider::AddLinksAndDescription(AccountsSettingsPaneCommandsRequestedEventArgs^ e)
{
    e->HeaderText = "Describe what adding an account to your application will do for the user.";

    e->Commands->Append(ref new SettingsCommand("privacypolicy", "Privacy Policy", ref new UICommandInvokedHandler(this, &SDKTemplate::Scenario6_CustomProvider::PrivacyPolicyInvoked)));
    e->Commands->Append(ref new SettingsCommand("otherlink", "Other Link", ref new UICommandInvokedHandler(this, &SDKTemplate::Scenario6_CustomProvider::OtherLinkInvoked)));
}

// The function that will be called from AccountSettingsPane with the WebAccountProvider
// that was clicked by the user
void SDKTemplate::Scenario6_CustomProvider::WebAccountProviderCommandInvoked(WebAccountProviderCommand^ command)
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
void SDKTemplate::Scenario6_CustomProvider::WebAccountCommandInvoked(WebAccountCommand^ command, WebAccountInvokedArgs^ args)
{
    if (args->Action == WebAccountAction::Remove)
    {
        LogoffAndRemoveAccount();
    }
}

// Displays privacy policy message on clicking it in the AccountsControl
void SDKTemplate::Scenario6_CustomProvider::PrivacyPolicyInvoked(IUICommand^ command)
{
    rootPage->NotifyUser("Privacy policy clicked by user", NotifyType::StatusMessage);
}

// Displays other link message on clicking it in the AccountsControl
void SDKTemplate::Scenario6_CustomProvider::OtherLinkInvoked(IUICommand^ command)
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
void SDKTemplate::Scenario6_CustomProvider::AuthenticateWithRequestToken(WebAccountProvider^ provider, String^ scope, String^ clientID)
{
    rootPage->NotifyUser("Requested " + provider->DisplayName + " from AccountsManager dialog.", NotifyType::StatusMessage);

    auto wtr = ref new WebTokenRequest(provider, scope, clientID);

    // When our task finishes it will return result of the operation, and if successful it will contain a token
    // and WebAccount. We save the WebAccount as the "active" account.
    concurrency::create_task(WebAuthenticationCoreManager::RequestTokenAsync(wtr)).then([this, provider](WebTokenRequestResult^ webTokenRequestResult)
    {
        if (webTokenRequestResult->ResponseStatus == WebTokenRequestStatus::Success)
        {
            if (webTokenRequestResult->ResponseData->GetAt(0)->WebAccount)
            {
                if (ApplicationData::Current->LocalSettings->Values->HasKey(STORED_ACCOUNT_ID_KEY))
                {
                    ApplicationData::Current->LocalSettings->Values->Remove(STORED_ACCOUNT_ID_KEY);
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
void SDKTemplate::Scenario6_CustomProvider::AuthenticateWithRequestTokenSilent(WebAccountProvider^ provider, String^ scope, String^ clientID, WebAccount^ account)
{
    rootPage->NotifyUser("Requested " + provider->DisplayName + " from AccountsManager dialog.", NotifyType::StatusMessage);

    auto wtr = ref new WebTokenRequest(provider, scope, clientID);

    // When our task finishes it will return result of the operation, and if successful it will contain a token
    // and WebAccount. We save the WebAccount as the "active" account.
    concurrency::create_task(WebAuthenticationCoreManager::GetTokenSilentlyAsync(wtr, account)).then([this](WebTokenRequestResult^ webTokenRequestResult)
    {
        if (webTokenRequestResult->ResponseStatus == WebTokenRequestStatus::Success)
        {
            // Perform an operation with the token you just got
        }

        OutputTokenResult(webTokenRequestResult);
    });
}

// Displays the result of requesting a token asynchronously to the main page
void SDKTemplate::Scenario6_CustomProvider::OutputTokenResult(WebTokenRequestResult^ result)
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
void SDKTemplate::Scenario6_CustomProvider::SaveAccount(WebAccount^ account)
{
    ApplicationData::Current->LocalSettings->Values->Insert(STORED_ACCOUNT_ID_KEY, account->Id);
    m_account = account;

    // Update the UI
    button_SignIn->IsEnabled = false;
    button_GetTokenSilently->IsEnabled = true;
    button_ManageAccount->IsEnabled = true;
    textblock_SignedInStatus->Text = "Signed in with:";
    textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Green);
    listview_SignedInAccounts->Items->Append(account->Id);
}

// Signs out the account using the SignOutAsync Token Broker API
// and removes our saved AccountId as it won't be valid when SignOutAsync finishes.
void SDKTemplate::Scenario6_CustomProvider::LogoffAndRemoveAccount()
{
    if (m_account)
    {
        //concurrency::create_task(account->SignOutAsync( MSA_SCOPE_REQUESTED));
    }

    if (ApplicationData::Current->LocalSettings->Values->HasKey(STORED_ACCOUNT_ID_KEY))
    {
        ApplicationData::Current->LocalSettings->Values->Remove(STORED_ACCOUNT_ID_KEY);
    }

    // Update UI
    button_SignIn->IsEnabled = true;
    button_GetTokenSilently->IsEnabled = false;
    button_ManageAccount->IsEnabled = false;
    textblock_SignedInStatus->Text = "Not signed in.";
    textblock_SignedInStatus->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
    listview_SignedInAccounts->Items->Clear();
}

void SDKTemplate::Scenario6_CustomProvider::RemoveCustomProviders()
{
    m_providers->Clear();
    GetProvidersAndAccounts();
}