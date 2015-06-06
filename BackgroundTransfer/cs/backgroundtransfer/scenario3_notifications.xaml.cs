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
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using SDKTemplate;
using Windows.Data.Xml.Dom;
using Windows.Networking.BackgroundTransfer;
using Windows.Storage;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web;

namespace BackgroundTransfer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    public sealed partial class Scenario3_Notifications : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        private BackgroundTransferGroup notificationsGroup;

        private Uri baseUri;

        private static int runId = 0;

        private enum ScenarioType
        {
            Toast,
            Tile
        }

        public Scenario3_Notifications()
        {
            // Use a unique group name so that no other component in the app uses the same group. The recommended way
            // is to generate a GUID and use it as group name as shown below.
            notificationsGroup = BackgroundTransferGroup.CreateGroup("{296628BF-5AE6-48CE-AA36-86A85A726B6A}");

            // When creating a group, we can optionally define the transfer behavior of transfers associated with the
            // group. A "parallel" transfer behavior allows multiple transfers in the same group to run concurrently
            // (default). A "serialized" transfer behavior allows at most one default priority transfer at a time for
            // the group.
            notificationsGroup.TransferBehavior = BackgroundTransferBehavior.Parallel;

            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            // An application must enumerate downloads when it gets started to prevent stale downloads/uploads.
            // Typically this can be done in the App class by overriding OnLaunched() and checking for
            // "args.Kind == ActivationKind.Launch" to detect an actual app launch.
            // We do it here in the sample to keep the sample code consolidated.
            // Note that for this specific scenario we're not interested in downloads from previous instances:
            // We'll just enumerate downloads from previous instances and cancel them immediately.
            await CancelActiveDownloadsAsync();
        }

        private async Task CancelActiveDownloadsAsync()
        {
            IReadOnlyList<DownloadOperation> downloads = null;
            try
            {
                // Note that we only enumerate transfers that belong to the transfer group used by this sample
                // scenario. We'll not enumerate transfers started by other sample scenarios in this app.
                downloads = await BackgroundDownloader.GetCurrentDownloadsForTransferGroupAsync(notificationsGroup);
            }
            catch (Exception ex)
            {
                if (!IsExceptionHandled("Discovery error", ex))
                {
                    throw;
                }
                return;
            }

            // If previous instances of this scenario started transfers that haven't completed yet, cancel them now
            // so that we can start this scenario cleanly.
            if (downloads.Count > 0)
            {
                CancellationTokenSource canceledToken = new CancellationTokenSource();
                canceledToken.Cancel();

                Task[] tasks = new Task[downloads.Count];
                for (int i = 0; i < downloads.Count; i++)
                {
                    tasks[i] = downloads[i].AttachAsync().AsTask(canceledToken.Token);
                }

                try
                {
                    await Task.WhenAll(tasks);
                }
                catch (TaskCanceledException)
                {
                }

                Log(String.Format(CultureInfo.CurrentCulture,
                    "Canceled {0} downloads from previous instances of this scenario.", downloads.Count));
            }

            // After cleaning up downloads from previous scenarios, enable buttons to allow the user to run the sample.
            ToastNotificationButton.IsEnabled = true;
            TileNotificationButton.IsEnabled = true;
        }

        private async void ToastNotificationButton_Click(object sender, RoutedEventArgs e)
        {
            // Create a downloader and associate all its downloads with the transfer group used for this scenario.
            BackgroundDownloader downloader = new BackgroundDownloader();
            downloader.TransferGroup = notificationsGroup;

            // Create a ToastNotification that should be shown when all transfers succeed.
            XmlDocument successToastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText01);
            successToastXml.GetElementsByTagName("text").Item(0).InnerText =
                "All three downloads completed successfully.";
            ToastNotification successToast = new ToastNotification(successToastXml);
            downloader.SuccessToastNotification = successToast;

            // Create a ToastNotification that should be shown if at least one transfer fails.
            XmlDocument failureToastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText01);
            failureToastXml.GetElementsByTagName("text").Item(0).InnerText =
                "At least one download completed with failure.";
            ToastNotification failureToast = new ToastNotification(failureToastXml);
            downloader.FailureToastNotification = failureToast;

            // Now create and start downloads for the configured BackgroundDownloader object.
            await RunDownloadsAsync(downloader, ScenarioType.Toast);
        }

        private async void TileNotificationButton_Click(object sender, RoutedEventArgs e)
        {
            // Create a downloader and associate all its downloads with the transfer group used for this scenario.
            BackgroundDownloader downloader = new BackgroundDownloader();
            downloader.TransferGroup = notificationsGroup;

            // Create a TileNotification that should be shown when all transfers succeed.
            XmlDocument successTileXml = TileUpdateManager.GetTemplateContent(
                TileTemplateType.TileSquare150x150Text03);
            XmlNodeList successTextNodes = successTileXml.GetElementsByTagName("text");
            successTextNodes.Item(0).InnerText = "All three";
            successTextNodes.Item(1).InnerText = "downloads";
            successTextNodes.Item(2).InnerText = "completed";
            successTextNodes.Item(3).InnerText = "successfully.";
            TileNotification successTile = new TileNotification(successTileXml);
            successTile.ExpirationTime = DateTime.Now.AddMinutes(10);
            downloader.SuccessTileNotification = successTile;

            // Create a TileNotification that should be shown if at least one transfer fails.
            XmlDocument failureTileXml = TileUpdateManager.GetTemplateContent(
                TileTemplateType.TileSquare150x150Text03);
            XmlNodeList failureTextNodes = failureTileXml.GetElementsByTagName("text");
            failureTextNodes.Item(0).InnerText = "At least";
            failureTextNodes.Item(1).InnerText = "one download";
            failureTextNodes.Item(2).InnerText = "completed";
            failureTextNodes.Item(3).InnerText = "with failure.";
            TileNotification failureTile = new TileNotification(failureTileXml);
            failureTile.ExpirationTime = DateTime.Now.AddMinutes(10);
            downloader.FailureTileNotification = failureTile;

            // Now create and start downloads for the configured BackgroundDownloader object.
            await RunDownloadsAsync(downloader, ScenarioType.Tile);
        }

        private async Task RunDownloadsAsync(BackgroundDownloader downloader, ScenarioType type)
        {
            // Validating the URI is required since it was received from an untrusted source (user input).
            // The URI is validated by calling Uri.TryCreate() that will return 'false' for strings that are not valid URIs.
            // Note that when enabling the text box users may provide URIs to machines on the intrAnet that require
            // the "Home or Work Networking" capability.
            if (!Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, out baseUri))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            // Use a unique ID for every button click, to help the user associate downloads of the same run
            // in the logs.
            runId++;

            DownloadOperation[] downloads = new DownloadOperation[3];

            try
            {
                // First we create three download operations: Note that we don't start downloads immediately. It is
                // important to first create all operations that should participate in the toast/tile update. Once all
                // operations have been created, we can start them.
                // If we start a download and create a second one afterwards, there is a race where the first download
                // may complete before we were able to create the second one. This would result in the toast/tile being
                // shown before we even create the second download.
                downloads[0] = await CreateDownload(downloader, 1, String.Format(CultureInfo.InvariantCulture,
                    "{0}.{1}.FastDownload.txt", type, runId));
                downloads[1] = await CreateDownload(downloader, 5, String.Format(CultureInfo.InvariantCulture,
                    "{0}.{1}.MediumDownload.txt", type, runId));
                downloads[2] = await CreateDownload(downloader, 10, String.Format(CultureInfo.InvariantCulture,
                    "{0}.{1}.SlowDownload.txt", type, runId));
            }
            catch (FileNotFoundException)
            {
                // We were unable to create the destination file.
                return;
            }

            // Once all downloads participating in the toast/tile update have been created, start them.
            Task[] downloadTasks = new Task[downloads.Length];
            for (int i = 0; i < downloads.Length; i++)
            {
                downloadTasks[i] = DownloadAsync(downloads[i]);
            }

            await Task.WhenAll(downloadTasks);
        }

        private async Task<DownloadOperation> CreateDownload(BackgroundDownloader downloader, int delaySeconds,
            string fileName)
        {
            Uri source = new Uri(baseUri, String.Format(CultureInfo.InvariantCulture, "?delay={0}", delaySeconds));

            StorageFile destinationFile;
            try
            {
                destinationFile = await KnownFolders.PicturesLibrary.CreateFileAsync(
                    fileName, CreationCollisionOption.GenerateUniqueName);
            }
            catch (FileNotFoundException ex)
            {
                rootPage.NotifyUser("Error while creating file: " + ex.Message, NotifyType.ErrorMessage);
                throw;
            }

            return downloader.CreateDownload(source, destinationFile);
        }

        private async Task DownloadAsync(DownloadOperation download)
        {
            Log(String.Format(CultureInfo.CurrentCulture, "Downloading {0}", download.ResultFile.Name));

            try
            {
                await download.StartAsync();

                LogStatus(String.Format(CultureInfo.CurrentCulture, "Downloading {0} completed.", download.ResultFile.Name), NotifyType.StatusMessage);
            }
            catch (TaskCanceledException)
            {
            }
            catch (Exception ex)
            {
                if (!IsExceptionHandled("Execution error", ex, download))
                {
                    throw;
                }
            }
        }

        private bool IsExceptionHandled(string title, Exception ex, DownloadOperation download = null)
        {
            WebErrorStatus error = BackgroundTransferError.GetStatus(ex.HResult);
            if (error == WebErrorStatus.Unknown)
            {
                return false;
            }

            if (download == null)
            {
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0}: {1}", title, error),
                    NotifyType.ErrorMessage);
            }
            else
            {
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0} - {1}: {2}", download.ResultFile.Name,
                    title, error), NotifyType.ErrorMessage);
            }

            return true;
        }

        private void Log(string message)
        {
            outputField.Text += message + "\r\n";
        }

        private void LogStatus(string message, NotifyType type)
        {
            rootPage.NotifyUser(message, type);
            Log(message);
        }
    }
}
