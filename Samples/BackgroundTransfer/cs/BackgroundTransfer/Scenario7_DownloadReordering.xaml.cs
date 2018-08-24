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
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Windows.Networking.BackgroundTransfer;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web;

namespace SDKTemplate
{
    // Class for each download and its row in the UI.
    public class DownloadOperationItem : INotifyPropertyChanged
    {
        public DownloadOperationItem(DownloadOperation download)
        {
            _download = download;
            _percentComplete = 0;
            _stateText = "Idle";
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public DownloadOperation download
        {
            get
            {
                return _download;
            }
        }

        public int percentComplete
        {
            get
            {
                return _percentComplete;
            }

            set
            {
                _percentComplete = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("percentComplete"));
                }
            }
        }

        public string stateText
        {
            get
            {
                return _stateText;
            }

            set
            {
                _stateText = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("stateText"));
                }
            }
        }

        private DownloadOperation _download;
        private int _percentComplete;
        private string _stateText;
    }

    public sealed partial class Scenario7_DownloadReordering : Page
    {
        private const int NumDownloads = 5;

        private MainPage rootPage;
        private BackgroundTransferGroup reorderGroup;
        public ObservableCollection<DownloadOperationItem> downloadCollection { get; } =
            new ObservableCollection<DownloadOperationItem>();

        public Scenario7_DownloadReordering()
        {
            reorderGroup = BackgroundTransferGroup.CreateGroup("{7421B969-18D4-4532-B6BD-22BDABF71C08}");
            reorderGroup.TransferBehavior = BackgroundTransferBehavior.Serialized;
            this.DataContext = this;
            this.InitializeComponent();
        }

        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            await CancelActiveDownloadsAsync();
        }

        private async Task CancelActiveDownloadsAsync()
        {
            // Only the downloads that belong to the transfer group used by this sample scenario
            // will be canceled.
            IReadOnlyList<DownloadOperation> downloads = null;
            try
            {
                downloads = await BackgroundDownloader.GetCurrentDownloadsForTransferGroupAsync(reorderGroup);
            }
            catch (Exception ex)
            {
                if (!IsWebException("Discovery error", ex))
                {
                    throw;
                }
                return;
            }

            // If previous instances of this scenario started transfers that haven't completed yet,
            // cancel them now so that we can start this scenario cleanly.
            if (downloads.Count > 0)
            {
                CancellationTokenSource cancellationToken = new CancellationTokenSource();
                cancellationToken.Cancel();

                Task[] tasks = new Task[downloads.Count];
                for (int i = 0; i < downloads.Count; i++)
                {
                    tasks[i] = downloads[i].AttachAsync().AsTask(cancellationToken.Token);
                }

                // Cancel each download and ignore the cancellation exception.
                try
                {
                    await Task.WhenAll(tasks);
                }
                catch (TaskCanceledException)
                {
                }
            }
        }

        private async void StartDownload_Click(object sender, RoutedEventArgs e)
        {
            startDownloadButton.IsEnabled = false;

            // Create and start downloads.
            await RunDownloadsAsync();

            // After all downloads are complete, let the user start new ones again.
            startDownloadButton.IsEnabled = true;
        }

        private void MakeCurrent_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            DownloadOperationItem item = button.DataContext as DownloadOperationItem;

            // Make the selected operation current.
            item.download.MakeCurrentInTransferGroup();
        }

        private async Task RunDownloadsAsync()
        {
            // Create a downloader and associate all its downloads with the transfer group used for
            // this scenario.
            BackgroundDownloader downloader = new BackgroundDownloader();
            downloader.TransferGroup = reorderGroup;

            // Validating the URI is required since it was received from an untrusted source (user
            // input).
            // The URI is validated by calling Uri.TryCreate() that will return 'false' for strings
            // that are not valid URIs.
            // Note that when enabling the text box users may provide URIs to machines on the
            // intranet that require the "Private Networks (Client and Server)" capability.
            string remoteAddress = remoteAddressField.Text.Trim();
            Uri tmpUri;
            if (!Uri.TryCreate(remoteAddress, UriKind.Absolute, out tmpUri))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            string fileName = fileNameField.Text.Trim();
            if (string.IsNullOrWhiteSpace(fileName))
            {
                rootPage.NotifyUser("A local file name is required.", NotifyType.ErrorMessage);
                return;
            }

            // Try to create some downloads.
            DownloadOperation[] downloads = new DownloadOperation[NumDownloads];
            try
            {
                for (int i = 0; i < NumDownloads; i++)
                {
                    string currRemoteAddress = $"{remoteAddress}?id={i}";
                    string currFileName = $"{i}.{fileName}";
                    downloads[i] = await CreateDownload(downloader, currRemoteAddress, currFileName);
                }
            }
            catch (FileNotFoundException ex)
            {
                rootPage.NotifyUser($"Error while creating file: {ex.Message}", NotifyType.ErrorMessage);
                return;
            }

            // Once all downloads have been created, start them.
            Task[] downloadTasks = new Task[downloads.Length];
            downloadCollection.Clear();
            for (int i = 0; i < downloads.Length; i++)
            {
                DownloadOperationItem item = new DownloadOperationItem(downloads[i]);
                downloadTasks[i] = DownloadAsync(item);
                downloadCollection.Add(item);
            }

            await Task.WhenAll(downloadTasks);
        }

        private async Task<DownloadOperation> CreateDownload(
            BackgroundDownloader downloader,
            string remoteAddress,
            string fileName)
        {
            Uri source = new Uri(remoteAddress);

            StorageFile destinationFile;
            try
            {
                destinationFile = await KnownFolders.PicturesLibrary.CreateFileAsync(
                    fileName,
                    CreationCollisionOption.GenerateUniqueName);
            }
            catch (FileNotFoundException ex)
            {
                rootPage.NotifyUser($"Error while creating file: {ex.Message}", NotifyType.ErrorMessage);
                throw;
            }

            return downloader.CreateDownload(source, destinationFile);
        }

        private async Task DownloadAsync(DownloadOperationItem item)
        {
            Progress<DownloadOperation> progressCallback = new Progress<DownloadOperation>(
                (operation) => DownloadProgress(item));

            try
            {
                await item.download.StartAsync().AsTask(progressCallback);

                item.stateText = item.download.Progress.Status.ToString();
                rootPage.NotifyUser(
                    $"Downloading {item.download.ResultFile.Name} completed.",
                    NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                // Ignore canceled downloads since they are not displayed.
                if (item.download.Progress.Status != BackgroundTransferStatus.Canceled)
                {
                    // Ensure that we reach 100% even for errors.
                    item.percentComplete = 100;
                    item.stateText = item.download.Progress.Status.ToString();
                    if (!IsWebException("Execution error", ex, item.download))
                    {
                        throw;
                    }
                }
            }
        }

        private void DownloadProgress(DownloadOperationItem item)
        {
            BackgroundDownloadProgress currentProgress = item.download.Progress;
            BackgroundTransferStatus status = currentProgress.Status;
            int percentComplete = 0;

            if (currentProgress.TotalBytesToReceive > 0)
            {
                percentComplete = (int)((currentProgress.BytesReceived * 100) / currentProgress.TotalBytesToReceive);
            }

            var ignore = this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                item.stateText = status.ToString();
                item.percentComplete = percentComplete;
            });
        }

        private bool IsWebException(string title, Exception ex, DownloadOperation download = null)
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
                rootPage.NotifyUser($"{download.ResultFile.Name} - {title}: {message}", NotifyType.ErrorMessage);
            }

            return result;
        }
    }
}
