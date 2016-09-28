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
using System.Globalization;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Security.EnterpriseData;
using Windows.Networking;
using Windows.Networking.BackgroundTransfer;
using Windows.Security.Credentials;
using Windows.Storage;

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

            DiscoverActiveDownloadsAsync();
        }

        // Enumerate the downloads that were going on in the background while the app was closed.
        private async void DiscoverActiveDownloadsAsync()
        {
            List<DownloadOperation> activeDownloads = new List<DownloadOperation>();

            IReadOnlyList<DownloadOperation> downloads = null;
            try
            {
                downloads = await BackgroundDownloader.GetCurrentDownloadsAsync();
            }
            catch (Exception ex)
            {
                // There was a problem with the downloads.
                // Use BackgroundTransferError.GetStatus(ex.HResult) to convert the HResult
                // into something more useful.
                // This sample merely reports the raw error.
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }

            rootPage.NotifyUser(String.Format(CultureInfo.CurrentCulture,
                "Loading background downloads: {0}",
                downloads.Count),
                NotifyType.StatusMessage);

            if (downloads.Count > 0)
            {
                List<Task> tasks = new List<Task>();
                foreach (DownloadOperation download in downloads)
                {
                    rootPage.NotifyUser(String.Format(CultureInfo.CurrentCulture,
                        "Discovered background download: {0}, Status: {1}",
                        download.Guid,
                        download.Progress.Status),
                        NotifyType.StatusMessage);

                    // Attach completion handler.
                    var notAwait = HandleDownloadAsync(download);
                }
            }
        }

        private async void CheckUriIdentity_Click(object sender, RoutedEventArgs e)
        {
            Uri resourceUri;
            string resourceIdentity;
            try
            {
                if (!Uri.TryCreate(ResourceUriText.Text.Trim(), UriKind.Absolute, out resourceUri))
                {
                    rootPage.NotifyUser("Invalid URI, please re-enter a valid URI", NotifyType.ErrorMessage);
                    return;
                }
                resourceIdentity = await ProtectionPolicyManager.GetPrimaryManagedIdentityForNetworkEndpointAsync(
                                           new HostName(resourceUri.Host));
                DisplayNetworkIdentity(resourceIdentity);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private async void ConnectUsingBackgroundTransfer(string resourceIdentity, Uri resourceUri)
        {
            ThreadNetworkContext context = null;

            try
            {
                // For demonstrational purposes, when "Use personal context" is selected, 
                // the resourceIdentity is set to null to force personal context.

                // When using Windows.Networking.BackgroundTransfer, the app is responsible for disallowing connections
                // to enterprise owned URLs when in personal context.
                // Otherwise, the app may inadvertently access cached data for a different context.
                if (UsePersonalContext.IsChecked.Value && !string.IsNullOrEmpty(resourceIdentity))
                {
                    rootPage.NotifyUser("This app does not have access to the specified URL in personal context", NotifyType.ErrorMessage);
                    return;
                }

                rootPage.NotifyUser("Creating file.........", NotifyType.StatusMessage);

                StorageFile resultFile = await ApplicationData.Current.LocalFolder.CreateFileAsync(
                    "ResultFile.txt",
                    CreationCollisionOption.GenerateUniqueName);

                if (!string.IsNullOrEmpty(resourceIdentity))
                {
                    context = ProtectionPolicyManager.CreateCurrentThreadNetworkContext(resourceIdentity);
                }

                rootPage.NotifyUser("Accessing network resource.........", NotifyType.StatusMessage);

                BackgroundDownloader downloader = new BackgroundDownloader();

                if (!string.IsNullOrEmpty(UserNameBox.Text) &&
                    !string.IsNullOrEmpty(Passwordbox.Password))
                {
                    downloader.ServerCredential = new PasswordCredential(
                        resourceUri.AbsoluteUri,
                        UserNameBox.Text,
                        Passwordbox.Password);
                }

                DownloadOperation download = downloader.CreateDownload(resourceUri, resultFile);

                await HandleDownloadAsync(download);
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

        private async void ConnectUsingWebHttpClient(string resourceIdentity, Uri resourceUri)
        {
            try
            {
                // Use Windows.Web.Http.HttpClient to access the resource.
                // The code is identical to a normal resource access, except that we create
                // a ThreadNetworkContext around the client.GetAsync() call.

                using (var client = new Windows.Web.Http.HttpClient())
                {
                    // When using Windows.Web.Http.HttpClient, the app is responsible for disallowing connections 
                    // to enterprise owned URLs when in personal context.
                    // Otherwise, the app may inadvertently access cached data for a different context.
                    if (!string.IsNullOrEmpty(resourceIdentity) && UsePersonalContext.IsChecked.Value)
                    {
                        rootPage.NotifyUser("This app does not have access to the specified URL in personal context", NotifyType.ErrorMessage);
                        return;
                    }
                    Windows.Web.Http.HttpResponseMessage response = await UsingNetworkContext(resourceIdentity, () => client.GetAsync(resourceUri));
                    rootPage.NotifyUser($"Server response = {(int)response.StatusCode} {response.ReasonPhrase}", NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                // Access to the network resource was not successful.
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private async void ConnectUsingNetHttpClient(string resourceIdentity, Uri resourceUri)
        {
            try
            {
                // Use System.Net.Http.HttpClient to access the resource.
                // The code is identical to a normal resource access, except that we specify a header
                // with the Enterprise ID that will allow us to access enterprise owned URLs.
                // The "X-MS-Windows-HttpClient-EnterpriseId" header is used instead of using ThreadNetworkContext
                // because the thread that ultimately carries out the network-bound operations may be different.

                // For demonstrational purposes, when "Use personal context" is selected, 
                // the resourceIdentity is set to null to force personal context.
                if (UsePersonalContext.IsChecked.Value)
                {
                    resourceIdentity = "";
                }

                var httpHandler = new System.Net.Http.HttpClientHandler();
                httpHandler.Credentials = new System.Net.NetworkCredential(UserNameBox.Text, Passwordbox.Password);
                httpHandler.AllowAutoRedirect = true;
                using (var client = new System.Net.Http.HttpClient(httpHandler))
                {
                    var message = new System.Net.Http.HttpRequestMessage(System.Net.Http.HttpMethod.Get, resourceUri);
                    message.Headers.Add("User-Agent", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1)");
                    client.DefaultRequestHeaders.Add("X-MS-Windows-HttpClient-EnterpriseId", resourceIdentity);
                    System.Net.Http.HttpResponseMessage response = await client.SendAsync(message);
                    rootPage.NotifyUser($"Server response = {(int)response.StatusCode} {response.ReasonPhrase}", NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                // Access to the network resource was not successful.
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }
        private async Task HandleDownloadAsync(DownloadOperation download)
        {
            try
            {
                // For more advanced Background Transfer features, please take a look at the
                // BackgroundTransfer sample.
                if (download.Progress.Status == BackgroundTransferStatus.Idle)
                {
                    await download.StartAsync();
                }
                else
                {
                    await download.AttachAsync();
                }

                ResponseInformation response = download.GetResponseInformation();

                // GetResponseInformation() returns null for non-HTTP transfers (e.g., FTP).
                string statusCode = response != null ? response.StatusCode.ToString() : String.Empty;

                rootPage.NotifyUser(String.Format(CultureInfo.CurrentCulture,
                    "Successfully completed background download: {0}, Status Code: {1}",
                    download.Guid,
                    statusCode),
                    NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        // For the Windows.Web.Http API, by default, a UI dialog is presented 
        // if the outgoing request is for a resource that requires user authentication.
        // For this reason, we hide the Credentials text blocks when Windows.Web.Http is selected.
        private void ClientTypeChanged()
        {
            if (UseWindowsWebHttpClient.IsChecked.Value)
            {
                CredentialsContent.Visibility = Visibility.Collapsed;
            }
            else
            {
                CredentialsContent.Visibility = Visibility.Visible;
            }
        }

        static T UsingNetworkContext<T>(string identity, Func<T> lambda)
        {
            // The "using" statement will Dispose the object when control leaves the block.
            using (ThreadNetworkContext context = string.IsNullOrEmpty(identity)
                ? null
                : ProtectionPolicyManager.CreateCurrentThreadNetworkContext(identity))
            {
                return lambda();
            }
        }

        private async void ConnectToUri()
        {
            Uri resourceUri;

            if (!Uri.TryCreate(ResourceUriText.Text, UriKind.Absolute, out resourceUri))
            {
                rootPage.NotifyUser("Invalid URI", NotifyType.ErrorMessage);
                return;
            }

            var hostName = new HostName(resourceUri.Host);

            try
            {
                string resourceIdentity = await ProtectionPolicyManager.GetPrimaryManagedIdentityForNetworkEndpointAsync(hostName);
                // if there is no resourceIdentity, then the resource is considered personal, and no thread network context is needed.
                // Otherwise, it requires the enterprise context.
                DisplayNetworkIdentity(resourceIdentity);

                rootPage.NotifyUser("Accessing network resource...", NotifyType.StatusMessage);

                if (UseSystemNetHttpClient.IsChecked.Value)
                {
                    ConnectUsingNetHttpClient(resourceIdentity, resourceUri);
                }
                else if (UseWindowsWebHttpClient.IsChecked.Value)
                {
                    ConnectUsingWebHttpClient(resourceIdentity, resourceUri);
                }
                else if (UseBackgroundTransferButton.IsChecked.Value)
                {
                    ConnectUsingBackgroundTransfer(resourceIdentity, resourceUri);
                }
                else
                {
                    rootPage.NotifyUser("Please select a method to connect.", NotifyType.ErrorMessage);
                }
            }
            catch (Exception ex)
            {
                // Access to the network resource was not successful.
                rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private void DisplayNetworkIdentity (string resourceIdentity)
        {
            if (string.IsNullOrEmpty(resourceIdentity))
            {
                ActualEnterpriseContextTextBlock.Text = "Network resource is not enterprise protected.";
            }
            else
            {
                ActualEnterpriseContextTextBlock.Text = "Network resource's enterprise identity is " + resourceIdentity;
            }
        }
    }
}
