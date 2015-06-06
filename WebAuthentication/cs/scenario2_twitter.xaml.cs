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
using Windows.Security.Authentication.Web;
using Windows.Security.Cryptography;
using Windows.Security.Cryptography.Core;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Web.Http;
using Windows.Web.Http.Headers;

namespace WebAuthentication
{
    public sealed partial class Scenario2_Twitter : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2_Twitter()
        {
            this.InitializeComponent();
        }

        private async Task<String> SendDataAsync(String Url)
        {
            try
            {
                HttpClient httpClient = new HttpClient();
                return await httpClient.GetStringAsync(new Uri(Url));
            }
            catch (Exception Err)
            {
                rootPage.NotifyUser("Error getting data from server." + Err.Message, NotifyType.StatusMessage);
            }

            return null;
        }

        private void OutputToken(String TokenUri)
        {
            TwitterReturnedToken.Text = TokenUri;
        }

        private async void Launch_Click(object sender, RoutedEventArgs e)
        {
            if (TwitterClientID.Text == "")
            {
                rootPage.NotifyUser("Please enter an Client ID.", NotifyType.StatusMessage);
            }
            else if (TwitterCallbackUrl.Text == "")
            {
                rootPage.NotifyUser("Please enter an Callback URL.", NotifyType.StatusMessage);
            }
            else if (TwitterClientSecret.Text == "")
            {
                rootPage.NotifyUser("Please enter an Client Secret.", NotifyType.StatusMessage);
            }

            try
            {
                string oauth_token = await GetTwitterRequestTokenAsync(TwitterCallbackUrl.Text, TwitterClientID.Text);
                string TwitterUrl = "https://api.twitter.com/oauth/authorize?oauth_token=" + oauth_token;
                System.Uri StartUri = new Uri(TwitterUrl);
                System.Uri EndUri = new Uri(TwitterCallbackUrl.Text);

                WebAuthenticationResult WebAuthenticationResult = await WebAuthenticationBroker.AuthenticateAsync(WebAuthenticationOptions.None, StartUri, EndUri);
                if (WebAuthenticationResult.ResponseStatus == WebAuthenticationStatus.Success)
                {
                    OutputToken(WebAuthenticationResult.ResponseData.ToString());
                    await GetTwitterUserNameAsync(WebAuthenticationResult.ResponseData.ToString());
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
                // Bad Parameter, SSL/TLS Errors and Network Unavailable errors are to be handled here.
                rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
            }
        }

        private async Task GetTwitterUserNameAsync(string webAuthResultResponseData)
        {
            // Acquiring a access_token first
            string responseData = webAuthResultResponseData.Substring(webAuthResultResponseData.IndexOf("oauth_token"));
            string request_token = null;
            string oauth_verifier = null;
            String[] keyValPairs = responseData.Split('&');

            for (int i = 0; i < keyValPairs.Length; i++)
            {
                String[] splits = keyValPairs[i].Split('=');
                switch (splits[0])
                {
                    case "oauth_token":
                        request_token = splits[1];
                        break;
                    case "oauth_verifier":
                        oauth_verifier = splits[1];
                        break;
                }
            }

            String TwitterUrl = "https://api.twitter.com/oauth/access_token";

            string timeStamp = GetTimeStamp();
            string nonce = GetNonce();

            String SigBaseStringParams = "oauth_consumer_key=" + TwitterClientID.Text;
            SigBaseStringParams += "&" + "oauth_nonce=" + nonce;
            SigBaseStringParams += "&" + "oauth_signature_method=HMAC-SHA1";
            SigBaseStringParams += "&" + "oauth_timestamp=" + timeStamp;
            SigBaseStringParams += "&" + "oauth_token=" + request_token;
            SigBaseStringParams += "&" + "oauth_version=1.0";
            String SigBaseString = "POST&";
            SigBaseString += Uri.EscapeDataString(TwitterUrl) + "&" + Uri.EscapeDataString(SigBaseStringParams);

            String Signature = GetSignature(SigBaseString, TwitterClientSecret.Text);

            HttpStringContent httpContent = new HttpStringContent("oauth_verifier=" + oauth_verifier, Windows.Storage.Streams.UnicodeEncoding.Utf8);
            httpContent.Headers.ContentType = HttpMediaTypeHeaderValue.Parse("application/x-www-form-urlencoded");
            string authorizationHeaderParams = "oauth_consumer_key=\"" + TwitterClientID.Text + "\", oauth_nonce=\"" + nonce + "\", oauth_signature_method=\"HMAC-SHA1\", oauth_signature=\"" + Uri.EscapeDataString(Signature) + "\", oauth_timestamp=\"" + timeStamp + "\", oauth_token=\"" + Uri.EscapeDataString(request_token) + "\", oauth_version=\"1.0\"";

            HttpClient httpClient = new HttpClient();

            httpClient.DefaultRequestHeaders.Authorization = new HttpCredentialsHeaderValue("OAuth", authorizationHeaderParams);
            var httpResponseMessage = await httpClient.PostAsync(new Uri(TwitterUrl), httpContent);
            string response = await httpResponseMessage.Content.ReadAsStringAsync();

            String[] Tokens = response.Split('&');
            string oauth_token_secret = null;
            string access_token = null;
            string screen_name = null;

            for (int i = 0; i < Tokens.Length; i++)
            {
                String[] splits = Tokens[i].Split('=');
                switch (splits[0])
                {
                    case "screen_name":
                        screen_name = splits[1];
                        break;
                    case "oauth_token":
                        access_token = splits[1];
                        break;
                    case "oauth_token_secret":
                        oauth_token_secret = splits[1];
                        break;
                }
            }

            if (access_token != null)
            {
                // Store access_token for futher use. See Scenario 5 (Account Management).
            }

            if (oauth_token_secret != null)
            {
                // Store oauth_token_secret for further use. See Scenario 5 (Account Management).
            }
            if (screen_name != null)
            {
                rootPage.NotifyUser(screen_name + " is connected!", NotifyType.StatusMessage);
            }
        }

        private async Task<string> GetTwitterRequestTokenAsync(string twitterCallbackUrl, string consumerKey)
        {
            // Acquiring a request token
            string TwitterUrl = "https://api.twitter.com/oauth/request_token";

            string nonce = GetNonce();
            string timeStamp = GetTimeStamp();
            string SigBaseStringParams = "oauth_callback=" + Uri.EscapeDataString(twitterCallbackUrl);
            SigBaseStringParams += "&" + "oauth_consumer_key=" + consumerKey;
            SigBaseStringParams += "&" + "oauth_nonce=" + nonce;
            SigBaseStringParams += "&" + "oauth_signature_method=HMAC-SHA1";
            SigBaseStringParams += "&" + "oauth_timestamp=" + timeStamp;
            SigBaseStringParams += "&" + "oauth_version=1.0";
            string SigBaseString = "GET&";
            SigBaseString += Uri.EscapeDataString(TwitterUrl) + "&" + Uri.EscapeDataString(SigBaseStringParams);
            string Signature = GetSignature(SigBaseString, TwitterClientSecret.Text);

            TwitterUrl += "?" + SigBaseStringParams + "&oauth_signature=" + Uri.EscapeDataString(Signature);
            HttpClient httpClient = new HttpClient();
            string GetResponse = await httpClient.GetStringAsync(new Uri(TwitterUrl));

            string request_token = null;
            string oauth_token_secret = null;
            string[] keyValPairs = GetResponse.Split('&');

            for (int i = 0; i < keyValPairs.Length; i++)
            {
                string[] splits = keyValPairs[i].Split('=');
                switch (splits[0])
                {
                    case "oauth_token":
                        request_token = splits[1];
                        break;
                    case "oauth_token_secret":
                        oauth_token_secret = splits[1];
                        break;
                }
            }

            return request_token;
        }

        string GetNonce()
        {
            Random rand = new Random();
            int nonce = rand.Next(1000000000);
            return nonce.ToString();
        }

        string GetTimeStamp()
        {
            TimeSpan SinceEpoch = DateTime.UtcNow - new DateTime(1970, 1, 1);
            return Math.Round(SinceEpoch.TotalSeconds).ToString();
        }

        string GetSignature(string sigBaseString, string consumerSecretKey)
        {
            IBuffer KeyMaterial = CryptographicBuffer.ConvertStringToBinary(consumerSecretKey + "&", BinaryStringEncoding.Utf8);
            MacAlgorithmProvider HmacSha1Provider = MacAlgorithmProvider.OpenAlgorithm("HMAC_SHA1");
            CryptographicKey MacKey = HmacSha1Provider.CreateKey(KeyMaterial);
            IBuffer DataToBeSigned = CryptographicBuffer.ConvertStringToBinary(sigBaseString, BinaryStringEncoding.Utf8);
            IBuffer SignatureBuffer = CryptographicEngine.Sign(MacKey, DataToBeSigned);
            string Signature = CryptographicBuffer.EncodeToBase64String(SignatureBuffer);

            return Signature;
        }
    }
}
