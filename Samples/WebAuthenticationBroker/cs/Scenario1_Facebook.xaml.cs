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
using Windows.Security.Authentication.Web;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Web.Http;

namespace WebAuthentication
{
    public sealed partial class Scenario1_Facebook : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_Facebook()
        {
            this.InitializeComponent();
        }

        private void OutputToken(String TokenUri)
        {
            FacebookReturnedToken.Text = TokenUri;
        }

        private async void Launch_Click(object sender, RoutedEventArgs e)
        {
            if (FacebookClientID.Text == "")
            {
                rootPage.NotifyUser("Please enter an Client ID.", NotifyType.StatusMessage);
            }
            else if (FacebookCallbackUrl.Text == "")
            {
                rootPage.NotifyUser("Please enter an Callback URL.", NotifyType.StatusMessage);
            }

            try
            {
                var facebookURL = "https://www.facebook.com/dialog/oauth?client_id=" + Uri.EscapeDataString(FacebookClientID.Text) + "&redirect_uri=" + Uri.EscapeDataString(FacebookCallbackUrl.Text) + "&scope=read_stream&display=popup&response_type=token";

                var startUri = new Uri(facebookURL);
                var endUri = new Uri(FacebookCallbackUrl.Text);

                rootPage.NotifyUser($"Navigating to: {facebookURL}", NotifyType.StatusMessage);

                WebAuthenticationResult WebAuthenticationResult = await WebAuthenticationBroker.AuthenticateAsync(WebAuthenticationOptions.None, startUri, endUri);
                if (WebAuthenticationResult.ResponseStatus == WebAuthenticationStatus.Success)
                {
                    OutputToken(WebAuthenticationResult.ResponseData.ToString());
                    await GetFacebookUserNameAsync(WebAuthenticationResult.ResponseData.ToString());
                }
                else if (WebAuthenticationResult.ResponseStatus == WebAuthenticationStatus.ErrorHttp)
                {
                    OutputToken("HTTP Error returned by AuthenticateAsync() : " + WebAuthenticationResult.ResponseErrorDetail.ToString());
                }
                else
                {
                    OutputToken("Error returned by AuthenticateAsync() : " + WebAuthenticationResult.ResponseStatus.ToString());
                }

            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This function extracts access_token from the response returned from web authentication broker
        /// and uses that token to get user information using facebook graph api. 
        /// </summary>
        /// <param name="webAuthResultResponseData">responseData returned from AuthenticateAsync result.</param>
        private async Task GetFacebookUserNameAsync(string webAuthResultResponseData)
        {
            //Get Access Token first
            var responseData = webAuthResultResponseData.Substring(webAuthResultResponseData.IndexOf("access_token"));
            var keyValPairs = responseData.Split('&');
            string accessToken = null;
            string expiresIn = null;
            foreach (var pair in keyValPairs)
            {
                var splits = pair.Split('=');
                switch (splits[0])
                {
                    case "access_token":
                        accessToken = splits[1]; //you may want to store access_token for further use. Look at Scenario 5 (Account Management).
                        break;
                    case "expires_in":
                        expiresIn = splits[1];
                        break;
                }
            }

            rootPage.NotifyUser("access_token = " + accessToken, NotifyType.StatusMessage);
            //Request User info.
            var httpClient = new HttpClient();
            var response = await httpClient.GetStringAsync(new Uri($"https://graph.facebook.com/me?access_token={accessToken}"));
            var value = JsonValue.Parse(response).GetObject();
            var facebookUserName = value.GetNamedString("name");

            rootPage.NotifyUser($"{facebookUserName} is connected!", NotifyType.StatusMessage);
        }
    }
}
