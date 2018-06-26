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
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web;

namespace SDKTemplate
{
    public sealed partial class Scenario5_RandomAccess : Page
    {
        private MainPage rootPage = MainPage.Current;

        // Constants.
        private const uint BytesPerMegaByte = 1000000;

        // Scenario instance state.
        private DownloadOperation download = null;
        private IRandomAccessStreamWithContentType randomAccessStream = null;
        private Task<IBuffer> readOperation = null;
        private CancellationTokenSource readCancellationTokenSource = new CancellationTokenSource();

        public Scenario5_RandomAccess()
        {
            InitializeComponent();
        }

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
            // Cancel any outstanding reads.
            readCancellationTokenSource.Cancel();
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

        private void PauseDownload_Click(object sender, RoutedEventArgs e)
        {
            if (download != null)
            {
                download.Pause();
                pauseDownloadButton.IsEnabled = false;
                resumeDownloadButton.IsEnabled = true;
            }
        }

        private void ResumeDownload_Click(object sender, RoutedEventArgs e)
        {
            if (download != null)
            {
                download.Resume();
                pauseDownloadButton.IsEnabled = true;
                resumeDownloadButton.IsEnabled = false;
            }
        }

        private void SeekDownload_Click(object sender, RoutedEventArgs e)
        {
            // Cancel the ongoing read operation (if any).
            if (readOperation != null)
            {
                readCancellationTokenSource.Cancel();

                // Re-create the CancellationTokenSource for future reads.
                readCancellationTokenSource = new CancellationTokenSource();
            }

            if (randomAccessStream != null)
            {
                // Update the stream's seek position to the user-selected one.
                // The new seek position will take effect on the next ReadAsync operation.
                var seekPositionInMegaBytes = (ulong)seekSlider.Value;
                ulong seekPositionInBytes = seekPositionInMegaBytes * BytesPerMegaByte;
                randomAccessStream.Seek(seekPositionInBytes);
            }
        }

        private static string FormatDownloadedRanges(IEnumerable<BackgroundTransferFileRange> ranges)
        {
            string result = "";
            foreach (BackgroundTransferFileRange range in ranges)
            {
                if (result != "")
                {
                    result += ", ";
                }

                result += $"{range.Offset}-{range.Offset + range.Length - 1}";
            }

            if (result == "")
            {
                result = "None";
            }
            return result;
        }

