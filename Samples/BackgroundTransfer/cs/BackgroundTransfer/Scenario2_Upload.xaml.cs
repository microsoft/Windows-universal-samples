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
using System.Threading;
using System.Threading.Tasks;
using SDKTemplate;
using Windows.Networking.BackgroundTransfer;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Pickers;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web;

namespace BackgroundTransfer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_Upload : Page, IDisposable
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        private CancellationTokenSource cts;

        private const int maxUploadFileSize = 100 * 1024 * 1024; // 100 MB (arbitrary limit chosen for this sample)

        public Scenario2_Upload()
        {
            cts = new CancellationTokenSource();

            this.InitializeComponent();
        }

        public void Dispose()
        {
            if (cts != null)
            {
                cts.Dispose();
                cts = null;
            }

            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            // An application must enumerate uploads when it gets started to prevent stale downloads/uploads.
            // Typically this can be done in the App class by overriding OnLaunched() and checking for
            // "args.Kind == ActivationKind.Launch" to detect an actual app launch.
            // We do it here in the sample to keep the sample code consolidated.
            await DiscoverActiveUploadsAsync();
        }

        // Enumerate the uploads that were going on in the background while the app was closed.
        private async Task DiscoverActiveUploadsAsync()
        {
            IReadOnlyList<UploadOperation> uploads = null;
            try
            {
                uploads = await BackgroundUploader.GetCurrentUploadsAsync();
            }
            catch (Exception ex)
            {
                if (!IsExceptionHandled("Discovery error", ex))
                {
                    throw;
                }
                return;
            }

            Log("Loading background uploads: " + uploads.Count);

            if (uploads.Count > 0)
            {
                List<Task> tasks = new List<Task>();
                foreach (UploadOperation upload in uploads)
                {
                    Log(String.Format(CultureInfo.CurrentCulture, "Discovered background upload: {0}, Status: {1}", 
                        upload.Guid, upload.Progress.Status));

                    // Attach progress and completion handlers.
                    tasks.Add(HandleUploadAsync(upload, false));
                }

                // Don't await HandleUploadAsync() in the foreach loop since we would attach to the second
                // upload only when the first one completed; attach to the third upload when the second one
                // completes etc. We want to attach to all uploads immediately.
                // If there are actions that need to be taken once uploads complete, await tasks here, outside
                // the loop.
                await Task.WhenAll(tasks);
            }
        }

        private void StartUpload_Click(object sender, RoutedEventArgs e)
        {
            // Validating the URI is required since it was received from an untrusted source (user input).
            // The URI is validated by calling Uri.TryCreate() that will return 'false' for strings that are not valid URIs.
            // Note that when enabling the text box users may provide URIs to machines on the intrAnet that require
            // the "Home or Work Networking" capability.
            Uri uri;
            if (!Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, out uri))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            FileOpenPicker picker = new FileOpenPicker();
            picker.FileTypeFilter.Add("*");

            StartSingleFileUpload(picker, uri);
        }

        private async void UploadSingleFile(Uri uri, StorageFile file)
        {
            if (file == null)
            {
                rootPage.NotifyUser("No file selected.", NotifyType.ErrorMessage);
                return;
            }

            BasicProperties properties = await file.GetBasicPropertiesAsync();
            if (properties.Size > maxUploadFileSize)
            {
                rootPage.NotifyUser(String.Format(CultureInfo.CurrentCulture, 
                    "Selected file exceeds max. upload file size ({0} MB).", maxUploadFileSize / (1024 * 1024)),
                    NotifyType.ErrorMessage);
                return;
            }

            BackgroundUploader uploader = new BackgroundUploader();
            uploader.SetRequestHeader("Filename", file.Name);

            UploadOperation upload = uploader.CreateUpload(uri, file);
            Log(String.Format(CultureInfo.CurrentCulture, "Uploading {0} to {1}, {2}", file.Name, uri.AbsoluteUri, 
                upload.Guid));

            // Attach progress and completion handlers.
            await HandleUploadAsync(upload, true);
        }

        private void StartMultipartUpload_Click(object sender, RoutedEventArgs e)
        {
            // By default 'serverAddressField' is disabled and URI validation is not required. When enabling the text
            // box validating the URI is required since it was received from an untrusted source (user input).
            // The URI is validated by calling Uri.TryCreate() that will return 'false' for strings that are not valid URIs.
            // Note that when enabling the text box users may provide URIs to machines on the intrAnet that require
            // the "Home or Work Networking" capability.
            Uri uri;
            if (!Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, out uri))
            {
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage);
                return;
            }

            FileOpenPicker picker = new FileOpenPicker();
            picker.FileTypeFilter.Add("*");

            StartMultipleFilesUpload(picker, uri);
        }

        private async void StartSingleFileUpload(FileOpenPicker picker, Uri uri)
        {
            StorageFile file = await picker.PickSingleFileAsync();
            UploadSingleFile(uri, file);
        }

        private async void StartMultipleFilesUpload(FileOpenPicker picker, Uri uri)
        {
            IReadOnlyList<StorageFile> files = await picker.PickMultipleFilesAsync();
            UploadMultipleFiles(uri, files);
        }

        private async void UploadMultipleFiles(Uri uri, IReadOnlyList<StorageFile> files)
        {
            if (files.Count == 0)
            {
                rootPage.NotifyUser("No file selected.", NotifyType.ErrorMessage);
                return;
            }

            ulong totalFileSize = 0;
            for (int i = 0; i < files.Count; i++)
            {
                BasicProperties properties = await files[i].GetBasicPropertiesAsync();
                totalFileSize += properties.Size;

                if (totalFileSize > maxUploadFileSize)
                {
                    rootPage.NotifyUser(String.Format(CultureInfo.CurrentCulture,
                        "Size of selected files exceeds max. upload file size ({0} MB).", 
                        maxUploadFileSize / (1024 * 1024)), NotifyType.ErrorMessage);
                    return;
                }
            }

            List<BackgroundTransferContentPart> parts = new List<BackgroundTransferContentPart>();
            for (int i = 0; i < files.Count; i++)
            {
                BackgroundTransferContentPart part = new BackgroundTransferContentPart("File" + i, files[i].Name);
                part.SetFile(files[i]);
                parts.Add(part);
            }

            BackgroundUploader uploader = new BackgroundUploader();
            UploadOperation upload = await uploader.CreateUploadAsync(uri, parts);

            String fileNames = files[0].Name;
            for (int i = 1; i < files.Count; i++)
            {
                fileNames += ", " + files[i].Name;
            }

            Log(String.Format(CultureInfo.CurrentCulture, "Uploading {0} to {1}, {2}", fileNames, uri.AbsoluteUri, 
                upload.Guid));

            // Attach progress and completion handlers.
            await HandleUploadAsync(upload, true);
        }

        private void CancelAll_Click(object sender, RoutedEventArgs e)
        {
            Log("Canceling all active uploads");

            cts.Cancel();
            cts.Dispose();

            // Re-create the CancellationTokenSource and activeUploads for future uploads.
            cts = new CancellationTokenSource();
        }

        // Note that this event is invoked on a background thread, so we cannot access the UI directly.
        private void UploadProgress(UploadOperation upload)
        {
            // UploadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
            // we must make a local copy so that we can have a consistent view of that ever-changing state
            // throughout this method's lifetime.
            BackgroundUploadProgress currentProgress = upload.Progress;

            MarshalLog(String.Format(CultureInfo.CurrentCulture, "Progress: {0}, Status: {1}", upload.Guid, 
                currentProgress.Status));

            double percentSent = 100;
            if (currentProgress.TotalBytesToSend > 0)
            {
                percentSent = currentProgress.BytesSent * 100 / currentProgress.TotalBytesToSend;
            }

            MarshalLog(String.Format(CultureInfo.CurrentCulture, 
                " - Sent bytes: {0} of {1} ({2}%), Received bytes: {3} of {4}", currentProgress.BytesSent, 
                currentProgress.TotalBytesToSend, percentSent, currentProgress.BytesReceived, currentProgress.TotalBytesToReceive));

            if (currentProgress.HasRestarted)
            {
                MarshalLog(" - Upload restarted");
            }

            if (currentProgress.HasResponseChanged)
            {
                // We've received new response headers from the server.
                MarshalLog(" - Response updated; Header count: " + upload.GetResponseInformation().Headers.Count);

                // If you want to stream the response data this is a good time to start.
                // upload.GetResultStreamAt(0);
            }
        }

        private async Task HandleUploadAsync(UploadOperation upload, bool start)
        {
            try
            {
                LogStatus("Running: " + upload.Guid, NotifyType.StatusMessage);

                Progress<UploadOperation> progressCallback = new Progress<UploadOperation>(UploadProgress);
                if (start)
                {
                    // Start the upload and attach a progress handler.
                    await upload.StartAsync().AsTask(cts.Token, progressCallback);
                }
                else
                {
                    // The upload was already running when the application started, re-attach the progress handler.
                    await upload.AttachAsync().AsTask(cts.Token, progressCallback);
                }

                ResponseInformation response = upload.GetResponseInformation();

                LogStatus(String.Format(CultureInfo.CurrentCulture, "Completed: {0}, Status Code: {1}", upload.Guid,
                    response.StatusCode), NotifyType.StatusMessage);
            }
            catch (TaskCanceledException)
            {
                LogStatus("Canceled: " + upload.Guid, NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                if (!IsExceptionHandled("Error", ex, upload))
                {
                    throw;
                }
            }
        }

        private bool IsExceptionHandled(string title, Exception ex, UploadOperation upload = null)
        {
            WebErrorStatus error = BackgroundTransferError.GetStatus(ex.HResult);
            if (error == WebErrorStatus.Unknown)
            {
                return false;
            }

            if (upload == null)
            {
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0}: {1}", title, error), 
                    NotifyType.ErrorMessage);
            }
            else
            {
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0} - {1}: {2}", upload.Guid, title, 
                    error), NotifyType.ErrorMessage);
            }

            return true;
        }

        // When operations happen on a background thread we have to marshal UI updates back to the UI thread.
        private void MarshalLog(string value)
        {
            var ignore = this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                Log(value);
            });
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
