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
using Windows.UI.Xaml.Media;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Accounts
{
    // Important Note for running this sample:
    // The sample as-is will not be able to get tokens without having it's app manifest being 
    // modified to use the App Identity of a registered Microsoft Store/registered AAD app. 
    //
    // See 'Related Topics' in the README.md for instructions on how to register an app.

    /// <summary>
    /// This scenario covers getting the default account.
    ///
    /// First you must get the default account provider by passing the id "https://login.windows.local"
    /// into WebAuthenticationCoreManager::FindAccountProviderAsync.
    ///
    /// It's important to note whether this account is MSA or AAD depending on the authority that the
    /// provider has.
    ///    MSA: authority == "consumers"
    ///    AAD: authority == "organizations"
    /// These will determine what strings you pass into the token request.
    ///
    /// Then you can call WebAuthenticationCoreManager::RequestTokenSilentlyAsync or 
    /// WebAuthenticationCoreManager::RequestTokenAsync to get a token for the user.
    ///
    /// The main difference between these APIs is that if the user needs to enter login
    /// credentials before a token can be returned, RequestTokenAsync will result in 
    /// a UI popping up while RequestTokenSilentlyAsync will just return a result of
    /// UserInteractionRequired. 
    ///
    /// If you hit UserInteractionRequired, then you will need to call the regular
    /// RequestTokenAsync.
    /// </summary>

    public sealed partial class DefaultAccountScenario : Page
    {
        private MainPage rootPage;

        const string DefaultProviderId = "https://login.windows.local";
        const string MicrosoftProviderId = "https://login.microsoft.com";
        const string MicrosoftAccountAuthority = "consumers";
        const string AzureActiveDirectoryAuthority = "organizations";

        const string MicrosoftAccountClientId = "none";
        const string MicrosoftAccountScopeRequested = "wl.basic";

        // To obtain azureAD tokens, you must register this app on the AzureAD portal, and obtain the client ID
        const string AzureActiveDirectoryClientId = "";
        const string AzureActiveDirectoryScopeRequested = "";

        WebAccountProvider defaultProvider;
        WebAccount defaultAccount;

        public DefaultAccountScenario()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            IdentityChecker.SampleIdentityConfigurationCorrect(NotRegisteredWarning, AzureActiveDirectoryClientId);
        } 

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {

        }

        private async void Button_DefaultSignIn_Click(object sender, RoutedEventArgs e)
        {
            defaultProvider = await WebAuthenticationCoreManager.FindAccountProviderAsync(DefaultProviderId);

            if(defaultProvider == null)
            {
                rootPage.NotifyUser("This user does not have a primary account", NotifyType.StatusMessage);
            }
            else
            {
                // Set the clientID and scope based on the authority. The authority tells us if the account is a Microsoft account or an Azure AD.
                String scope = defaultProvider.Authority == MicrosoftAccountAuthority ? MicrosoftAccountScopeRequested : AzureActiveDirectoryScopeRequested;
                String clientID = defaultProvider.Authority == MicrosoftAccountAuthority ? MicrosoftAccountClientId : AzureActiveDirectoryClientId;

                AuthenticateWithRequestToken(defaultProvider, scope, clientID);
            }
        }

        private async void Button_GetTokenSilently_Click(object sender, RoutedEventArgs e)
        {
            if(defaultAccount == null || defaultProvider == null)
            {
                rootPage.NotifyUser("No account known to get account silently", NotifyType.ErrorMessage);
            }
            else
            {
                await GetTokenSilent(defaultProvider, defaultAccount);
            }
        }

        private void Button_Reset_Click(object sender, RoutedEventArgs e)
        {
            defaultAccount = null;
            defaultProvider = null;
            button_GetTokenSilently.IsEnabled = false;
            textblock_SignedInStatus.Text = "Not signed in";
            textblock_SignedInStatus.Foreground = new SolidColorBrush(Windows.UI.Colors.Red);
            listview_SignedInAccounts.Items.Clear();

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
                    defaultAccount = webTokenRequestResult.ResponseData[0].WebAccount;

                    button_GetTokenSilently.IsEnabled = true;
                    textblock_SignedInStatus.Text = "Signed in with:";
                    textblock_SignedInStatus.Foreground = new SolidColorBrush(Windows.UI.Colors.Green);
                    listview_SignedInAccounts.Items.Clear();
                    listview_SignedInAccounts.Items.Add(defaultAccount.Id);
                }

                OutputTokenResult(webTokenRequestResult);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Web Token request failed: " + ex, NotifyType.ErrorMessage);
            }
        }

        private async Task GetTokenSilent(WebAccountProvider Provider, WebAccount Account)
        {
            // Set the clientID and scope based on the authority. The authority tells us if the account is a Microsoft account or an Azure AD.
            String scope = defaultProvider.Authority == MicrosoftAccountAuthority ? MicrosoftAccountScopeRequested : AzureActiveDirectoryScopeRequested;
            String clientID = defaultProvider.Authority == MicrosoftAccountAuthority ? MicrosoftAccountClientId : AzureActiveDirectoryClientId;

            WebTokenRequest webTokenRequest = new WebTokenRequest(Provider, scope, clientID);

            // Azure Active Directory requires a resource to return a token
            if (Provider.Authority == "organizations")
            {
                webTokenRequest.Properties.Add("resource", "https://graph.windows.net");
            }

            WebTokenRequestResult webTokenRequestResult = await WebAuthenticationCoreManager.GetTokenSilentlyAsync(webTokenRequest, Account);

            if (webTokenRequestResult.ResponseStatus == WebTokenRequestStatus.Success)
            {
                rootPage.NotifyUser("Silent request successful for user:" + webTokenRequestResult.ResponseData[0].WebAccount.UserName, NotifyType.StatusMessage);
            }
            else if (webTokenRequestResult.ResponseStatus == WebTokenRequestStatus.UserInteractionRequired)
            {
                rootPage.NotifyUser("Cannot retrieve token silently because user interaction is required to complete this request.", NotifyType.ErrorMessage);
            }
            else
            {
                rootPage.NotifyUser("Web Token request error: " + webTokenRequestResult.ResponseStatus + " Code: " + webTokenRequestResult.ResponseError.ErrorMessage, NotifyType.StatusMessage);
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

    }
}