        private async void OnDownloadProgress(IAsyncOperationWithProgress<DownloadOperation, DownloadOperation> sender, DownloadOperation progress)
        {
            // We capture a snapshot of DownloadOperation.Progress because
            // it is is updated in real-time while the operation is ongoing.
            BackgroundDownloadProgress currentProgress = progress.Progress;

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Prepare the progress message to display in the UI.
                ulong percent = 100;
                if (currentProgress.TotalBytesToReceive > 0)
                {
                    percent = currentProgress.BytesReceived * 100 / currentProgress.TotalBytesToReceive;
                }

                DownloadedStatusText.Text = $"{currentProgress.Status} - {percent}% downloaded.";
            });
        }

        private async void OnRangesDownloaded(DownloadOperation sender, BackgroundTransferRangesDownloadedEventArgs args)
        {
            // BackgroundTransfer will wait for the app's RangesDownloaded event handlers to finish executing before
            // triggering the next RangesDownloaded event. Any download progress made while the app's handlers are
            // executing get coalesced into a single RangesDownloaded event.
            //
            // When calling async APIs inside this handler, we must first take a deferral and then complete it once we
            // are done processing the event. Otherwise, BackgroundTransfer will assume that the handler is done executing
            // as soon as the synchronous portion of the handler returns, even if there are still tasks running.
            using (args.GetDeferral())
            {
                // AddedRanges tells you which ranges were downloaded since the last RangesDownloaded event.
                // To get the full list of downloaded ranges, use the GetDownloadedRanges method.
                string message = " Newly-downloaded ranges: " + FormatDownloadedRanges(args.AddedRanges);

                // A download may restart if the server contents changed while the download is in progress.
                if (args.WasDownloadRestarted)
                {
                    message += " (download was restarted)";
                }

                // Wait for the UI update to happen before completing the deferral. That way, we can be certain that
                // the next RangesDownloaded event will not get triggered midway through the UI update.
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    DownloadedRangesText.Text = FormatDownloadedRanges(sender.GetDownloadedRanges());
                    rootPage.NotifyUser(message, NotifyType.StatusMessage);
                });
            }
        }

        private async void StartDownload_Click(object sender, RoutedEventArgs e)
        {
            // Reset the output whenever a new download attempt begins.
            DownloadedInfoText.Text = "";
            DownloadedStatusText.Text = "";
            DownloadedRangesText.Text = "None";
            PreviousReadText.Text = "";
            CurrentReadText.Text = "";

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
                rootPage.NotifyUser("Error while creating file: " + ex.Message, NotifyType.ErrorMessage);
                return;
            }

            BackgroundDownloader downloader = new BackgroundDownloader();
            download = downloader.CreateDownload(source, destinationFile);

            // Apps need to set the IsRandomAccessRequired boolean property to TRUE (default is FALSE) before they
            // call StartAsync() on a DownloadOperation object if they want to use the random access feature. After
            // opting into "random access" mode, GetResultRandomAccessStreamReference() can be called.
            //
            // Setting IsRandomAccessRequired to TRUE before starting the download is necessary because the
            // server-side requirements for Random Access functionality are stricter than for regular (sequential)
            // downloads, so BackgroundTransfer needs to know the app's intentions in advance. That way, it can fail
            // the transfer up front if the server does not satisfy those additional requirements (e.g., HTTP range
            // header support).
            download.IsRandomAccessRequired = true;

            // Subscribe to RangesDownloaded notifications.
            download.RangesDownloaded += OnRangesDownloaded;

            DownloadedInfoText.Text = $"Downloading to {destinationFile.Name}, {download.Guid}";

            // Start the download and read partially-downloaded content while the transfer is still ongoing.
            await StartDownloadAndReadContentsAsync();
        }

        private async Task StartDownloadAndReadContentsAsync()
        {
            try
            {
                // Retrieve a random access stream from the download operation. Every OpenReadAsync() operation returns
                // a new stream instance that is independent of previous ones (i.e., the seek position of one stream
                // isn't affected by calls on another stream).
                //
                // This sample demonstrates the direct usage of a DownloadOperation's random access stream and its
                // effects on the ongoing transfer. However, bear in mind that there are a variety of operations
                // that can manage the stream on the app's behalf for specific scenarios. For instance, a
                // DownloadOperation pointing to a video URL can be consumed by the MediaPlayer class in four easy
                // steps:
                //
                // var randomAccessStreamReference = download.GetResultRandomAccessStreamReference();
                // stream = await randomAccessStreamReference.OpenReadAsync();
                // var mediaPlayer = new Windows.Media.Playback.MediaPlayer();
                // mediaPlayer.Source = Windows.Media.Core.MediaSource.CreateFromStream(stream, stream.ContentType);

                var randomAccessStreamReference = download.GetResultRandomAccessStreamReference();
                randomAccessStream = await randomAccessStreamReference.OpenReadAsync();

                // Start the download. If the server doesn't support random access, the download will fail
                // with WebErrorStatus.InsufficientRangeSupport or WebErrorStatus.MissingContentLengthSupport.
                IAsyncOperationWithProgress<DownloadOperation, DownloadOperation> downloadOperation = download.StartAsync();
                downloadOperation.Progress += OnDownloadProgress;
                Task downloadTask = downloadOperation.AsTask();

                startDownloadButton.IsEnabled = false;
                pauseDownloadButton.IsEnabled = true;

                // Read data while the download is still ongoing. Use a 1 MB read buffer for that purpose.
                var readBuffer = new Windows.Storage.Streams.Buffer(BytesPerMegaByte);
                while (!downloadTask.IsCompleted)
                {
                    ulong readOffsetInBytes = randomAccessStream.Position;

                    PreviousReadText.Text = CurrentReadText.Text;
                    CurrentReadText.Text = $"Reading from offset {readOffsetInBytes:n0}";

                    readOperation = randomAccessStream.ReadAsync(
                        readBuffer,
                        readBuffer.Capacity,
                        InputStreamOptions.None).
                        AsTask(readCancellationTokenSource.Token);

                    // Update the UI to show the current read's position.
                    currentPositionSlider.Value = readOffsetInBytes / BytesPerMegaByte;

                    try
                    {
                        // Wait for the read to complete.
                        IBuffer bytesRead = await readOperation;
                        CurrentReadText.Text += $", completed with {bytesRead.Length:n0} bytes";

                        // At this point, a real app would process the 'bytesRead' data to do something interesting
                        // with it (e.g., display video and/or play audio).

                        if (randomAccessStream.Position >= randomAccessStream.Size)
                        {
                            // We have reached EOF. Wrap around to the beginning while we wait for the download to complete.
                            randomAccessStream.Seek(0);
                        }
                    }
                    catch (OperationCanceledException)
                    {
                        // The ongoing read was canceled by SeekDownload_Click(...) in order for a new download
                        // position to take effect immediately.
                        CurrentReadText.Text += ", cancelled.";
                    }
                }

                // Wait for the download to complete.
                await downloadTask;

                rootPage.NotifyUser("Download completed successfully", NotifyType.StatusMessage);
            }
            catch (Exception ex) when (IsWebException("Execution error", ex))
            {
                // Abandon the operation if a web exception occurs.
            }
            finally
            {
                download = null;
                randomAccessStream = null;
                readOperation = null;

                startDownloadButton.IsEnabled = true;
                pauseDownloadButton.IsEnabled = false;
                resumeDownloadButton.IsEnabled = false;
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
