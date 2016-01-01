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

using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using Windows.ApplicationModel.Store;
using Windows.Data.Json;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// A page for ninth scenario.  For more details on the business to business APIs, see:
    /// https://msdn.microsoft.com/en-us/library/windows/apps/mt609002.aspx
    /// </summary>
    public sealed partial class Scenario9_B2B : Page
    {
        private MainPage rootPage = MainPage.Current;

        // Note: The methods on CurrentApp require your application to be published in the store and you must have a
        // valid license.  The best way to accomlish this is to publish your application once privately.  Then,
        // download it from the store once, and deploy over the top of it with this application after you have associated
        // it with your store application.
        const uint IAP_E_UNEXPECTED = 0x803f6107;
        const string IAP_E_UNEXPECTED_MESSAGE = "This scenario will not run correctly if you haven't associated it with an existing published application.";

        // Note: 'tenant', 'clientId', "clientSecret" and "appIdUri" will need to be updated based on your particular
        // Azure Active Directory configuration. See the README for more information.
        // https://msdn.microsoft.com/en-us/library/azure/dn645542.aspx
        readonly string tenantId = "00000000-0000-0000-0000-000000000000";
        readonly string clientId = "00000000-0000-0000-0000-000000000000";
        readonly string clientSecret = "00000000000000000000000000000000000000000000";
        readonly string appIdUri = "https://contoso.onmicrosoft.com";

        public Scenario9_B2B()
        {
            this.InitializeComponent();
        }

        private async void GetCustomerCollectionsId()
        {
            try
            {
                string aadToken = await GetTokenFromAzureOAuthAsync();
                if (!string.IsNullOrEmpty(aadToken))
                {
                    string result = await CurrentApp.GetCustomerCollectionsIdAsync(aadToken, "kim@example.com");
                    Output.Text = result;
                }
            }
            catch (Exception ex)
            {
                if ((uint)ex.HResult == IAP_E_UNEXPECTED)
                {
                    rootPage.NotifyUser($"GetCustomerCollectionsIdAsync failed: {IAP_E_UNEXPECTED_MESSAGE}", NotifyType.ErrorMessage);
                }
                else
                {
                    rootPage.NotifyUser("GetCustomerCollectionsIdAsync failed.", NotifyType.ErrorMessage);
                }
            }
        }

        private async void GetCustomerPurchaseId()
        {
            try
            {
                String aadToken = await GetTokenFromAzureOAuthAsync();

                if (!string.IsNullOrEmpty(aadToken))
                {
                    String result = await CurrentApp.GetCustomerPurchaseIdAsync(aadToken, "kim@example.com");
                    Output.Text = result;
                }
            }
            catch (Exception ex)
            {
                if ((uint)ex.HResult == IAP_E_UNEXPECTED)
                {
                    rootPage.NotifyUser($"GetCustomerPurchaseIdAsync failed: {IAP_E_UNEXPECTED_MESSAGE}", NotifyType.ErrorMessage);
                }
                else
                {
                    rootPage.NotifyUser("GetCustomerPurchaseIdAsync failed.", NotifyType.ErrorMessage);
                }
            }
        }

        private async Task<String> GetTokenFromAzureOAuthAsync()
        {
            try
            {
                HttpClient client = new HttpClient();
                HttpContent content = new FormUrlEncodedContent(new[]
                {
                    new KeyValuePair<string, string>("grant_type", "client_credentials"),
                    new KeyValuePair<string, string>("client_id", clientId),
                    new KeyValuePair<string, string>("client_secret", clientSecret),
                    new KeyValuePair<string, string>("resource", appIdUri)
                });

                content.Headers.ContentType = new MediaTypeHeaderValue("application/x-www-form-urlencoded");
                var result = await client.PostAsync(
                    new Uri($"https://login.microsoftonline.com/{tenantId}/oauth2/token"), content);

                byte[] responseBytes = await result.Content.ReadAsByteArrayAsync();
                string responseString = UTF8Encoding.UTF8.GetString(responseBytes, 0, responseBytes.Length);
                JsonValue response = JsonValue.Parse(responseString);

                return response.GetObject().GetNamedString("access_token");
            }
            catch (Exception ex)
            {
                Output.Text = $"Failed to load Azure OAuth Token: {ex}";
                return string.Empty;
            }
        }
    }
}
