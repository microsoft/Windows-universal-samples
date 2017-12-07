//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Microsoft.IdentityModel.Clients.ActiveDirectory;
using System;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Embedded.DeviceLockdown;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;

namespace SDKTemplate
{
    public sealed partial class Scenario1_SignIn : Page
    {
        private MainPage rootPage = MainPage.Current;

        bool canSignOut;

        // Note: 'tenant' and 'clientId' will need to be updated based on your particular Azure Active Directory configuration.
        // See the README for more information.
        readonly string tenant = "contoso.onmicrosoft.com";
        readonly string clientId = "{...}";

        readonly Uri RedirectURI = Windows.Security.Authentication.Web.WebAuthenticationBroker.GetCurrentApplicationCallbackUri();
        readonly string resource = "https://graph.windows.net";
        readonly string apiversion = "?api-version=2013-04-05";
        readonly AuthenticationContext authenticationContext;

        public Scenario1_SignIn()
        {
            this.InitializeComponent();

            // Initialize the authentication context for Azure Authentication.
            string authority = "https://login.windows.net/" + tenant;
            authenticationContext = new AuthenticationContext(authority);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (tenant == "contoso.onmicrosoft.com" || clientId == "{...}")
            {
                // The user has not configured the sample for Azure Authentication.
                UseAzureAuthenticationCheckBox.IsEnabled = false;
                UseAzureAuthenticationCheckBox.IsChecked = false;
                rootPage.NotifyUser("This sample has not been configured for Azure Authentication.", NotifyType.ErrorMessage);
            }

            try
            {
                // If the current role is Guid.Empty, then the user is not signed in.
                Guid currentRole = DeviceLockdownProfile.GetCurrentLockdownProfile();
                if (currentRole == Guid.Empty)
                {
                    SignInStatus.Text = "You are not signed in.";
                    canSignOut = false;
                }
                else
                {
                    DeviceLockdownProfileInformation currentProfile = DeviceLockdownProfile.GetLockdownProfileInformation(currentRole);
                    SignInStatus.Text = "You are signed in as " + currentProfile.Name;
                    canSignOut = true;
                }
                SignOutButton.IsEnabled = canSignOut;
                LoadApplicationUsers();
            }
            catch (System.IO.FileNotFoundException)
            {
                rootPage.NotifyUser("Assigned Access is not configured on this device.", NotifyType.ErrorMessage);
            }
        }

        private void LoadApplicationUsers()
        {
            // Add the available roles.
            foreach (Guid roleId in DeviceLockdownProfile.GetSupportedLockdownProfiles())
            {
                DeviceLockdownProfileInformation profile = DeviceLockdownProfile.GetLockdownProfileInformation(roleId);
                UserRoles.Items.Add(new ListBoxItem() { Content = profile.Name, Tag = roleId });
            }

            // If there are roles available, then pre-select the first one and enable the Sign In button.
            if (UserRoles.Items.Count > 0)
            {
                UserRoles.SelectedIndex = 0;
                SignInButton.IsEnabled = true;
            }
        }

        private void DisableUI()
        {
            // Clear any previous error message.
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Disable all the buttons while we sign the user in or out.
            SignInButton.IsEnabled = false;
            SignOutButton.IsEnabled = false;

            // Display the progress control so the user knows that we are working.
            ProgressControl.IsActive = true;
        }

        private void EnableUI()
        {
            // Hide the progress control.
            ProgressControl.IsActive = false;

            // Restore the buttons.
            SignOutButton.IsEnabled = canSignOut;
            SignInButton.IsEnabled = true;
        }

        private async void SignIn()
        {
            DisableUI();
            await SignInAsync();
            EnableUI();
        }

