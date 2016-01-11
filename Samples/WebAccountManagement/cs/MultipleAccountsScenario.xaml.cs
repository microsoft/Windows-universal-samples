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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Security.Authentication.Web.Core;
using Windows.Security.Credentials;
using Windows.Storage;
using Windows.UI.ApplicationSettings;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace Accounts
{

    public sealed partial class MultipleAccountsScenario : Page
    {
        private MainPage rootPage;

        const string AppSpecificProviderId = "myproviderid";
        const string AppSpecificProviderName = "App specific provider";

        const string AccountsContainer = "accountssettingscontainer";
        const string ProviderIdSubContainer = "providers";
        const string AuthoritySubContainer = "authorities";

        const string MicrosoftProviderId = "https://login.microsoft.com";
        const string MicrosoftAccountAuthority = "consumers";
        const string AzureActiveDirectoryAuthority = "organizations";

        const string MicrosoftAccountClientId = "none";
        const string MicrosoftAccountScopeRequested = "wl.basic";

        // To obtain azureAD tokens, you must register this app on the AzureAD portal, and obtain the client ID
        const string AzureActiveDirectoryClientId = "";
        const string AzureActiveDirectoryScopeRequested = "";

        public MultipleAccountsScenario()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            CreateLocalDataContainers();

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

            rootPage.NotifyUser("Resetting ", NotifyType.ErrorMessage);

            await LogoffAndRemoveAllAccounts();

            ((Button)sender).IsEnabled = true;
        }

        private async Task<List<WebAccount>> GetAllAccounts()
        {
            List<WebAccount> accounts = new List<WebAccount>();

            ApplicationDataContainer AccountListContainer = ApplicationData.Current.LocalSettings.Containers[AccountsContainer];

            foreach (Object value in AccountListContainer.Containers[ProviderIdSubContainer].Values.Keys)
            {
                String accountID = value as String;
                String providerID = AccountListContainer.Containers[ProviderIdSubContainer].Values[accountID] as String;
                String authority = AccountListContainer.Containers[AuthoritySubContainer].Values[accountID] as String;

                WebAccountProvider provider = await GetProvider(providerID, authority);

                if (providerID == AppSpecificProviderId)
                {
                    accounts.Add(new WebAccount(provider, accountID, WebAccountState.None));
                }
                else
                {
                    WebAccount loadedAccount = await WebAuthenticationCoreManager.FindAccountAsync(provider, accountID);
                    if(loadedAccount != null)
                    {
                        accounts.Add(loadedAccount);
                    }
                    else
                    {
                        // The account has been deleted
                        ApplicationDataContainer accountsContainer = ApplicationData.Current.LocalSettings.Containers[AccountsContainer];

                        accountsContainer.Containers[ProviderIdSubContainer].Values.Remove(accountID);
                        accountsContainer.Containers[AuthoritySubContainer].Values.Remove(accountID);
                    }
                }
            }

            return accounts;
        }
        private async Task<List<WebAccountProvider>> GetAllProviders()
        {
            List<WebAccountProvider> providers = new List<WebAccountProvider>();
            providers.Add(await GetProvider(MicrosoftProviderId, MicrosoftAccountAuthority));
            providers.Add(await GetProvider(MicrosoftProviderId, AzureActiveDirectoryAuthority));
            providers.Add(await GetProvider(AppSpecificProviderId));

            return providers;
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

        private async void OnAccountCommandsRequested(
            AccountsSettingsPane sender,
            AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            // In order to make async calls within this callback, the deferral object is needed
            AccountsSettingsPaneEventDeferral deferral = e.GetDeferral();

            // This scenario only supports a single account at one time.
            // If there already is an account, we do not include a provider in the list
            // This will prevent the add account button from showing up.
            await AddWebAccountProvidersToPane(e);
            await AddWebAccountsToPane(e);
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

        private void OtherLinkInvoked(IUICommand command)
        {
            rootPage.NotifyUser("Other link pressed by user", NotifyType.StatusMessage);
        }

        private void PrivacyPolicyInvoked(IUICommand command)
        {
            rootPage.NotifyUser("Privacy policy clicked by user", NotifyType.StatusMessage);
        }

        private async Task AddWebAccountProvidersToPane(AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            // The order of providers displayed is determined by the order provided to the Accounts pane
            List<WebAccountProvider> providers = await GetAllProviders();

            foreach (WebAccountProvider provider in providers)
            {
                WebAccountProviderCommand providerCommand = new WebAccountProviderCommand(provider, WebAccountProviderCommandInvoked);
                e.WebAccountProviderCommands.Add(providerCommand);
            }
        }

        private async Task AddWebAccountsToPane(AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            List<WebAccount> accounts = await GetAllAccounts();

            foreach (WebAccount account in accounts)
            {
                WebAccountCommand command = new WebAccountCommand(account, WebAccountInvoked, SupportedWebAccountActions.Remove | SupportedWebAccountActions.Manage);
                e.WebAccountCommands.Add(command);
            }
        }

        private async void WebAccountProviderCommandInvoked(WebAccountProviderCommand command)
        {
            if ((command.WebAccountProvider.Id == MicrosoftProviderId) && (command.WebAccountProvider.Authority == MicrosoftAccountAuthority))
            {
                // ClientID is ignored by MSA
                await AuthenticateWithRequestToken(command.WebAccountProvider, MicrosoftAccountScopeRequested, MicrosoftAccountClientId);
            }
            else if ((command.WebAccountProvider.Id == MicrosoftProviderId) && (command.WebAccountProvider.Authority == AzureActiveDirectoryAuthority))
            {
                await AuthenticateWithRequestToken(command.WebAccountProvider, AzureActiveDirectoryScopeRequested, AzureActiveDirectoryClientId);
            }
            else if (command.WebAccountProvider.Id == AppSpecificProviderId)
            {
                // Show user registration/login for your app specific account type.
                // Store the user if registration/login successful
                StoreNewAccountDataLocally(new WebAccount(command.WebAccountProvider, "App Specific User(" + DateTime.Now + ")", WebAccountState.None));
            }
        }

        public async Task AuthenticateWithRequestToken(WebAccountProvider Provider, String Scope, String ClientID)
        {
            try
            {
                WebTokenRequest webTokenRequest = new WebTokenRequest(Provider, Scope, ClientID);

                // Azure Active Directory requires a resource to return a token
                if (Provider.Id == "https://login.microsoft.com" && Provider.Authority == "organizations")
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
            ApplicationDataContainer accountsContainer = ApplicationData.Current.LocalSettings.Containers[AccountsContainer];

            if (account.Id != "")
            {
                accountsContainer.Containers[ProviderIdSubContainer].Values[account.Id] = account.WebAccountProvider.Id;
                accountsContainer.Containers[AuthoritySubContainer].Values[account.Id] = account.WebAccountProvider.Authority;
            }
            else
            {
                accountsContainer.Containers[ProviderIdSubContainer].Values[account.UserName] = account.WebAccountProvider.Id;
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
                rootPage.NotifyUser("Web Token request error: " + result.ResponseError, NotifyType.StatusMessage);
            }
        }

        private async void WebAccountInvoked(WebAccountCommand command, WebAccountInvokedArgs args)
        {
            if (args.Action == WebAccountAction.Remove)
            {
                await LogoffAndRemoveAccount(command.WebAccount);
            }
            else if (args.Action == WebAccountAction.Manage)
            {
                // Display user management UI for this account
                rootPage.NotifyUser("Manage invoked for user: " + command.WebAccount.UserName, NotifyType.StatusMessage);
            }
        }

        private async Task LogoffAndRemoveAccount(WebAccount account)
        {
            ApplicationDataContainer accountsContainer = ApplicationData.Current.LocalSettings.Containers[AccountsContainer];

            if(account.WebAccountProvider.Id != AppSpecificProviderId)
            {
                await account.SignOutAsync();

                accountsContainer.Containers[ProviderIdSubContainer].Values.Remove(account.Id);
                accountsContainer.Containers[AuthoritySubContainer].Values.Remove(account.Id);
            }
            else
            {
                //perform any actions needed to log off the app specific account
                accountsContainer.Containers[ProviderIdSubContainer].Values.Remove(account.UserName);
            }
        }

        private async Task LogoffAndRemoveAllAccounts()
        {
            List<WebAccount> accounts = await GetAllAccounts();

            foreach(WebAccount account in accounts)
            {
                await LogoffAndRemoveAccount(account);
            }

            accounts = null;
        }

        private void CreateLocalDataContainers()
        {
            ApplicationData.Current.LocalSettings.CreateContainer(AccountsContainer, ApplicationDataCreateDisposition.Always);
            ApplicationData.Current.LocalSettings.Containers[AccountsContainer].CreateContainer(ProviderIdSubContainer, ApplicationDataCreateDisposition.Always);
            ApplicationData.Current.LocalSettings.Containers[AccountsContainer].CreateContainer(AuthoritySubContainer, ApplicationDataCreateDisposition.Always);
        }
    }
}
