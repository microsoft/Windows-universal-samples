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

    const BytesPerMegaByte = 1000000;

    var BackgroundDownloader = Windows.Networking.BackgroundTransfer.BackgroundDownloader;
    var BackgroundTransferStatus = Windows.Networking.BackgroundTransfer.BackgroundTransferStatus;
    var BackgroundTransferError = Windows.Networking.BackgroundTransfer.BackgroundTransferError;
    var WebErrorStatus = Windows.Web.WebErrorStatus;

    var pauseDownloadButton;
    var resumeDownloadButton;
    var seekDownloadButton;
    var startDownloadButton;
    var seekSlider;
    var seekSliderText;
    var currentPositionSlider;
    var currentPositionSliderText;
    var downloadedInfoText;
    var downloadStatusText;
    var downloadedRangesText;
    var previousReadText;
    var currentReadText;

    var download;
    var readOperation;
    var randomAccessStream;

    var page = WinJS.UI.Pages.define("/html/scenario5_RandomAccess.html", {
        ready: function (element, options) {
            pauseDownloadButton = document.getElementById("pauseDownloadButton");
            resumeDownloadButton = document.getElementById("resumeDownloadButton");
            seekDownloadButton = document.getElementById("seekDownloadButton");
            startDownloadButton = document.getElementById("startDownloadButton");
            seekSlider = document.getElementById("seekSlider");
            seekSliderText = document.getElementById("seekSliderText");
            currentPositionSlider = document.getElementById("currentPositionSlider");
            currentPositionSliderText = document.getElementById("currentPositionSliderText");
            downloadedInfoText = document.getElementById("downloadedInfoText");
            downloadStatusText = document.getElementById("downloadStatusText");
            downloadedRangesText = document.getElementById("downloadedRangesText");
            previousReadText = document.getElementById("previousReadText");
            currentReadText = document.getElementById("currentReadText");

            pauseDownloadButton.addEventListener("click", pauseDownload_click);
            resumeDownloadButton.addEventListener("click", resumeDownload_click);
            seekDownloadButton.addEventListener("click", seekDownload_click);
            startDownloadButton.addEventListener("click", startDownload_click);
            seekSlider.addEventListener("input", seekSlider_input);
            cancelActiveDownloadsAsync().done();
        },
        unload: function (element) {
            // Cancel any outstanding reads.
            if (readOperation) {
                readOperation.cancel();
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

    function pauseDownload_click() {
        if (download) {
            download.pause();
            pauseDownloadButton.disabled = true;
            resumeDownloadButton.disabled = false;
        }
    }

    function resumeDownload_click() {
        if (download) {
            download.resume();
            pauseDownloadButton.disabled = false;
            resumeDownloadButton.disabled = true;
        }
    }

    function seekDownload_click() {
        // Cancel the ongoing read operation (if any).
        if (readOperation) {
            readOperation.cancel();
        }

        if (randomAccessStream) {
            // Update the stream's seek position to the user-selected one.
            // The new seek position will take effect on the next ReadAsync operation.
            var seekPositionInMegaBytes = seekSlider.value;
            var seekPositionInBytes = seekPositionInMegaBytes * BytesPerMegaByte;
            randomAccessStream.seek(seekPositionInBytes);
        }
    }

    function seekSlider_input() {
        seekSliderText.innerText = seekSlider.value;
    }

    function formatDownloadedRanges(ranges) {
        if (ranges.length == 0) {
            return "None";
        }
        return ranges.map(function (range) {
            return `${range.offset}-${range.offset + range.length - 1}`;
        }).join(", ");
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
    }

    function onRangesDownloaded(args) {
        // BackgroundTransfer will wait for the app's RangesDownloaded event handlers to finish executing before
        // triggering the next RangesDownloaded event. Any download progress made while the app's handlers are
        // executing get coalesced into a single RangesDownloaded event.
        //
        // When calling async APIs inside this handler, we must first take a deferral and then complete it once we
        // are done processing the event. Otherwise, BackgroundTransfer will assume that the handler is done executing
        // as soon as the synchronous portion of the handler returns, even if there are still tasks running.
        // (We don't perform any asynchronous actions in this handler, but we take a deferral for demonstration purposes.)
        var deferral = args.getDeferral();

        // AddedRanges tells you which ranges were downloaded since the last RangesDownloaded event.
        // To get the full list of downloaded ranges, use the GetDownloadedRanges method.
        var message = " Newly-downloaded ranges: " + formatDownloadedRanges(args.addedRanges);

        // A download may restart if the server contents changed while the download is in progress.
        if (args.wasDownloadRestarted) {
            message += " (download was restarted)";
        }

        // Wait for the UI update to happen before completing the deferral. That way, we can be certain that
        // the next RangesDownloaded event will not get triggered midway through the UI update.
        downloadedRangesText.innerText = formatDownloadedRanges(args.target.getDownloadedRanges());
        WinJS.log(message, "sample", "status");
        deferral.complete();
    }

    function startDownload_click() {
        // Reset the output whenever a new download attempt begins.
        downloadedInfoText.innerText = "";
        downloadStatusText.innerText = "";
        downloadedRangesText.innerText = "None";
        previousReadText.innerText = "";
        currentReadText.innerText = "";

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

            // Apps need to set the isRandomAccessRequired boolean property to TRUE (default is FALSE) before they
            // call startAsync() on a DownloadOperation object if they want to use the random access feature. After
            // opting into "random access" mode, getResultRandomAccessStreamReference() can be called.
            //
            // Setting isRandomAccessRequired to TRUE before starting the download is necessary because the
            // server-side requirements for Random Access functionality are stricter than for regular (sequential)
            // downloads, so BackgroundTransfer needs to know the app's intentions in advance. That way, it can fail
            // the transfer up front if the server does not satisfy those additional requirements (e.g., HTTP range
            // header support).
            download.isRandomAccessRequired = true;

            // Subscribe to RangesDownloaded notifications.
            download.addEventListener("rangesdownloaded", onRangesDownloaded);

            downloadedInfoText.innerText = `Downloading to ${destinationFile.name}, ${download.guid}`;

            // Start the download and read partially-downloaded content while the transfer is still ongoing.
            return startDownloadAndReadContentsAsync();
        }, function (e) {
            WinJS.log(`Error while creating file: ${e.message}`, "sample", "error");
        }).done();
    }

    function startDownloadAndReadContentsAsync() {
        // Retrieve a random access stream from the download operation. Every openReadAsync() operation returns
        // a new stream instance that is independent of previous ones (i.e., the seek position of one stream
        // isn't affected by calls on another stream).
        //
        // This sample demonstrates the direct usage of a DownloadOperation's random access stream and its
        // effects on the ongoing transfer. However, bear in mind that there are a variety of operations
        // that can manage the stream on the app's behalf for specific scenarios. For instance, a
        // DownloadOperation pointing to a video URL can be consumed by the MediaPlayer class in four easy
        // steps:
        //
        // var randomAccessStreamReference = download.getResultRandomAccessStreamReference();
        // randomAccessStreamReference.openReadAsync().then(function (stream)
        // {
        //     var mediaPlayer = new Windows.Media.Playback.MediaPlayer();
        //     mediaPlayer.source = Windows.Media.Core.MediaSource.createFromStream(stream, stream.contentType);
        // });

        var downloadPromise;
        var randomAccessStreamReference = download.getResultRandomAccessStreamReference();
        return randomAccessStreamReference.openReadAsync().then(function (stream) {
            randomAccessStream = stream;

            // Start the download. If the server doesn't support random access, the download will fail
            // with WebErrorStatus.InsufficientRangeSupport or WebErrorStatus.MissingContentLengthSupport.
            downloadPromise = download.startAsync();
            downloadPromise.then(null, null, onDownloadProgress);

            startDownloadButton.disabled = true;
            pauseDownloadButton.disabled = false;

            // Read data while the download is still ongoing. Use a 1 MB read buffer for that purpose.
            var readBuffer = new Windows.Storage.Streams.Buffer(BytesPerMegaByte);

            return readStreamAsync(downloadPromise, readBuffer);
        }).then(function () {
            // Wait for the download to complete.
            return downloadPromise;
        }).then(function () {
            WinJS.log("Download completed successfully", "sample", "status");
        }, function (e) {
            // Abandon the operation if a web exception occurs.
            return allowWebException("Execution error", e);
        }).then(function () {
            download = null;
            randomAccessStream = null;
            readOperation = null;

            startDownloadButton.disabled = false;
            pauseDownloadButton.disabled = true;
            resumeDownloadButton.disabled = true;
        });
    }

    function readStreamAsync(downloadPromise, readBuffer) {
        if (downloadPromise.operation.status != Windows.Foundation.AsyncStatus.started) {
            // Operation has completed (possibly with error).
            return;
        }

        var readOffsetInBytes = randomAccessStream.position;

        previousReadText.innerText = currentReadText.innerText;
        currentReadText.innerText = `Reading from offset ${readOffsetInBytes}`;

        readOperation = randomAccessStream.readAsync(readBuffer, readBuffer.capacity, Windows.Storage.Streams.InputStreamOptions.none);

        // Update the UI to show the current read's position.
        currentPositionSlider.value = readOffsetInBytes / BytesPerMegaByte;
        currentPositionSliderText.innerText = Math.floor(currentPositionSlider.value);

        // Wait for the read to complete.
        return readOperation.then(function (bytesRead) {
            currentReadText.innerText += `, completed with ${bytesRead.length} bytes`;

            // At this point, a real app would process the 'bytesRead' data to do something interesting
            // with it (e.g., display video and/or play audio).

            if (randomAccessStream.position >= randomAccessStream.size) {
                // We have reached EOF. Wrap around to the beginning while we wait for the download to complete.
                randomAccessStream.seek(0);
            }
        }, function (e) {
            if (e.name === "Canceled") {
                // The ongoing read was canceled by seekDownload_click(...) in order for a new download
                // position to take effect immediately.
                currentReadText.innerText += ", cancelled.";
            } else {
                // Propagate the exception.
                return WinJS.Promise.wrapError(e);
            }
        }).then(function () {
            return readStreamAsync(downloadPromise, readBuffer);
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
