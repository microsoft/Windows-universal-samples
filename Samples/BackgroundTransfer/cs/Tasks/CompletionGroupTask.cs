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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Data.Xml.Dom;
using Windows.Networking.BackgroundTransfer;
using Windows.Storage;
using Windows.UI.Notifications;

namespace Tasks
{
    public sealed class CompletionGroupTask : IBackgroundTask
    {
        // Completion groups allow us to immediately take action after a set of downloads completes.
        // In this sample, the server intentionally replies with an error status for some of the downloads.
        // Based on the trigger details, we can determine which of the downloads have failed and try them again
        // using a new completion group.
        public async void Run(IBackgroundTaskInstance taskInstance)
        {
            // The background task awaits on an asynchronous task (RetryDownloadsAsync). Take a deferral
            // to delay the background task from closing prematurely while the asynchronous code is still running.
            BackgroundTaskDeferral deferral = taskInstance.GetDeferral();

            BackgroundTransferCompletionGroupTriggerDetails details = taskInstance.TriggerDetails
                as BackgroundTransferCompletionGroupTriggerDetails;

            if (details == null)
            {
                // This task was not triggered by a completion group.
                deferral.Complete();
                return;
            }

            List<DownloadOperation> failedDownloads = new List<DownloadOperation>();
            int succeeded = 0;
            foreach (DownloadOperation download in details.Downloads)
            {
                if (IsFailed(download))
                {
                    failedDownloads.Add(download);
                }
                else
                {
                    succeeded++;
                }
            }

            InvokeSimpleToast(succeeded, failedDownloads.Count);

            if (failedDownloads.Count > 0)
            {
                await RetryDownloadsAsync(failedDownloads);
            }

            deferral.Complete();
        }

        private bool IsFailed(DownloadOperation download)
        {
            BackgroundTransferStatus status = download.Progress.Status;
            if (status == BackgroundTransferStatus.Error || status == BackgroundTransferStatus.Canceled)
            {
                return true;
            }

            ResponseInformation response = download.GetResponseInformation();
            if (response == null || response.StatusCode != 200)
            {
                return true;
            }

            return false;
        }
        private async Task RetryDownloadsAsync(IEnumerable<DownloadOperation> downloadsToRetry)
        {
            BackgroundDownloader downloader = CompletionGroupTask.CreateBackgroundDownloader();

            foreach (DownloadOperation downloadToRetry in downloadsToRetry)
            {
                // Retry with the same uri, but save to a different file name.
                string originalName = downloadToRetry.ResultFile.Name;
                string newName = originalName.Insert(originalName.LastIndexOf('.'), "_retried");
                await downloadToRetry.ResultFile.RenameAsync(newName, NameCollisionOption.ReplaceExisting);

                DownloadOperation download = downloader.CreateDownload(downloadToRetry.RequestedUri, downloadToRetry.ResultFile);

                // We do not await on background transfer asynchronous tasks, since await will only 
                // finish after the entire transfer is complete, which may take a very long time. 
                // The completion of these downloads will be handled by a future CompletionGroupTask instance.
                Task<DownloadOperation> startTask = download.StartAsync().AsTask();
            }

            downloader.CompletionGroup.Enable();
        }

        public void InvokeSimpleToast(int succeeded, int failed)
        {
            XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastImageAndText02);

            XmlNodeList stringElements = toastXml.GetElementsByTagName("text");
            stringElements.Item(0).AppendChild(toastXml.CreateTextNode(String.Format(
                CultureInfo.InvariantCulture,
                "{0} downloads succeeded.",
                succeeded)));
            stringElements.Item(1).AppendChild(toastXml.CreateTextNode(String.Format(
                CultureInfo.InvariantCulture,
                "{0} downloads failed.",
                failed)));

            ToastNotification toast = new ToastNotification(toastXml);
            ToastNotificationManager.CreateToastNotifier().Show(toast);
        }

        public static BackgroundDownloader CreateBackgroundDownloader()
        {
            BackgroundTransferCompletionGroup completionGroup = new BackgroundTransferCompletionGroup();

            BackgroundTaskBuilder builder = new BackgroundTaskBuilder();
            builder.TaskEntryPoint = "Tasks.CompletionGroupTask";
            builder.SetTrigger(completionGroup.Trigger);

            // The system automatically unregisters the BackgroundTransferCompletionGroup task when it triggers.
            // You do not need to unregister it explicitly.
            BackgroundTaskRegistration taskRegistration = builder.Register();

            BackgroundDownloader downloader = new BackgroundDownloader(completionGroup);

            return downloader;
        }

    }
}
