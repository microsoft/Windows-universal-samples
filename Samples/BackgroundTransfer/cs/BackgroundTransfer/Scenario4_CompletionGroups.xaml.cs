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
using System.Globalization;
using System.Threading.Tasks;
using Tasks;
using Windows.Networking.BackgroundTransfer;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario4_CompletionGroups : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;
        private int downloadsCompleted;

        public Scenario4_CompletionGroups()
        {
            this.InitializeComponent();
        }

        private async void StartDownloadsButton_Click(object sender, RoutedEventArgs e)
        {
            Uri baseUri;
            if (!Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, out baseUri))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            BackgroundDownloader downloader = CompletionGroupTask.CreateBackgroundDownloader();

            downloadsCompleted = 0;

            for (int i = 0; i < 10;  i++)
            {
                Uri uri = new Uri(baseUri, String.Format(CultureInfo.InvariantCulture, "?id={0}", i));
                DownloadOperation download = downloader.CreateDownload(uri, await CreateResultFileAsync(i));
                Task<DownloadOperation> startTask = download.StartAsync().AsTask();
                Task continueTask = startTask.ContinueWith(OnDownloadCompleted);
            }

            downloader.CompletionGroup.Enable();

            SetSubstatus("Completion group enabled.");
        }

        private async Task<IStorageFile> CreateResultFileAsync(int id)
        {
            StorageFolder picturesLibrary = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.PicturesLibrary);
            IStorageFile resultFile = await picturesLibrary.CreateFileAsync(
                String.Format(CultureInfo.InvariantCulture, "picture{0}.png", id),
                CreationCollisionOption.GenerateUniqueName);
            return resultFile;
        }

        private void SetSubstatus(string text)
        {
            var ignore = SubstatusBlock.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                SubstatusBlock.Text = text;
            });
        }

        private void OnDownloadCompleted(Task<DownloadOperation> task)
        {
            var ignore = rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(
                    String.Format(CultureInfo.InvariantCulture, "{0} downloads completed.", ++downloadsCompleted),
                    NotifyType.StatusMessage);
            });
        }
    }
}
