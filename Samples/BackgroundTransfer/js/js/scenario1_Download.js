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

    var page = WinJS.UI.Pages.define("/html/scenario1_Download.html", {
        ready: function (element, options) {
            // Assign event listeners for each button on click.
            id("startDownloadButton").addEventListener("click", startDownload, false);
            id("startHighPriorityDownloadButton").addEventListener("click", startHighPriorityDownload, false);
            id("cancelAllButton").addEventListener("click", cancelAll, false);
            id("pauseAllButton").addEventListener("click", pauseAll, false);
            id("resumeAllButton").addEventListener("click", resumeAll, false);

            // Enumerate the downloads that were going on in the background while the app was closed.
            BackgroundTransfer.BackgroundDownloader.getCurrentDownloadsAsync().done(function (downloads) {
                printLog("Loading background downloads: " + downloads.size + "<br/>");

                // If downloads from previous application state exist, reassign callbacks and persist to global array.
                for (var i = 0; i < downloads.size; i++) {
                    var download = new DownloadOperation();
                    download.load(downloads[i]);
                    downloadOperations.push(download);
                }
            });
        }
    });

    // Global array used to persist operations.
    var downloadOperations = [];

    function backgroundTransferStatusToString(transferStatus) {
        switch (transferStatus)
        {
            case BackgroundTransfer.BackgroundTransferStatus.idle:
                return "Idle";
            case BackgroundTransfer.BackgroundTransferStatus.running:
                return "Running";
            case BackgroundTransfer.BackgroundTransferStatus.pausedByApplication:
                return "PausedByApplication";
            case BackgroundTransfer.BackgroundTransferStatus.pausedCostedNetwork:
                return "PausedCostedNetwork";
            case BackgroundTransfer.BackgroundTransferStatus.pausedNoNetwork:
                return "PausedNoNetwork";
            case BackgroundTransfer.BackgroundTransferStatus.completed:
                return "Completed";
            case BackgroundTransfer.BackgroundTransferStatus.canceled:
                return "Canceled";
            case BackgroundTransfer.BackgroundTransferStatus.error:
                return "Error";
            case BackgroundTransfer.BackgroundTransferStatus.pausedSystemPolicy:
                return "PausedSystemPolicy";
            default:
                return "Unknown";
        }
    }

    // Class associated with each download.
    function DownloadOperation() {
        var download = null;
        var promise = null;

        this.start = function (uri, fileName, priority) {
            // Asynchronously create the destination file in the pictures folder.
            Windows.Storage.KnownFolders.picturesLibrary.createFileAsync(
                fileName, Windows.Storage.CreationCollisionOption.generateUniqueName)
                .done(function (destinationFile) {
                    var downloader = new BackgroundTransfer.BackgroundDownloader();

                    // Create a new download operation.
                    download = downloader.createDownload(uri, destinationFile);

                    printLog("Downloading " + uri.absoluteUri + " to " + destinationFile.name + " with " +
                        (priority === BackgroundTransfer.BackgroundTransferPriority.high ? "High" : "Default") +
                        " priority, " + download.guid + "<br/>");
                    download.priority = priority;

                    // Start the download and persist the promise to be able to cancel the download.
                    promise = download.startAsync().then(complete, error, progress);
                }, error);
        };

        // On application activation, reassign callbacks for a download
        // operation persisted from previous application state.
        this.load = function (loadedDownload) {
            download = loadedDownload;
            printLog("Discovered background download: " + download.guid + ", Status: " +
                backgroundTransferStatusToString(download.progress.status) + "<br/>");
            promise = download.attachAsync().then(complete, error, progress);
        };

        // Cancel download.
        this.cancel = function () {
            if (promise !== null) {
                promise.cancel();
                promise = null;
                printLog("Canceled: " + download.guid + "<br/>");
            } else {
                printLog("Download " + download.guid + " is already canceled.<br/>");
            }
        };

        // Resume download - download will restart if server does not allow range-requests.
        this.resume = function () {
            if (download !== null) {
                // DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
                // we must make a local copy so that we can have a consistent view of that ever-changing state
                // throughout this function's lifetime.
                var currentProgress = download.progress;

                if (currentProgress.status === BackgroundTransfer.BackgroundTransferStatus.pausedByApplication) {
                    download.resume();
                    printLog("Resumed: " + download.guid + "<br/>");
                } else {
                    printLog("Skipped: " + download.guid + ", Status: " +
                        backgroundTransferStatusToString(currentProgress.status) + "<br/>");
                }
            }
        };

        // Pause download.
        this.pause = function () {
            if (download !== null) {
                // DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
                // we must make a local copy so that we can have a consistent view of that ever-changing state
                // throughout this function's lifetime.
                var currentProgress = download.progress;

                if (currentProgress.status === BackgroundTransfer.BackgroundTransferStatus.running) {
                    download.pause();
                    printLog("Paused: " + download.guid + "<br/>");
                } else {
                    printLog("Skipped: " + download.guid + ", Status: " +
                        backgroundTransferStatusToString(currentProgress.status) + "<br/>");
                }
            }
        };

        // Returns true if this is the download identified by the guid.
        this.hasGuid = function (guid) {
            return download.guid === guid;
        };

        // Removes download operation from global array.
        function removeDownload(guid) {
            downloadOperations.forEach(function (operation, index) {
                if (operation.hasGuid(guid)) {
                    downloadOperations.splice(index, 1);
                }
            });
        }

        // Progress callback.
        function progress() {
            // DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
            // we must make a local copy so that we can have a consistent view of that ever-changing state throughout
            // this function's lifetime.
            var currentProgress = download.progress;

            printLog("Progress: " + download.guid + ", Status: " +
                backgroundTransferStatusToString(currentProgress.status) + "<br/>");

            var percent = 100;
            if (currentProgress.totalBytesToReceive > 0) {
                percent = currentProgress.bytesReceived * 100 / currentProgress.totalBytesToReceive;
            }

            printLog(" - Transferred bytes: " + String(currentProgress.bytesReceived) + " of " +
                String(currentProgress.totalBytesToReceive) + ", " + percent.toFixed(0) + "%" + "<br/>");

            if (currentProgress.hasRestarted) {
                printLog(" - Download restarted <br/>");
            }

            if (currentProgress.hasResponseChanged) {
                // We have received new response headers from the server.
                // Be aware that GetResponseInformation() returns null for non-HTTP transfers (e.g., FTP).
                var response = download.getResponseInformation();
                var headersCount = response !== null ? response.headers.size : 0;

                printLog(" - Response updated; Header count: " + String(headersCount) + "<br/>");

                // If you want to stream the response data this is a good time to start.
                // download.GetResultStreamAt(0);
            }
        }

        // Completion callback.
        function complete() {
            removeDownload(download.guid);

            try {
                var responseInfo = download.getResponseInformation();

                // 'getResponseInformation()' returns null for non-HTTP transfers (e.g., FTP).
                var statusCode = responseInfo !== null ? String(responseInfo.statusCode) : "";

                printLog("Completed: " + download.guid + ", Status Code: " + statusCode + "<br/>");
                displayStatus("Completed: " + download.guid + ", Status Code: " + statusCode);
            } catch (err) {
                displayException(err);
            }
        }

        // Error callback.
        function error(err) {
            if (download !== null) {
                removeDownload(download.guid);
                printLog(download.guid + " - download completed with error.<br/>");
            }
            displayException(err);
        }
    }

    function displayException(err) {
        var message;
        if (err.stack) {
            message = err.stack;
        } else {
            message = err.message;
        }

        var errorStatus = BackgroundTransfer.BackgroundTransferError.getStatus(err.number);
        if (errorStatus === Windows.Web.WebErrorStatus.cannotConnect) {
            message = "App cannot connect. Network may be down, connection was refused or the host is unreachable.";
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

    function startDownload() {
        downloadFile(BackgroundTransfer.BackgroundTransferPriority.default);
    }

    function startHighPriorityDownload() {
        downloadFile(BackgroundTransfer.BackgroundTransferPriority.high);
    }

    function downloadFile(priority) {
        // Instantiate downloads.
        var newDownload = new DownloadOperation();

        // Pass the uri and the file name to be stored on disk to start the download.
        var fileName = document.getElementById("fileNameField").value;
        if (fileName === "") {
            displayError("A local file name is required.");
            return;
        }

        // Validating the URI is required since it was received from an untrusted source (user input).
        // The URI is validated by catching exceptions thrown by the Uri constructor.
        // Note that when enabling the text box users may provide URIs to machines on the intranet that require the
        // "Private Networking" capability.
        var uri = null;
        try {
            uri = new Windows.Foundation.Uri(document.getElementById("serverAddressField").value);
        } catch (error) {
            displayError("Error: Invalid URI. " + error.message);
            return;
        }

        newDownload.start(uri, fileName, priority);

        // Persist the download operation in the global array.
        downloadOperations.push(newDownload);
    }

    // Cancel all downloads.
    function cancelAll() {
        printLog("Canceling Downloads: " + String(downloadOperations.length) + "<br/>");
        for (var i = 0; i < downloadOperations.length; i++) {
            downloadOperations[i].cancel();
        }
    }

    // Pause all downloads.
    function pauseAll() {
        printLog("Downloads: " + String(downloadOperations.length) + "<br/>");
        for (var i = 0; i < downloadOperations.length; i++) {
            downloadOperations[i].pause();
        }
    }

    // Resume all downloads.
    function resumeAll() {
        printLog("Downloads: " + String(downloadOperations.length) + "<br/>");
        for (var i = 0; i < downloadOperations.length; i++) {
            downloadOperations[i].resume();
        }
    }

})();
