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

using SDKTemplate;
using System;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Foundation;
using Windows.Security.Authentication.Web;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Web.Http;

namespace WebAuthentication
{
    public sealed partial class Scenario1_Facebook : Page
    {
        private MainPage rootPage = MainPage.Current;
        bool authzInProgress = false;

        public Scenario1_Facebook()
        {
            this.InitializeComponent();

            // Use these SIDs to register the app with Facebook.
            WindowsStoreSidTextBlock.Text = WebAuthenticationBroker.GetCurrentApplicationCallbackUri().Host;
            WindowsPhoneStoreSidTextBlock.Text = "feaebe20-b974-4857-a51c-3525e4dfe2a8"; // copied from Package.appxmanifest
        }

        private async void Launch_Click(object sender, RoutedEventArgs e)
        {
            if (authzInProgress)
            {
                return;
            }

            FacebookReturnedToken.Text = "";
            FacebookUserName.Text = "";

            if (String.IsNullOrEmpty(FacebookClientID.Text))
            {
                rootPage.NotifyUser("Please enter a Client ID.", NotifyType.StatusMessage);
                return;
            }

            Uri callbackUri;
            if (!Uri.TryCreate(FacebookCallbackUrl.Text, UriKind.Absolute, out callbackUri))
            {
                rootPage.NotifyUser("Please enter a Callback URL.", NotifyType.StatusMessage);
                return;
            }

            Uri facebookStartUri = new Uri($"https://www.facebook.com/dialog/oauth?client_id={Uri.EscapeDataString(FacebookClientID.Text)}&redirect_uri={Uri.EscapeDataString(callbackUri.AbsoluteUri)}&scope=email&display=popup&response_type=token");

            rootPage.NotifyUser($"Navigating to {facebookStartUri}", NotifyType.StatusMessage);

            authzInProgress = true;
            try
            {
                WebAuthenticationResult WebAuthenticationResult = await WebAuthenticationBroker.AuthenticateAsync(WebAuthenticationOptions.None, facebookStartUri, callbackUri);
                if (WebAuthenticationResult.ResponseStatus == WebAuthenticationStatus.Success)
                {
                    FacebookReturnedToken.Text = WebAuthenticationResult.ResponseData;
                    await GetFacebookUserNameAsync(WebAuthenticationResult.ResponseData);
                }
                else if (WebAuthenticationResult.ResponseStatus == WebAuthenticationStatus.ErrorHttp)
                {
                    FacebookReturnedToken.Text = $"HTTP error: {WebAuthenticationResult.ResponseErrorDetail}";
                }
                else
                {
                    FacebookReturnedToken.Text = $"Error: {WebAuthenticationResult.ResponseStatus}";
                }

            }
            catch (Exception Error)
            {
                rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
            }

            authzInProgress = false;
        }

        /// <summary>
        /// This function extracts access_token from the response returned from web authentication broker
        /// and uses that token to get user information using facebook graph api. 
        /// </summary>
        /// <param name="responseData">responseData returned from AuthenticateAsync result.</param>
        private async Task GetFacebookUserNameAsync(string responseData)
        {
            var decoder = new WwwFormUrlDecoder(responseData);
            var error = decoder.TryGetValue("error");
            if (error != null)
            {
                FacebookUserName.Text = $"Error: {error}";
                return;
            }

            // You can store access token locally for further use.
            string access_token = decoder.GetFirstValueByName("access_token");
            string expires_in = decoder.TryGetValue("expires_in"); // expires_in is optional

            HttpClient httpClient = new HttpClient();
            Uri uri = new Uri($"https://graph.facebook.com/me?access_token={Uri.EscapeDataString(access_token)}");

            HttpGetStringResult result = await httpClient.TryGetStringAsync(uri);
            if (result.Succeeded)
            {
                JsonObject userInfo = JsonObject.Parse(result.Value).GetObject();
                FacebookUserName.Text = userInfo.GetNamedString("name");
            }
            else
            {
                FacebookUserName.Text = "Error contacting Facebook";
            }
        }
    }
}
