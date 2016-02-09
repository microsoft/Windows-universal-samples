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
            ThreadNetworkContext context = null;
            IdentityBox.Text = String.Empty;

            try
            {
                rootPage.NotifyUser("Creating file.........", NotifyType.StatusMessage);

                StorageFile resultFile = await ApplicationData.Current.LocalFolder.CreateFileAsync(
                    "ResultFile.txt",
                    CreationCollisionOption.GenerateUniqueName);

                if (!Uri.TryCreate(EnterpriseUri.Text.Trim(), UriKind.Absolute, out resourceUri))
                {
                    rootPage.NotifyUser("Invalid URI, please re-enter a valid URI", NotifyType.ErrorMessage);
                    return;
                }
                resourceIdentity = await ProtectionPolicyManager.GetPrimaryManagedIdentityForNetworkEndpointAsync(
                                           new HostName(resourceUri.Host));

                // if resourceIdentity is empty or Null, then it is considered personal
                IdentityBox.Text = resourceIdentity;

                if (!string.IsNullOrEmpty(resourceIdentity))
                {
                    context = ProtectionPolicyManager.CreateCurrentThreadNetworkContext(resourceIdentity);
                }

                // Access enterprise content

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
    }
}
