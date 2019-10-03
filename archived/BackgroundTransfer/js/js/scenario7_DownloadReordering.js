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

    // Class for each download and its row in the UI.
    function DownloadCollectionItem(i) {
        var makeCurrentButton = document.getElementById("makeCurrentButton" + i);
        var percentCompleteInput = document.getElementById("percentCompleteInput" + i);
        var stateTextSpan = document.getElementById("stateTextSpan" + i);
        var download = null;

        this.startAsync = function (dl) {
            download = dl;

            makeCurrentButton.addEventListener("click", makeCurrent_click);
            makeCurrentButton.disabled = false;
            percentCompleteInput.value = 0;
            stateTextSpan.innerText = "idle";

            return download.startAsync().then(complete, error, progress);
        }

        this.reset = function () {
            makeCurrentButton.disabled = true;
            download = null;
        }

        function makeCurrent_click() {
            // Make the selected operation current.
            download.makeCurrentInTransferGroup();
        }

        function complete() {
            stateTextSpan.innerText = SdkSample.lookupEnumName(
                BackgroundTransfer.BackgroundTransferStatus,
                download.progress.status);
            WinJS.log(`Downloading ${download.resultFile.name} completed.`, "sample", "status");
        };

        function error(err) {
            // Ignore canceled downloads since they are not displayed.
            if (download.progress.status != BackgroundTransfer.BackgroundTransferStatus.canceled) {
                // Ensure that we reach 100% even for errors.
                percentCompleteInput.value = 100;
                stateTextSpan.innerText = SdkSample.lookupEnumName(
                    BackgroundTransfer.BackgroundTransferStatus,
                    download.progress.status);
                if (!isWebException("Execution error", err, download)) {
                    return WinJS.Promise.wrapError(err);
                }
            }
        }

        function progress() {
            var currentProgress = download.progress;
            var percentComplete = 0;

            if (currentProgress.totalBytesToReceive > 0) {
                percentComplete = Math.floor(
                    (currentProgress.bytesReceived * 100) / currentProgress.totalBytesToReceive);
            }

            percentCompleteInput.value = percentComplete;
            stateTextSpan.innerText = SdkSample.lookupEnumName(
                BackgroundTransfer.BackgroundTransferStatus,
                currentProgress.status);
        }
    }

    const NumDownloads = 5;

    var remoteAddressField;
    var fileNameField;
    var startDownloadButton;

    var reorderGroup;
    var downloadCollection;

    var page = WinJS.UI.Pages.define("/html/scenario7_DownloadReordering.html", {
        ready: function (element, options) {
            remoteAddressField = document.getElementById("remoteAddressField");
            fileNameField = document.getElementById("fileNameField");
            startDownloadButton = document.getElementById("startDownloadButton");
            startDownloadButton.addEventListener("click", startDownload_click);

            reorderGroup = BackgroundTransfer.BackgroundTransferGroup.createGroup(
                "{7421B969-18D4-4532-B6BD-22BDABF71C08}");
            reorderGroup.transferBehavior = BackgroundTransfer.BackgroundTransferBehavior.serialized;
            downloadCollection = [];
            for (var i = 0; i < NumDownloads; i++) {
                downloadCollection.push(new DownloadCollectionItem(i));
            }

            cancelActiveDownloadsAsync().done();
        }
    });

    function cancelActiveDownloadsAsync() {
        // Only the downloads that belong to the transfer group used by this sample scenario will be
        // canceled.
        return BackgroundTransfer.BackgroundDownloader.getCurrentDownloadsForTransferGroupAsync(reorderGroup)
            .then(function (downloads) {
            // If previous sample instances/scenarios started transfers that haven't completed yet,
            // cancel them now so that we can start this scenario cleanly.
            if (downloads.length > 0) {
                var promises = downloads.map(function (download) {
                    // Cancel each download and ignore the cancellation exception.
                    var downloadPromise = download.attachAsync();
                    downloadPromise.cancel();
                    return downloadPromise.then(null, function (err) {
                        if (download.progress.status != BackgroundTransfer.BackgroundTransferStatus.canceled) {
                            if (!isWebException("Discovery error", err, null)) {
                                return WinJS.Promise.wrapError(err);
                            }
                        }
                    });
                })
                return WinJS.Promise.join(promises);
            }
        });
    }

    function startDownload_click() {
        startDownloadButton.disabled = true;

        // Create and start downloads.
        runDownloadAsync(function () {
            // After all downloads are complete, disable the downloads and let the user start new
            // ones again.
            downloadCollection.forEach(function (downloadCollectionItem) {
                downloadCollectionItem.reset();
            });
            startDownloadButton.disabled = false;
        });
    }

    function runDownloadAsync(done_callback) {
        // Create a downloader and associate all its downloads with the transfer group used for this
        // scenario.
        var downloader = new BackgroundTransfer.BackgroundDownloader();
        downloader.transferGroup = reorderGroup;

        // Validating the URI is required since it was received from an untrusted source (user
        // input).
        // The URI is validated by calling Windows.Foundation.Uri that will throw an exception for
        // strings that are not valid URIs.
        // Note that when enabling the text box users may provide URIs to machines on the intranet
        // that require the "Private Networks (Client and Server)" capability.
        var remoteAddress = remoteAddressField.value.trim();
        try {
            var uri = new Windows.Foundation.Uri(remoteAddress);
        }
        catch (error) {
            WinJS.log("Invalid URI.", "sample", "error");
            return;
        }

        var fileName = fileNameField.value.trim();
        if (fileName == "") {
            WinJS.log("A local file name is required.", "sample", "error");
            return;
        }

        // Try to create some downloads.
        var createDownloadPromises = [];
        for (var i = 0; i < NumDownloads; i++) {
            createDownloadPromises.push(
                createDownloadAsync(downloader, remoteAddress + "?id=" + i, i + "." + fileName));
        }

        return WinJS.Promise.join(createDownloadPromises).then(function (downloads) {
            // Once all downloads have been created, start them.
            var downloadPromises = [];
            for (var i = 0; i < downloads.length; i++) {
                var downloadCollectionItem = downloadCollection[i];
                downloadPromises.push(downloadCollectionItem.startAsync(downloads[i]));
            }

            WinJS.Promise.join(downloadPromises).done(done_callback);
        });
    }

    function createDownloadAsync(downloader, remoteAddress, fileName) {
        return Windows.Storage.KnownFolders.picturesLibrary.createFileAsync(
            fileName,
            Windows.Storage.CreationCollisionOption.generateUniqueName).then(function (destinationFile) {
                var source = new Windows.Foundation.Uri(remoteAddress);
                return downloader.createDownload(source, destinationFile);
            }, function (e) {
                WinJS.log(`Error while creating file: ${e.message}`, "sample", "error");
            }
        );
    }

    function isWebException(title, ex, download) {
        var error = BackgroundTransfer.BackgroundTransferError.getStatus(ex.number);
        var result = (error != Windows.Web.WebErrorStatus.unknown);
        var message = result ? SdkSample.lookupEnumName(Windows.Web.WebErrorStatus, error) : ex.message;

        if (download == null) {
            WinJS.log(`${title}: ${message}`, "sample", "error");
        } else {
            WinJS.log(`${download.resultFile.name} - ${title}: ${message}`, "sample", "error");
        }

        return result;
    }
})();
