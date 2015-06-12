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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Security.EnterpriseData;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Net;
using System.IO;
using Windows.Networking;
using Windows.Security.Cryptography;
using Windows.Security.Cryptography.Core;
using Windows.Storage.Streams;
using System.Security.Principal;

namespace EdpSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario16_NetworkResource : Page
    {
        private MainPage rootPage;

        public Scenario16_NetworkResource()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void CheckUriIdentity_Click(object sender, RoutedEventArgs e)
        {
            Uri resourceUri;
            string resourceIdentity;
            ThreadNetworkContext context = null;

            try
            {
                if (!Uri.TryCreate(EnterpriseUri.Text.Trim(), UriKind.Absolute, out resourceUri))
                {
                    rootPage.NotifyUser("Invalid URI, please re-enter a valid URI", NotifyType.ErrorMessage);
                    return;
                }
                resourceIdentity = await ProtectionPolicyManager.GetPrimaryManagedIdentityForNetworkEndpointAsync(
                                           new HostName(resourceUri.Host));

                // if resourceIdentity is empty or Null, then it is considered personal

                if (!string.IsNullOrEmpty(resourceIdentity))
                {
                    context = ProtectionPolicyManager.CreateCurrentThreadNetworkContext(resourceIdentity);
                }

                // Access enterprise content

                rootPage.NotifyUser("Accessing network resource.........", NotifyType.StatusMessage);

                HttpClientHandler httpHandler = new HttpClientHandler();
                if (!string.IsNullOrEmpty(UserNameBox.Text) &&
                    !string.IsNullOrEmpty(DomainBox.Text) &&
                    !string.IsNullOrEmpty(Passwordbox.Password))
                {
                    httpHandler.Credentials = new NetworkCredential(UserNameBox.Text, Passwordbox.Password, DomainBox.Text);
                }
                httpHandler.AllowAutoRedirect = true;

                using (var client = new HttpClient(httpHandler))
                {
                    var message = new HttpRequestMessage(HttpMethod.Get, resourceUri);
                    message.Headers.Add("User-Agent", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1)");
                    var result = await client.SendAsync(message);
                }

                // Access to network resource was a success. If it wasn't, exception would have been thrown

                rootPage.NotifyUser("Successfully got network resource", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                if (context != null)
                {
                    context.Dispose();
                }
            }
        }
    }
}
