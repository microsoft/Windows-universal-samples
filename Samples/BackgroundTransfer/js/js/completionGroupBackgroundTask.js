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

(function () {
    "use strict";

    // This variable is used to get information about the current instance of the background task.
    var backgroundTaskInstance = Windows.UI.WebUI.WebUIBackgroundTaskInstance.current;

    // Trigger details contain information surrounding the reason why the background task was triggered.
    var details = backgroundTaskInstance.triggerDetails;

    // Entry point for the Completion Group background task.
    var taskEntryPoint = "js\\completionGroupBackgroundTask.js";

    // Completion groups allow us to immediately take action after a set of downloads completes.
    // In this sample, the server intentionally replies with an error status for some of the downloads.
    // Based on the trigger details, we can determine which of the downloads have failed and try them again
    // using a new completion group.
    function run() {
        if (details === null) {
            // This task was not triggered by a completion group.
            return;
        }

        var failedDownloads = [];
        var succeeded = 0;
        for (var i = 0; i < details.downloads.size; i++) {
            var download = details.downloads[i];
            if (isFailed(download)) {
                failedDownloads.push(download);
            } else {
                succeeded++;
            }
        }

        if (failedDownloads.length > 0) {
            retryDownloads(failedDownloads);
        }

        invokeSimpleToast(succeeded, failedDownloads.length);

        // A JavaScript background task must call close when done
        close();
    }

    function isFailed(download) {
        var currentStatus = download.progress.status;
        if (currentStatus === Windows.Networking.BackgroundTransfer.BackgroundTransferStatus.error ||
            currentStatus === Windows.Networking.BackgroundTransfer.BackgroundTransferStatus.canceled) {
            return true;
        }

        var response = download.getResponseInformation();
        if (response.statusCode !== 200) {
            return true;
        }

        return false;
    }

    function createBackgroundDownloader() {
        var completionGroup = new Windows.Networking.BackgroundTransfer.BackgroundTransferCompletionGroup();

        var builder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();
        builder.taskEntryPoint = taskEntryPoint;
        builder.setTrigger(completionGroup.trigger);
        var taskRegistration = builder.register();

        var downloader = new Windows.Networking.BackgroundTransfer.BackgroundDownloader(completionGroup);
        return downloader;
    }

    function retryDownloads(downloads) {
        var downloader = createBackgroundDownloader();

        for (var i = 0; i < downloads.length; i++) {
            var download = downloads[i];
            var newDownload = downloader.createDownload(download.requestedUri, download.resultFile);
            newDownload.startAsync();
        }

        downloader.completionGroup.enable();
    }

    function invokeSimpleToast(succeededCount, failedCount) {
        var toastXml = Windows.UI.Notifications.ToastNotificationManager.getTemplateContent(
            Windows.UI.Notifications.ToastTemplateType.toastImageAndText02);
        var stringElements = toastXml.getElementsByTagName("text");
        stringElements.item(0).appendChild(toastXml.createTextNode(String(succeededCount) + " downloads succeeded."));
        stringElements.item(1).appendChild(toastXml.createTextNode(String(failedCount) + " downloads failed."));

        var toast = new Windows.UI.Notifications.ToastNotification(toastXml);
        Windows.UI.Notifications.ToastNotificationManager.createToastNotifier().show(toast);
    }

    run();
})();
