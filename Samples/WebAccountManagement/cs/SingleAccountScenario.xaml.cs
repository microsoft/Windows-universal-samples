//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.ApplicationSettings;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Security.Credentials;
using Windows.Security.Authentication.Web.Core;
using System;
using Windows.Storage;
using System.Threading.Tasks;
using System.Collections.Generic;
using Windows.UI.Popups;


namespace Accounts
{

    public sealed partial class SingleAccountScenario : Page
    {
        private MainPage rootPage;

        const string AppSpecificProviderId = "myproviderid";
        const string AppSpecificProviderName = "App specific provider";

        const string MicrosoftProviderId = "https://login.microsoft.com";
        const string MicrosoftAccountAuthority = "consumers";
        const string AzureActiveDirectoryAuthority = "organizations";

        const string StoredAccountIdKey = "accountid";
        const string StoredProviderIdKey = "providerid";
        const string StoredAuthorityKey = "authority";

        const string MicrosoftAccountClientId = "none";
        const string MicrosoftAccountScopeRequested = "wl.basic";

        // To obtain azureAD tokens, you must register this app on the AzureAD portal, and obtain the client ID
        const string AzureActiveDirectoryClientId = "";
        const string AzureActiveDirectoryScopeRequested = "";

        public SingleAccountScenario()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            AccountsSettingsPane.GetForCurrentView().AccountCommandsRequested += OnAccountCommandsRequested;

            IdentityChecker.SampleIdentityConfigurationCorrect(NotRegisteredWarning, AzureActiveDirectoryClientId);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            AccountsSettingsPane.GetForCurrentView().AccountCommandsRequested -= OnAccountCommandsRequested;
        }

        private void Button_ShowAccountSettings(object sender, RoutedEventArgs e)
        {
            ((Button)sender).IsEnabled = false;

            rootPage.NotifyUser("Launching AccountSettingsPane", NotifyType.StatusMessage);

            AccountsSettingsPane.Show();

            ((Button)sender).IsEnabled = true;
        }

        private async void Button_Reset(object sender, RoutedEventArgs e)
        {
            ((Button)sender).IsEnabled = false;

            rootPage.NotifyUser("Resetting...", NotifyType.StatusMessage);

            await LogoffAndRemoveAccount();

            ((Button)sender).IsEnabled = true;
        }

        private async void OnAccountCommandsRequested(
            AccountsSettingsPane sender,
            AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            // In order to make async calls within this callback, the deferral object is needed
            AccountsSettingsPaneEventDeferral deferral = e.GetDeferral();

            // This scenario only lets the user have one account at a time.
            // If there already is an account, we do not include a provider in the list
            // This will prevent the add account button from showing up.
            if (HasAccountStored())
            {
                await AddWebAccountToPane(e);
            }
            else
            {
                await AddWebAccountProvidersToPane(e);
            }

            AddLinksAndDescription(e);

            deferral.Complete();
        }

        private void AddLinksAndDescription(AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            e.HeaderText = "Describe what adding an account to your application will do for the user";

            // You can add links such as privacy policy, help, general account settings
            e.Commands.Add(new SettingsCommand("privacypolicy", "Privacy policy", PrivacyPolicyInvoked));
            e.Commands.Add(new SettingsCommand("otherlink", "Other link", OtherLinkInvoked));
        }

        private async Task AddWebAccountProvidersToPane(AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            List<WebAccountProvider> providers = new List<WebAccountProvider>();

            // Microsoft account and Azure AD providers will always return. Non-installed plugins or incorrect identities will return null
            providers.Add(await GetProvider(MicrosoftProviderId, MicrosoftAccountAuthority));
            providers.Add(await GetProvider(MicrosoftProviderId, AzureActiveDirectoryAuthority));
            providers.Add(await GetProvider(AppSpecificProviderId));

            foreach (WebAccountProvider provider in providers)
            {
                WebAccountProviderCommand providerCommand = new WebAccountProviderCommand(provider, WebAccountProviderCommandInvoked);
                e.WebAccountProviderCommands.Add(providerCommand);
            }
        }

        private async Task<WebAccount> GetWebAccount()
        {
            String accountID = ApplicationData.Current.LocalSettings.Values[StoredAccountIdKey] as String;
            String providerID = ApplicationData.Current.LocalSettings.Values[StoredProviderIdKey] as String;
            String authority = ApplicationData.Current.LocalSettings.Values[StoredAuthorityKey] as String;

            WebAccount account;

            WebAccountProvider provider = await GetProvider(providerID);

            if (providerID == AppSpecificProviderId)
            {
                account = new WebAccount(provider, accountID, WebAccountState.None);
            }
            else
            {
                account = await WebAuthenticationCoreManager.FindAccountAsync(provider, accountID);

                // The account has been deleted if FindAccountAsync returns null
                if (account == null)
                {
                    RemoveAccountData();
                }
            }

            return account;
        }

        private async Task AddWebAccountToPane(AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            WebAccount account = await GetWebAccount();
   
            WebAccountCommand command = new WebAccountCommand(account, WebAccountInvoked, SupportedWebAccountActions.Remove | SupportedWebAccountActions.Manage);
            e.WebAccountCommands.Add(command);
        }

        private async Task<WebAccountProvider> GetProvider(string ProviderID, string AuthorityId = "")
        {
            if (ProviderID == AppSpecificProviderId)
            {
                return new WebAccountProvider(AppSpecificProviderId, AppSpecificProviderName, new Uri(this.BaseUri, "Assets/smallTile-sdk.png"));
            }
            else
            {
                return await WebAuthenticationCoreManager.FindAccountProviderAsync(ProviderID, AuthorityId);
            }
        }

