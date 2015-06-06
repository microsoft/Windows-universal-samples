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
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Tasks;
using Windows.ApplicationModel.Background;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Networking.BackgroundTransfer;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace BackgroundTransfer
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
            IStorageFile resultFile = await KnownFolders.PicturesLibrary.CreateFileAsync(
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
