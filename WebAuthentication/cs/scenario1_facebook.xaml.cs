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
                String FacebookURL = "https://www.facebook.com/dialog/oauth?client_id=" + Uri.EscapeDataString(FacebookClientID.Text) + "&redirect_uri=" + Uri.EscapeDataString(FacebookCallbackUrl.Text) + "&scope=read_stream&display=popup&response_type=token";

                System.Uri StartUri = new Uri(FacebookURL);
                System.Uri EndUri = new Uri(FacebookCallbackUrl.Text);

                rootPage.NotifyUser("Navigating to: " + FacebookURL, NotifyType.StatusMessage);

                WebAuthenticationResult WebAuthenticationResult = await WebAuthenticationBroker.AuthenticateAsync(WebAuthenticationOptions.None, StartUri, EndUri);
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
            catch (Exception Error)
            {
                rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
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
            string responseData = webAuthResultResponseData.Substring(webAuthResultResponseData.IndexOf("access_token"));
            String[] keyValPairs = responseData.Split('&');
            string access_token = null;
            string expires_in = null;
            for (int i = 0; i < keyValPairs.Length; i++)
            {
                String[] splits = keyValPairs[i].Split('=');
                switch (splits[0])
                {
                    case "access_token":
                        access_token = splits[1]; //you may want to store access_token for further use. Look at Scenario 5 (Account Management).
                        break;
                    case "expires_in":
                        expires_in = splits[1];
                        break;
                }
            }

            rootPage.NotifyUser("access_token = " + access_token, NotifyType.StatusMessage);
            //Request User info.
            HttpClient httpClient = new HttpClient();
            string response = await httpClient.GetStringAsync(new Uri("https://graph.facebook.com/me?access_token=" + access_token));
            JsonObject value = JsonValue.Parse(response).GetObject();
            string facebookUserName = value.GetNamedString("name");

            rootPage.NotifyUser(facebookUserName + " is connected!", NotifyType.StatusMessage);
        }
    }
}
