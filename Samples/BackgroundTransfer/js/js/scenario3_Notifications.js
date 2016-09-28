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

    var BackgroundTransfer = Windows.Networking.BackgroundTransfer;

    var runId = 0;
    var notificationsGroup;

    var scenarioType = {
        tile: "Tile",
        toast: "Toast"
    };

    var page = WinJS.UI.Pages.define("/html/scenario3_Notifications.html", {
        ready: function (element, options) {
            // Assign event listeners for each button on click.
            id("toastNotificationButton").addEventListener("click", toastNotificationButtonClick, false);
            id("tileNotificationButton").addEventListener("click", tileNotificationButtonClick, false);

            // Use a unique group name so that no other component in the app uses the same group. The recommended way
            // is to generate a GUID and use it as group name as shown below.
            notificationsGroup = BackgroundTransfer.BackgroundTransferGroup.createGroup(
                "{AA456EC0-DBF4-4411-8D76-97FAEDF9C661}");

            // When creating a group, we can optionally define the transfer behavior of transfers associated with the
            // group. A "parallel" transfer behavior allows multiple transfers in the same group to run concurrently
            // (default). A "serialized" transfer behavior allows at most one default priority transfer at a time for
            // the group.
            notificationsGroup.transferBehavior = BackgroundTransfer.BackgroundTransferBehavior.parallel;

            // An application must enumerate downloads when it gets started to prevent stale downloads/uploads.
            // Note that for this specific scenario we're not interested in downloads from previous instances:
            // We'll just enumerate downloads from previous instances and cancel them immediately.
            BackgroundTransfer.BackgroundDownloader.getCurrentDownloadsForTransferGroupAsync(notificationsGroup)
                .done(function (downloadOperations) {
                    var loadedDownloadOperations = [];

                    // If previous instances of this scenario started transfers that haven't completed yet, cancel them
                    // now so that we can start this scenario cleanly.
                    for (var i = 0; i < downloadOperations.size; i++) {
                        var promise = downloadOperations[i].attachAsync();
                        promise.cancel();
                        loadedDownloadOperations.push(promise);
                    }

                    WinJS.Promise.join(loadedDownloadOperations).done(function () {
                        onLoadedDownloadsCanceled(downloadOperations);
                    }, function () {
                        onLoadedDownloadsCanceled(downloadOperations);
                    });
                });
        }
    });

    function onLoadedDownloadsCanceled(downloadOperations) {
        if (downloadOperations.size > 0) {
            printLog("Canceled " + downloadOperations.size + " downloads from previous instances of this scenario.");
            printLog("<br/>");
        }

        document.getElementById("toastNotificationButton").disabled = false;
        document.getElementById("tileNotificationButton").disabled = false;
    }

    function toastNotificationButtonClick() {
        // Create a downloader and associate all its downloads with the transfer group used for this scenario.
        var downloader = new BackgroundTransfer.BackgroundDownloader();
        downloader.transferGroup = notificationsGroup;

        // Create a ToastNotification that should be shown when all transfers succeed.
        var successMessage = "All three downloads completed successfully.";
        var successToastXml = Windows.UI.Notifications.ToastNotificationManager.getTemplateContent(
            Windows.UI.Notifications.ToastTemplateType.toastText01);
        successToastXml.getElementsByTagName("text").item(0).innerText = successMessage;
        var successToast = new Windows.UI.Notifications.ToastNotification(successToastXml);
        downloader.successToastNotification = successToast;

        // Create a ToastNotification that should be shown if at least one transfer fails.
        var failureMessage = "At least one download completed with failure.";
        var failureToastXml = Windows.UI.Notifications.ToastNotificationManager.getTemplateContent(
            Windows.UI.Notifications.ToastTemplateType.toastText01);
        failureToastXml.getElementsByTagName("text").item(0).innerText = failureMessage;
        var failureToast = new Windows.UI.Notifications.ToastNotification(failureToastXml);
        downloader.failureToastNotification = failureToast;

        // Now create and start downloads for the configured BackgroundDownloader object.
        runDownloads(downloader, scenarioType.toast);
    }

    function tileNotificationButtonClick() {
        // Create a downloader and associate all its downloads with the transfer group used for this scenario.
        var downloader = new BackgroundTransfer.BackgroundDownloader();
        downloader.transferGroup = notificationsGroup;

        var nowPlus10Minutes = new Date(Date.now() + 600000);

        // Create a TileNotification that should be shown when all transfers succeed.
        var successTileXml = Windows.UI.Notifications.TileUpdateManager.getTemplateContent(
            Windows.UI.Notifications.TileTemplateType.tileSquare150x150Text03);
        var successTextNodes = successTileXml.getElementsByTagName("text");
        successTextNodes.item(0).innerText = "All three";
        successTextNodes.item(1).innerText = "downloads";
        successTextNodes.item(2).innerText = "completed";
        successTextNodes.item(3).innerText = "successfully.";
        var successTile = new Windows.UI.Notifications.TileNotification(successTileXml);
        successTile.expirationTime = nowPlus10Minutes;
        downloader.successTileNotification = successTile;

        // Create a TileNotification that should be shown if at least one transfer fails.
        var failureTileXml = Windows.UI.Notifications.TileUpdateManager.getTemplateContent(
            Windows.UI.Notifications.TileTemplateType.tileSquare150x150Text03);
        var failureTextNodes = failureTileXml.getElementsByTagName("text");
        failureTextNodes.item(0).innerText = "At least";
        failureTextNodes.item(1).innerText = "one download";
        failureTextNodes.item(2).innerText = "completed";
        failureTextNodes.item(3).innerText = "with failure.";
        var failureTile = new Windows.UI.Notifications.TileNotification(failureTileXml);
        failureTile.expirationTime = nowPlus10Minutes;
        downloader.failureTileNotification = failureTile;

        // Now create and start downloads for the configured BackgroundDownloader object.
        runDownloads(downloader, scenarioType.tile);
    }

    function runDownloads(downloader, type) {
        // Use a unique ID for every button click, to help the user associate downloads of the same run
        // in the logs.
        runId++;

        var downloadOperations = [];
        var createDownloadPromises = [];

        var current = createDownloadAsync(downloader, 1, "FastDownload", type, downloadOperations);
        if (current === null) {
            return;
        }
        createDownloadPromises.push(current);

        current = createDownloadAsync(downloader, 5, "MediumDownload", type, downloadOperations);
        if (current === null) {
            return;
        }
        createDownloadPromises.push(current);

        current = createDownloadAsync(downloader, 10, "SlowDownload", type, downloadOperations);
        if (current === null) {
            return;
        }
        createDownloadPromises.push(current);

        // Once all downloads participating in the toast/tile update have been created, start them.
        WinJS.Promise.join(createDownloadPromises).done(function () {
            if (downloadOperations.length !== 3) {
                // At least one of the three downloads couldn't be created.
                // Just return and don't start remaining downloads.
                return;
            }

            for (var i = 0; i < downloadOperations.length; i++) {
                runDownload(downloadOperations[i]);
            }
        });
    }

    function createDownloadAsync(downloader, delaySeconds, fileNameSuffix, type, downloadOperations)
    {
        // Validating the URI is required since it was received from an untrusted source (user input).
        // The URI is validated by catching exceptions thrown by the Uri constructor.
        var source = null;
        try {
            var baseUri = document.getElementById("serverAddressField").value;
            source = new Windows.Foundation.Uri(baseUri, "?delay=" + String(delaySeconds));
        } catch (error) {
            displayError("Error: Invalid URI. " + error.message);
            return null;
        }

        var fileName = type + "." + String(runId) + "." + fileNameSuffix + ".txt";

        return Windows.Storage.KnownFolders.picturesLibrary.createFileAsync(
            fileName,
            Windows.Storage.CreationCollisionOption.generateUniqueName)
            .then(function (destinationFile) {
                downloadOperations.push(downloader.createDownload(source, destinationFile));
            }, function (err) {
                displayException(err, fileName);
            });
    }

    function runDownload(download) {
        printLog("Downloading " + download.resultFile.name + ".<br/>");

        download.startAsync().done(function () {
            var completedString = "Downloading " + download.resultFile.name + " completed.";
            printLog(completedString + "<br/>");
            displayStatus(completedString);
        }, function (err) {
            displayException(err, download.resultFile.name);
        });
    }

    function displayException(err, fileName) {
        var message;
        if (fileName) {
            message = fileName + ": " + err.message;
        } else {
            message = err.message;
        }

        displayError(message);
    }

    function displayError(/*@type(String)*/message) {
        WinJS.log && WinJS.log(message, "sample", "error");
    }

    function displayStatus(/*@type(String)*/message) {
        WinJS.log && WinJS.log(message, "sample", "status");
    }

    // Print helper function.
    function printLog(/*@type(String)*/txt) {
        var outputConsole = document.getElementById("outputConsole");
        outputConsole.innerHTML += txt;
    }

    function id(elementId) {
        return document.getElementById(elementId);
    }

})();
