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

    var BackgroundDownloader = Windows.Networking.BackgroundTransfer.BackgroundDownloader;
    var BackgroundTransferStatus = Windows.Networking.BackgroundTransfer.BackgroundTransferStatus;
    var BackgroundTransferError = Windows.Networking.BackgroundTransfer.BackgroundTransferError;
    var WebErrorStatus = Windows.Web.WebErrorStatus;

    var startDownloadButton;
    var configureRecoverableErrorsCheckBox;
    var downloadedInfoText;
    var downloadStatusText;

    var download;
    var downloadPromise;

    var page = WinJS.UI.Pages.define("/html/scenario6_RecoverableErrors.html", {
        ready: function (element, options) {
            startDownloadButton = document.getElementById("startDownloadButton");
            configureRecoverableErrorsCheckBox = document.getElementById("configureRecoverableErrorsCheckBox");
            downloadedInfoText = document.getElementById("downloadedInfoText");
            downloadStatusText = document.getElementById("downloadStatusText");

            startDownloadButton.addEventListener("click", startDownload_click);
        },
        unload: function (element) {
            // Cancel any outstanding download.
            if (downloadPromise) {
                downloadPromise.cancel();
            }
        }
    });

    // Helper function to allow web exceptions but propagate all other exceptions.
    function allowWebException(title, e) {
        if (isWebException(title, e)) {
            return;
        } else {
            // Propagate the exception.
            return WinJS.Promise.wrapError(e);
        }
    }

    function cancelActiveDownloadsAsync() {
        return BackgroundDownloader.getCurrentDownloadsAsync().then(function (downloads) {
            // If previous sample instances/scenarios started transfers that haven't completed yet, cancel them now
            // so that we can start this scenario cleanly.
            if (downloads.length > 0) {
                var promises = downloads.map(function (downloadOperation) {
                    // Cancel each download and ignore the cancellation exception.
                    var downloadPromise = downloadOperation.attachAsync();
                    downloadPromise.cancel();
                    return downloadPromise.then(null, function (e) {
                        if (e.name === "Canceled") {
                            return;
                        } else {
                            // Propagate the exception.
                            return WinJS.Promise.wrapError(e);
                        }
                    });
                })
                return WinJS.Promise.join(promises).then(function () {
                    WinJS.log(`Canceled ${downloads.length} downloads.`, "sample", "status");
                });
            }
        }, function (e) {
            // Don't worry if the old downloads encountered web exceptions.
            return allowWebException("Discovery error", e);
        });
    }

    function onDownloadProgress(progress) {
        // We capture a snapshot of DownloadOperation.Progress because
        // it is is updated in real-time while the operation is ongoing.
        var currentProgress = progress.progress;

        // Prepare the progress message to display in the UI.
        var percent = 100;
        if (currentProgress.totalBytesToReceive > 0) {
            percent = Math.floor(currentProgress.bytesReceived * 100 / currentProgress.totalBytesToReceive);
        }
        downloadStatusText.innerText = `${SdkSample.lookupEnumName(BackgroundTransferStatus, currentProgress.status)} - ${percent}% downloaded.`;

        if (currentProgress.hasRestarted) {
            WinJS.log("Download has restarted.", "sample", "status");
        }

        if (currentProgress.status === BackgroundTransferStatus.pausedRecoverableWebErrorStatus) {
            // The only value we put in recoverableWebErrorStatuses is WebErrorStatus.forbidden,
            // so that will be the only value observed here.
            console.assert(download.currentWebErrorStatus === WebErrorStatus.forbidden);

            WinJS.log("URL has expired.", "sample", "error");

            // The URL expired. Ask the user for information so we can get a new URL.
            // A real program might prompt for an authorization key.
            var dialog = new Windows.UI.Popups.MessageDialog("The download URL has expired. Click OK to resume the download.",
                "Reauthorize download");
            dialog.commands.push(new Windows.UI.Popups.UICommand("OK", null, true));
            dialog.commands.push(new Windows.UI.Popups.UICommand("Cancel", null, false));
            dialog.defaultCommandIndex = 0;
            dialog.cancelCommandIndex = 1;
            dialog.showAsync().done(function (result) {
                if (result.id) {
                    // For the purpose of this sample, we simply remove "?shouldExpire=yes" from the URL
                    // to indicate that the sample server should treat it like a new, unexpired URL.
                    var originalUrlString = download.requestedUri.absoluteUri;
                    var newUrlString = originalUrlString.replace("?shouldExpire=yes", "");

                    WinJS.log("Updating URL and resuming the download.", "sample", "status");
                    download.requestedUri = new Windows.Foundation.Uri(newUrlString);
                    download.resume();
                } else {
                    // Cancel the download.
                    downloadPromise.cancel();
                }
            });
        }
    }

    function startDownload_click() {
        // Reset the output whenever a new download attempt begins.
        downloadedInfoText.innerText = "";
        downloadStatusText.innerText = "";
        WinJS.log("", "sample", "status");

        // The URI is validated by catching exceptions thrown by the Uri constructor.
        var uri = null;
        try {
            uri = new Windows.Foundation.Uri(document.getElementById("serverAddressField").value);
        } catch (error) {
            WinJS.log("Error: Invalid URI. " + error.message, "sample", "error");
            return;
        }

        var destination = document.getElementById("fileNameField").value.trim();
        if (destination == "") {
            WinJS.log("A local file name is required.", "sample", "error");
            return;
        }

        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            return picturesLibrary.createFileAsync(destination, Windows.Storage.CreationCollisionOption.generateUniqueName);
        }).then(function (destinationFile) {
            var downloader = new BackgroundDownloader();
            download = downloader.createDownload(uri, destinationFile);

            // Opt into "random access" mode. Transfers configured this way have full support for resumable URL updates.
            // If the timestamp or file size of the updated URL is different from that of the previous URL, the download
            // will restart from scratch. Otherwise, the transfer will resume from the same position using the new URL.
            //
            // Due to OS limitations, downloads that don't opt into "random access" mode will always restart from scratch
            // whenever their URL is updated.
            download.isRandomAccessRequired = true;

            if (configureRecoverableErrorsCheckBox.checked) {
                // Declare HTTP 403 (WebErrorStatus.Forbidden) as a recoverable error.
                download.recoverableWebErrorStatuses.push(WebErrorStatus.forbidden);
            }

            downloadedInfoText.innerText = `Downloading to ${destinationFile.name}, ${download.guid}`;

            // Start the download and wait for it to complete.
            return handleDownloadAsync();
        }, function (e) {
            WinJS.log(`Error while creating file: ${e.message}`, "sample", "error");
        }).done();
    }

    function handleDownloadAsync() {
        startDownloadButton.disabled = true;

        downloadPromise = download.startAsync();
        // Wait for the download to complete.
        return downloadPromise.then(function () {
            WinJS.log("Download completed successfully", "sample", "status");
        }, function (e) {
            if (e.name === "Canceled") {
                // Download was canceled.
                return;
            }
            // Abandon the operation if a web exception occurs.
            return allowWebException("Execution error", e);
        }, onDownloadProgress).then(function () {
            download = null;
            startDownloadButton.disabled = false;
        });
    }

    function isWebException(title, ex) {
        var error = BackgroundTransferError.getStatus(ex.number);
        var result = (error != WebErrorStatus.unknown);
        var message = isWebException ? SdkSample.lookupEnumName(WebErrorStatus, error) : e.message;
        if (!download) {
            WinJS.log(`${title}: ${message}`, "sample", "error");
        } else {
            WinJS.log(`${download.guid} - ${title}: ${message}`, "sample", "error");
        }

        return result;
    }

})();
