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
using Windows.UI.Popups;
using Windows.Security.Authentication.Web.Provider;

namespace Accounts
{
    public sealed partial class SingleMicrosoftAccountScenario : Page
    {
        private MainPage rootPage;

        // To obtain Microsoft account tokens, you must register your application online
        // Then, you must associate the app with the store.
        const string MicrosoftAccountProviderId = "https://login.microsoft.com";
        const string ConsumerAuthority = "consumers";
        const string AccountScopeRequested = "wl.basic";
        const string AccountClientId = "none";
        const string StoredAccountKey = "accountid";

        public SingleMicrosoftAccountScenario()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // The AccountCommandsRequested event triggers before the Accounts settings pane is displayed 
            AccountsSettingsPane.GetForCurrentView().AccountCommandsRequested += OnAccountCommandsRequested;

            IdentityChecker.SampleIdentityConfigurationCorrect(NotRegisteredWarning);
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


        // This event handler is called when the Account settings pane is to be launched.
        private async void OnAccountCommandsRequested(
            AccountsSettingsPane sender,
            AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            // In order to make async calls within this callback, the deferral object is needed
            AccountsSettingsPaneEventDeferral deferral = e.GetDeferral();

            // This scenario only lets the user have one account at a time.
            // If there already is an account, we do not include a provider in the list
            // This will prevent the add account button from showing up.
            bool isPresent = ApplicationData.Current.LocalSettings.Values.ContainsKey(StoredAccountKey);

            if (isPresent)
            {
                await AddWebAccount(e);
            }
            else
            {
                await AddWebAccountProvider(e);
            }

            AddLinksAndDescription(e);

            deferral.Complete();
        }


        private async Task AddWebAccountProvider(AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            // FindAccountProviderAsync returns the WebAccountProvider of an installed plugin
            // The Provider and Authority specifies the specific plugin
            // This scenario only supports Microsoft accounts.

            // The Microsoft account provider is always present in Windows 10 devices, as is the Azure AD plugin.
            // If a non-installed plugin or incorect identity is specified, FindAccountProviderAsync will return null   
            WebAccountProvider provider = await WebAuthenticationCoreManager.FindAccountProviderAsync(MicrosoftAccountProviderId, ConsumerAuthority);

            WebAccountProviderCommand providerCommand = new WebAccountProviderCommand(provider, WebAccountProviderCommandInvoked);
            e.WebAccountProviderCommands.Add(providerCommand);
        }

        private async Task AddWebAccount(AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            WebAccountProvider provider = await WebAuthenticationCoreManager.FindAccountProviderAsync(MicrosoftAccountProviderId, ConsumerAuthority);

            String accountID = (String)ApplicationData.Current.LocalSettings.Values[StoredAccountKey];
            WebAccount account = await WebAuthenticationCoreManager.FindAccountAsync(provider, accountID);

            if (account == null)
            {
                // The account has most likely been deleted in Windows settings
                // Unless there would be significant data loss, you should just delete the account
                // If there would be significant data loss, prompt the user to either re-add the account, or to remove it
                ApplicationData.Current.LocalSettings.Values.Remove(StoredAccountKey);
            }

            WebAccountCommand command = new WebAccountCommand(account, WebAccountInvoked, SupportedWebAccountActions.Remove);
            e.WebAccountCommands.Add(command);
        }

        private void AddLinksAndDescription(AccountsSettingsPaneCommandsRequestedEventArgs e)
        {
            e.HeaderText = "Describe what adding an account to your application will do for the user";

            // You can add links such as privacy policy, help, general account settings
            e.Commands.Add(new SettingsCommand("privacypolicy", "Privacy policy", PrivacyPolicyInvoked));
            e.Commands.Add(new SettingsCommand("otherlink", "Other link", OtherLinkInvoked));
        }

        private async void WebAccountProviderCommandInvoked(WebAccountProviderCommand command)
        {
            // ClientID is ignored by MSA
            await RequestTokenAndSaveAccount(command.WebAccountProvider, AccountScopeRequested, AccountClientId);
        }

        private async void WebAccountInvoked(WebAccountCommand command, WebAccountInvokedArgs args)
        {
            if (args.Action == WebAccountAction.Remove)
            {
                rootPage.NotifyUser("Removing account", NotifyType.StatusMessage);
                await LogoffAndRemoveAccount();
            }
        }

        private void PrivacyPolicyInvoked(IUICommand command)
        {
            rootPage.NotifyUser("Privacy policy clicked by user", NotifyType.StatusMessage);
        }

        private void OtherLinkInvoked(IUICommand command)
        {
            rootPage.NotifyUser("Other link pressed by user", NotifyType.StatusMessage);
        }

        private async Task RequestTokenAndSaveAccount(WebAccountProvider Provider, String Scope, String ClientID)
        {
            try
            {
                WebTokenRequest webTokenRequest = new WebTokenRequest(Provider, Scope, ClientID);
                rootPage.NotifyUser("Requesting Web Token", NotifyType.StatusMessage);
         
                // If the user selected a specific account, RequestTokenAsync will return a token for that account.
                // The user may be prompted for credentials or to authorize using that account with your app
                // If the user selected a provider, the user will be prompted for credentials to login to a new account
                WebTokenRequestResult webTokenRequestResult = await WebAuthenticationCoreManager.RequestTokenAsync(webTokenRequest);

                // If a token was successfully returned, then store the WebAccount Id into local app data
                // This Id can be used to retrieve the account whenever needed. To later get a token with that account
                // First retrieve the account with FindAccountAsync, and include that webaccount 
                // as a parameter to RequestTokenAsync or RequestTokenSilentlyAsync
                if (webTokenRequestResult.ResponseStatus == WebTokenRequestStatus.Success)
                {
                    ApplicationData.Current.LocalSettings.Values.Remove(StoredAccountKey);

                    ApplicationData.Current.LocalSettings.Values[StoredAccountKey] = webTokenRequestResult.ResponseData[0].WebAccount.Id;
                }

                OutputTokenResult(webTokenRequestResult);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Web Token request failed: " + ex.Message, NotifyType.ErrorMessage);
            }
        }

        private void OutputTokenResult(WebTokenRequestResult result)
        {
            if (result.ResponseStatus == WebTokenRequestStatus.Success)
            {
                rootPage.NotifyUser("Web Token request successful for user: " + result.ResponseData[0].WebAccount.UserName, NotifyType.StatusMessage);
                SignInButton.Content = "Account";
            }
            else
            {
                rootPage.NotifyUser("Web Token request error: " + result.ResponseError, NotifyType.StatusMessage);
            }
        }

        private async Task LogoffAndRemoveAccount()
        {
            if (ApplicationData.Current.LocalSettings.Values.ContainsKey(StoredAccountKey))
            {
                WebAccountProvider providertoDelete = await WebAuthenticationCoreManager.FindAccountProviderAsync(MicrosoftAccountProviderId, ConsumerAuthority);

                WebAccount accountToDelete = await WebAuthenticationCoreManager.FindAccountAsync(providertoDelete, (string)ApplicationData.Current.LocalSettings.Values[StoredAccountKey]);

                if(accountToDelete != null)
                {
                    await accountToDelete.SignOutAsync();
                }

                ApplicationData.Current.LocalSettings.Values.Remove(StoredAccountKey);

                SignInButton.Content = "Sign in";
            }
        }
    }
}
