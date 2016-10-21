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
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Services.Store;
using Windows.UI.Xaml.Controls;
using Windows.Web.Http;
using Windows.Web.Http.Headers;

namespace SDKTemplate
{
    /// <summary>
    /// For more details on the business to business APIs, see:
    /// https://msdn.microsoft.com/en-us/library/windows/apps/mt609002.aspx
    /// </summary>
    public sealed partial class Scenario7_B2B : Page
    {
        private MainPage rootPage = MainPage.Current;
        private StoreContext storeContext = StoreContext.GetDefault();

        // Note: "tenant", "clientId", "clientSecret" and "appIdUri" will need to be updated based on your particular
        // Azure Active Directory configuration. See the README for more information.
        // https://msdn.microsoft.com/en-us/library/azure/dn645542.aspx
        readonly string tenantId = "00000000-0000-0000-0000-000000000000";
        readonly string clientId = "00000000-0000-0000-0000-000000000000";
        readonly string clientSecret = "00000000000000000000000000000000000000000000";
        readonly string appIdUri = "https://contoso.onmicrosoft.com";

        public Scenario7_B2B()
        {
            this.InitializeComponent();
        }

        private async void GetCustomerCollectionsId()
        {
            string aadToken = await GetTokenFromAzureOAuthAsync();
            if (!String.IsNullOrEmpty(aadToken))
            {
                string id = await storeContext.GetCustomerCollectionsIdAsync(aadToken, "kim@example.com");
                Output.Text = id;
                if (String.IsNullOrEmpty(id))
                {
                    rootPage.NotifyUser("GetCustomerCollectionsIdAsync failed.", NotifyType.ErrorMessage);
                }
            }
        }

        private async void GetCustomerPurchaseId()
        {
            String aadToken = await GetTokenFromAzureOAuthAsync();
            if (!String.IsNullOrEmpty(aadToken))
            {
                string id = await storeContext.GetCustomerPurchaseIdAsync(aadToken, "kim@example.com");
                Output.Text = id;
                if (String.IsNullOrEmpty(id))
                {
                    rootPage.NotifyUser("GetCustomerPurchaseIdAsync failed.", NotifyType.ErrorMessage);
                }
            }
        }

        private async Task<string> GetTokenFromAzureOAuthAsync()
        {
            var content = new HttpFormUrlEncodedContent(new Dictionary<string, string>
            {
                { "grant_type", "client_credentials" },
                { "client_id", clientId },
                { "client_secret", clientSecret },
                { "resource", appIdUri },
            });
            content.Headers.ContentType = new HttpMediaTypeHeaderValue("application/x-www-form-urlencoded");
            try
            {
                HttpClient client = new HttpClient();
                HttpResponseMessage result = await client.PostAsync(
                    new Uri($"https://login.microsoftonline.com/{tenantId}/oauth2/token"), content);

                string responseString = await result.Content.ReadAsStringAsync();
                JsonValue response = JsonValue.Parse(responseString);

                return response.GetObject().GetNamedString("access_token");
            }
            catch (Exception ex)
            {
                // Networking errors in PostAsync are reported as exceptions.
                // JSON parsing errors are also reported as exceptions.
                Output.Text = $"Failed to load Azure OAuth Token: {ex}";
                return String.Empty;
            }
        }
    }
}
