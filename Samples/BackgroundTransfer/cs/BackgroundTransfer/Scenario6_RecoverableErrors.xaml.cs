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
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Networking.BackgroundTransfer;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web;

namespace SDKTemplate
{
    public sealed partial class Scenario6_RecoverableErrors : Page
    {
        private MainPage rootPage = MainPage.Current;

        // Scenario instance state.
        private DownloadOperation download = null;
        private CancellationTokenSource downloadCancellationTokenSource = new CancellationTokenSource();

        public Scenario6_RecoverableErrors()
        {
            InitializeComponent();
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
            // Note that for this specific scenario we are not interested in downloads from previous instances, so
            // we just enumerate all downloads from previous instances and cancel them immediately.
            await CancelActiveDownloadsAsync();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            downloadCancellationTokenSource.Cancel();
        }

        private async Task CancelActiveDownloadsAsync()
        {
            IReadOnlyList<DownloadOperation> downloads;
            try
            {
                downloads = await BackgroundDownloader.GetCurrentDownloadsAsync();
            }
            catch (Exception ex) when (IsWebException("Discovery error", ex))
            {
                // Don't worry if the old downloads encountered web exceptions.
                return;
            }

            // If previous sample instances/scenarios started transfers that haven't completed yet, cancel them now
            // so that we can start this scenario cleanly.
            if (downloads.Count > 0)
            {
                using (CancellationTokenSource canceledToken = new CancellationTokenSource())
                {
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
                }

                rootPage.NotifyUser($"Canceled {downloads.Count} downloads.", NotifyType.StatusMessage);
            }
        }

        private async void OnDownloadProgress(IAsyncOperationWithProgress<DownloadOperation, DownloadOperation> sender, DownloadOperation progress)
        {
            // We capture a snapshot of DownloadOperation.Progress because
            // it is is updated in real-time while the operation is ongoing.
            BackgroundDownloadProgress currentProgress = progress.Progress;

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                // Prepare the progress message to display in the UI.
                ulong percent = 100;
                if (currentProgress.TotalBytesToReceive > 0)
                {
                    percent = currentProgress.BytesReceived * 100 / currentProgress.TotalBytesToReceive;
                }

                DownloadedStatusText.Text = $"{currentProgress.Status} - {percent}% downloaded.";

                if (currentProgress.HasRestarted)
                {
                    rootPage.NotifyUser("Download has restarted.", NotifyType.StatusMessage);
                }

                if (currentProgress.Status == BackgroundTransferStatus.PausedRecoverableWebErrorStatus)
                {
                    // The only value we put in RecoverableWebErrorStatuses is WebErrorStatus.Forbidden,
                    // so that will be the only value observed here.
                    System.Diagnostics.Debug.Assert(download.CurrentWebErrorStatus == WebErrorStatus.Forbidden);

                    rootPage.NotifyUser("URL has expired.", NotifyType.ErrorMessage);

                    // The URL expired. Ask the user for information so we can get a new URL.
                    ContentDialogResult result = await ReauthorizeDialog.ShowAsync();

                    if (result == ContentDialogResult.Primary)
                    {
                        // For the purpose of this sample, we simply remove "?shouldExpire=yes" from the URL
                        // to indicate that the sample server should treat it like a new, unexpired URL.
                        string originalUrlString = download.RequestedUri.OriginalString;
                        string newUrlString = originalUrlString.Replace("?shouldExpire=yes", "");

                        rootPage.NotifyUser("Updating URL and resuming the download.", NotifyType.StatusMessage);
                        download.RequestedUri = new Uri(newUrlString);
                        download.Resume();
                    }
                    else
                    {
                        // Cancel the download.
                        downloadCancellationTokenSource.Cancel();
                        downloadCancellationTokenSource.Dispose();

                        // Re-create the CancellationTokenSource for future downloads.
                        downloadCancellationTokenSource = new CancellationTokenSource();
                    }
                }
            });
        }

        private async void StartDownload_Click(object sender, RoutedEventArgs e)
        {
            // Reset the output whenever a new download attempt begins.
            DownloadedInfoText.Text = "";
            DownloadedStatusText.Text = "";
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            Uri source;
            if (!Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, out source))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            string destination = fileNameField.Text.Trim();
            if (string.IsNullOrWhiteSpace(destination))
            {
                rootPage.NotifyUser("A local file name is required.", NotifyType.ErrorMessage);
                return;
            }

            StorageFolder picturesLibrary = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.PicturesLibrary);
            StorageFile destinationFile;
            try
            {
                destinationFile = await picturesLibrary.CreateFileAsync(destination, CreationCollisionOption.GenerateUniqueName);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser($"Error while creating file: {ex.Message}", NotifyType.ErrorMessage);
                return;
            }

            BackgroundDownloader downloader = new BackgroundDownloader();
            download = downloader.CreateDownload(source, destinationFile);

            // Opt into "random access" mode. Transfers configured this way have full support for resumable URL updates.
            // If the timestamp or file size of the updated URL is different from that of the previous URL, the download
            // will restart from scratch. Otherwise, the transfer will resume from the same position using the new URL.
            //
            // Due to OS limitations, downloads that don't opt into "random access" mode will always restart from scratch
            // whenever their URL is updated.
            download.IsRandomAccessRequired = true;

            if (configureRecoverableErrorsCheckBox.IsChecked.Value)
            {
                // Declare HTTP 403 (WebErrorStatus.Forbidden) as a recoverable error.
                download.RecoverableWebErrorStatuses.Add(WebErrorStatus.Forbidden);
            }

            DownloadedInfoText.Text = $"Downloading to {destinationFile.Name}, {download.Guid}";

            // Start the download and wait for it to complete.
            await HandleDownloadAsync();
        }

        private async Task HandleDownloadAsync()
        {
            try
            {
                startDownloadButton.IsEnabled = false;
                IAsyncOperationWithProgress<DownloadOperation, DownloadOperation> downloadOperation = download.StartAsync();
                downloadOperation.Progress += OnDownloadProgress;
                await downloadOperation.AsTask(downloadCancellationTokenSource.Token);

                rootPage.NotifyUser("Download completed successfully", NotifyType.StatusMessage);
            }
            catch (Exception ex) when (IsWebException("Execution error", ex))
            {
            }
            catch (TaskCanceledException)
            {
                // Download was canceled.
            }
            finally
            {
                download = null;
                startDownloadButton.IsEnabled = true;
            }
        }

        private bool IsWebException(string title, Exception ex)
        {
            WebErrorStatus error = BackgroundTransferError.GetStatus(ex.HResult);
            bool result = (error != WebErrorStatus.Unknown);
            string message = result ? error.ToString() : ex.Message;
            if (download == null)
            {
                rootPage.NotifyUser($"{title}: {message}", NotifyType.ErrorMessage);
            }
            else
            {
                rootPage.NotifyUser($"{download.Guid} - {title}: {message}", NotifyType.ErrorMessage);
            }

            return result;
        }
    }
}