        private void OtherLinkInvoked(IUICommand command)
        {
            rootPage.NotifyUser("Other link pressed by user", NotifyType.StatusMessage);
        }

        private void PrivacyPolicyInvoked(IUICommand command)
        {
            rootPage.NotifyUser("Privacy policy clicked by user", NotifyType.StatusMessage);
        }

        private void WebAccountProviderCommandInvoked(WebAccountProviderCommand command)
        {
            if ((command.WebAccountProvider.Id == MicrosoftProviderId) && (command.WebAccountProvider.Authority == MicrosoftAccountAuthority))
            {
                // ClientID is ignored by MSA
                AuthenticateWithRequestToken(command.WebAccountProvider, MicrosoftAccountScopeRequested, MicrosoftAccountClientId);
            }
            else if ((command.WebAccountProvider.Id == MicrosoftProviderId) && (command.WebAccountProvider.Authority == AzureActiveDirectoryAuthority))
            {
                AuthenticateWithRequestToken(command.WebAccountProvider, AzureActiveDirectoryAuthority, AzureActiveDirectoryClientId);
            }
            else if (command.WebAccountProvider.Id == AppSpecificProviderId)
            {
                // Show user registration/login for your app specific account type.
                // Store the user if registration/login successful
                StoreNewAccountDataLocally(new WebAccount(command.WebAccountProvider, "App Specific User", WebAccountState.None));
            }
        }
        
        public async void AuthenticateWithRequestToken(WebAccountProvider Provider, String Scope, String ClientID)
        {
            try
            {
                WebTokenRequest webTokenRequest = new WebTokenRequest(Provider, Scope, ClientID);

                // Azure Active Directory requires a resource to return a token
                if(Provider.Id == "https://login.microsoft.com" && Provider.Authority == "organizations")
                {
                    webTokenRequest.Properties.Add("resource", "https://graph.windows.net");
                }

                // If the user selected a specific account, RequestTokenAsync will return a token for that account.
                // The user may be prompted for credentials or to authorize using that account with your app
                // If the user selected a provider, the user will be prompted for credentials to login to a new account
                    WebTokenRequestResult webTokenRequestResult = await WebAuthenticationCoreManager.RequestTokenAsync(webTokenRequest);

                if (webTokenRequestResult.ResponseStatus == WebTokenRequestStatus.Success)
                {
                    StoreNewAccountDataLocally(webTokenRequestResult.ResponseData[0].WebAccount);
                }

                OutputTokenResult(webTokenRequestResult);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Web Token request failed: " + ex, NotifyType.ErrorMessage);
            }
        }

        public void StoreNewAccountDataLocally(WebAccount account)
        {
            if (account.Id != "")
            {
                ApplicationData.Current.LocalSettings.Values["AccountID"] = account.Id;
            }
            else
            {
                // It's a custom account
                ApplicationData.Current.LocalSettings.Values["AccountID"] = account.UserName;
            }


            ApplicationData.Current.LocalSettings.Values["ProviderID"] = account.WebAccountProvider.Id;
            if (account.WebAccountProvider.Authority != null)
            {
                ApplicationData.Current.LocalSettings.Values["Authority"] = account.WebAccountProvider.Authority;
            }
            else
            {
                ApplicationData.Current.LocalSettings.Values["Authority"] = "";
            }
        }

        public void OutputTokenResult(WebTokenRequestResult result)
        {
            if (result.ResponseStatus == WebTokenRequestStatus.Success)
            {
                rootPage.NotifyUser("Web Token request successful for user:" + result.ResponseData[0].WebAccount.UserName, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Web Token request error: " + result.ResponseStatus + " Code: " + result.ResponseError.ErrorMessage, NotifyType.StatusMessage);
            }
        }

        private async void WebAccountInvoked(WebAccountCommand command, WebAccountInvokedArgs args)
        {
            if (args.Action == WebAccountAction.Remove)
            {
                rootPage.NotifyUser("Removing account", NotifyType.StatusMessage);
                await LogoffAndRemoveAccount();
            }
            else if (args.Action == WebAccountAction.Manage)
            {
                // Display user management UI for this account
                rootPage.NotifyUser("Managing account", NotifyType.StatusMessage);
            }
        }

        private bool HasAccountStored()
        {
            return (ApplicationData.Current.LocalSettings.Values.ContainsKey(StoredAccountIdKey) &&
                ApplicationData.Current.LocalSettings.Values.ContainsKey(StoredProviderIdKey) &&
                ApplicationData.Current.LocalSettings.Values.ContainsKey(StoredAuthorityKey));
        }

        private async Task LogoffAndRemoveAccount()
        {
            if (HasAccountStored())
            {
                WebAccount account = await GetWebAccount();

                // Check if the account has been deleted already by Token Broker
                if (account != null)
                { 
                    if (account.WebAccountProvider.Id == AppSpecificProviderId)
                    {
                        // perform actions to sign out of the app specific provider
                    }
                    else
                    {
                        await account.SignOutAsync();
                    }
                }
                account = null;
                RemoveAccountData();
            }
        }

        private void RemoveAccountData()
        {
            ApplicationData.Current.LocalSettings.Values.Remove(StoredAccountIdKey);
            ApplicationData.Current.LocalSettings.Values.Remove(StoredProviderIdKey);
            ApplicationData.Current.LocalSettings.Values.Remove(StoredAuthorityKey);
        }

    }
}