        private async Task SignInAsync()
        {
            // Extract the name and role of the item the user selected.
            ListBoxItem selectedItem = (ListBoxItem)UserRoles.SelectedItem;
            string selectedName = (string)selectedItem.Content;
            Guid selectedRole = (Guid)selectedItem.Tag;

            bool canSignIn;
            if (UseAzureAuthenticationCheckBox.IsChecked.Value)
            {
                canSignIn = await AuthenticateAzureRole(selectedRole, selectedName);
            }
            else
            {
                // If the sample should use local authentication,
                // then do some app-specific authentication here.
                // For this sample, we just assume anybody can sign in as any role.
                canSignIn = true;
            }

            if (canSignIn)
            {
                // Note that successfully applying the profile will result in the termination of all running apps, including this sample.
                await DeviceLockdownProfile.ApplyLockdownProfileAsync(selectedRole);
            }
        }

        private async Task<bool> AuthenticateAzureRole(Guid selectedRole, string selectedName)
        {
            // Display the Azure authentication dialog to gather user credentials.
            AuthenticationResult authResult = await this.authenticationContext.AcquireTokenAsync(
                this.resource,
                this.clientId,
                this.RedirectURI,
                PromptBehavior.Always);

            if (authResult.Status != AuthenticationStatus.Success)
            {
                // Report the failure to the user.
                string details = string.Format("Error: {0}\nDetails: {1}", authResult.Error, authResult.ErrorDescription);
                switch (authResult.Status)
                {
                    case AuthenticationStatus.ClientError:
                        details = "Could not contact the server. Check that you have Internet access.\n" + details;
                        break;

                    case AuthenticationStatus.ServiceError:
                        details = "The server could not sign you in.\n" + details;
                        break;
                }
                rootPage.NotifyUser(details, NotifyType.ErrorMessage);
                return false;
            }

            // Formulate an http query using the graph API to determine what roles (also known as groups)
            // the user belongs to.
            HttpClient httpClient = new HttpClient();
            httpClient.DefaultRequestHeaders.Add(
                "Authorization",
                string.Format("Bearer {0}", authResult.AccessToken));
            Uri requestUri = new Uri(
                this.resource + "/" +
                this.tenant +
                "/users/" + authResult.UserInfo.DisplayableId +
                "/memberOf" + this.apiversion);

            // Perform the query.
            HttpResponseMessage httpResponse = await httpClient.GetAsync(requestUri);

            if (!httpResponse.IsSuccessStatusCode)
            {
                rootPage.NotifyUser(string.Format("Sorry, an error occured. Please sign-in again. Error: {0}", httpResponse.StatusCode), NotifyType.ErrorMessage);
                return false;
            }

            // The graph API response contains the GUIDs of all the roles the user is a member of.
            string graphResponse = await httpResponse.Content.ReadAsStringAsync();

            // See whether the user belongs to the selected role.
            // The "value" array contains a list of objects. The "objectId" tells us the role.
            try
            {
                foreach (JsonValue value in JsonObject.Parse(graphResponse).GetNamedArray("value"))
                {
                    Guid role = Guid.Parse(value.GetObject().GetNamedValue("objectId").GetString());
                    if (role == selectedRole)
                    {
                        return true;
                    }
                }
            }
            catch (Exception)
            {
                // There was a problem parsing the JSON. It wasn't in the format we expected.
                rootPage.NotifyUser("Invalid response from server.", NotifyType.ErrorMessage);
            }

            rootPage.NotifyUser(string.Format("User '{0}' is not a member of '{1}'", authResult.UserInfo.DisplayableId, selectedName), NotifyType.ErrorMessage);

            return false;
        }

        private async void SignOut()
        {
            DisableUI();
            await SignOutAsync();
            EnableUI();
        }

        private async Task SignOutAsync()
        {
            // Apply the Default role, which is represented by Guid.Empty.
            // The Default role is the one that is used when nobody is signed in.
            // Note that successfully applying the profile will result in the termination of all running apps, including this sample.
            await DeviceLockdownProfile.ApplyLockdownProfileAsync(Guid.Empty);
        }
    }
}
