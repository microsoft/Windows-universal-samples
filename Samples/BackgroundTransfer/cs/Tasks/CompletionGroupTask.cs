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
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            BackgroundTransferCompletionGroupTriggerDetails details = taskInstance.TriggerDetails
                as BackgroundTransferCompletionGroupTriggerDetails;

            if (details == null)
            {
                // This task was not triggered by a completion group.
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

            if (failedDownloads.Count > 0)
            {
                RetryDownloads(failedDownloads);
            }

            InvokeSimpleToast(succeeded, failedDownloads.Count);
        }

        private bool IsFailed(DownloadOperation download)
        {
            BackgroundTransferStatus status = download.Progress.Status;
            if (status == BackgroundTransferStatus.Error || status == BackgroundTransferStatus.Canceled)
            {
                return true;
            }

            ResponseInformation response = download.GetResponseInformation();
            if (response.StatusCode != 200)
            {
                return true;
            }

            return false;
        }

        private void RetryDownloads(IEnumerable<DownloadOperation> downloads)
        {
            BackgroundDownloader downloader = CompletionGroupTask.CreateBackgroundDownloader();

            foreach (DownloadOperation download in downloads)
            {
                DownloadOperation download1 = downloader.CreateDownload(download.RequestedUri, download.ResultFile);
                Task<DownloadOperation> startTask = download1.StartAsync().AsTask();
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

            BackgroundTaskRegistration taskRegistration = builder.Register();

            BackgroundDownloader downloader = new BackgroundDownloader(completionGroup);

            return downloader;
        }

    }
}
