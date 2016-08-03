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

    var page = WinJS.UI.Pages.define("/html/scenario2_Upload.html", {
        ready: function (element, options) {
            // Assign event listeners for each button on click.
            id("startUploadButton").addEventListener("click", uploadFile, false);
            id("startMultipartUploadButton").addEventListener("click", uploadFiles, false);
            id("cancelAllButton").addEventListener("click", cancelAll, false);

            // Enumerate the uploads that were going on in the background while the app was closed.
            BackgroundTransfer.BackgroundUploader.getCurrentUploadsAsync().done(function (uploads) {
                printLog("Loading background uploads: " + uploads.size + "<br/>");

                // If uploads from previous application state exist, reassign callbacks and persist to global array.
                for (var i = 0; i < uploads.size; i++) {
                    var upload = new UploadOperation();
                    upload.load(uploads[i]);
                    uploadOperations.push(upload);
                }
            });

            if (Windows.ApplicationModel.Activation.ActivationKind.pickFileContinuation !== undefined &&
                options !== undefined &&
                options.activationKind === Windows.ApplicationModel.Activation.ActivationKind.pickFileContinuation) {
                continueFileOpenPicker(options.activatedEventArgs);
            }
        }
    });

    function continueFileOpenPicker(args) {
        if (args.length > 0) {
            var uri = new Windows.Foundation.Uri(args[0].continuationData["uri"]);
            if (args[0].files.length === 1) {
                uploadSingleFileAsync(uri, args[0].files[0]).done(null, displayException);
            } else if (args[0].files.length > 1) {
                uploadMultipleFilesAsync(uri, args[0].files).done(null, displayException);
            }
        }
    }

    // Global array used to persist operations.
    var uploadOperations = [];

    var maxUploadFileSize = 100 * 1024 * 1024; // 100 MB (arbitrary limit chosen for this sample)

    function backgroundTransferStatusToString(transferStatus) {
        switch (transferStatus) {
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

    // Class associated with each upload.
    function UploadOperation() {
        var upload = null;
        var promise = null;

        this.start = function (uri, file) {
            var uploader = new BackgroundTransfer.BackgroundUploader();

            // Set a header, so the server can save the file (this is specific to the sample server).
            uploader.setRequestHeader("Filename", file.name);

            // Create a new upload operation.
            upload = uploader.createUpload(uri, file);

            printLog("Uploading " + file.name + " to " + uri.absoluteUri + ", " + upload.guid + "<br/>");

            // Start the upload and persist the promise to be able to cancel the upload.
            promise = upload.startAsync().then(complete, error, progress);
        };

        this.startMultipart = function (uri, files) {
            var fileNames = "";
            var uploader = new BackgroundTransfer.BackgroundUploader();
            var contentParts = [];
            files.forEach(function (file, index) {
                fileNames += (index === 0) ? file.name : ", " + file.name;
                var part = new BackgroundTransfer.BackgroundTransferContentPart("File" + index, file.name);
                part.setFile(file);
                contentParts.push(part);
            });

            // Create a new upload operation.
            uploader.createUploadAsync(uri, contentParts).then(function (uploadOperation) {
                // Start the upload and persist the promise to be able to cancel the upload.
                upload = uploadOperation;

                printLog("Uploading " + fileNames + " to " + uri.absoluteUri + ", " + upload.guid + "<br/>");
                promise = uploadOperation.startAsync().then(complete, error, progress);
            });
        };

        // On application activation, reassign callbacks for a upload
        // operation persisted from previous application state.
        this.load = function (loadedUpload) {
            upload = loadedUpload;
            printLog("Discovered background upload: " + upload.guid + " , Status: " +
                backgroundTransferStatusToString(upload.progress.status) + "<br/>");
            promise = upload.attachAsync().then(complete, error, progress);
        };

        // Cancel upload.
        this.cancel = function () {
            if (promise !== null) {
                promise.cancel();
                promise = null;
                printLog("Canceled: " + upload.guid + "<br/>");
            } else {
                printLog("Upload " + upload.guid + " is already canceled.<br/>");
            }
        };

        // Returns true if this is the upload identified by the guid.
        this.hasGuid = function (guid) {
            return upload.guid === guid;
        };

        // Removes upload operation from global array.
        function removeUpload(guid) {
            uploadOperations.forEach(function (operation, index) {
                if (operation.hasGuid(guid)) {
                    uploadOperations.splice(index, 1);
                }
            });
        }

        // Progress callback.
        function progress() {
            // UploadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
            // we must make a local copy so that we can have a consistent view of that ever-changing state throughout
            // this function's lifetime.
            var currentProgress = upload.progress;

            printLog("Progress: " + upload.guid + ", Status: " +
                backgroundTransferStatusToString(currentProgress.status) + "<br/>");

            var percent = 100;
            if (currentProgress.totalBytesToSend > 0) {
                percent = currentProgress.bytesSent * 100 / currentProgress.totalBytesToSend;
            }

            printLog(" - Sent bytes: " + String(currentProgress.bytesSent) + " of " +
                String(currentProgress.totalBytesToSend) + " (" + percent.toFixed(0) + "%), Received bytes: " +
                String(currentProgress.bytesReceived) + " of " + String(currentProgress.totalBytesToReceive) +
                "<br/>");

            if (currentProgress.hasRestarted) {
                printLog(" - Upload restarted <br/>");
            }

            if (currentProgress.hasResponseChanged) {
                // We have received new response headers from the server.
                printLog(" - Response updated; Header count: " +
                    String(upload.getResponseInformation().headers.size) + "<br/>");

                // If you want to stream the response data this is a good time to start.
                // upload.GetResultStreamAt(0);
            }
        }

        // Completion callback.
        function complete() {
            removeUpload(upload.guid);

            printLog("Completed: " + upload.guid + ", Status Code: " +
                String(upload.getResponseInformation().statusCode) + "<br/>");
            displayStatus("Completed: " + upload.guid + ", Status Code: " +
                String(upload.getResponseInformation().statusCode));
        }

        // Error callback.
        function error(err) {
            if (upload !== null) {
                removeUpload(upload.guid);
                printLog(upload.guid + " - upload completed with error.<br/>");
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

    function uploadFile() {
        // Validating the URI is required since it was received from an untrusted source (user input).
        // The URI is validated by catching exceptions thrown by the Uri constructor.
        // Note that when enabling the text box users may provide URIs to machines on the intranet that require
        // the "Private Networking" capability.
        var uri = null;
        try {
            uri = new Windows.Foundation.Uri(document.getElementById("serverAddressField").value);
        } catch (error) {
            displayError("Error: Invalid URI. " + error.message);
            return;
        }

        var filePicker = new Windows.Storage.Pickers.FileOpenPicker();
        filePicker.fileTypeFilter.replaceAll(["*"]);
        filePicker.pickSingleFileAsync().then(function (file) {
            uploadSingleFileAsync(uri, file);
        }).done(null, displayException);
    }

    function uploadSingleFileAsync(uri, file)
    {
        if (!file) {
            displayError("Error: No file selected.");
            return;
        }

        return file.getBasicPropertiesAsync().then(function (properties) {
            if (properties.size > maxUploadFileSize) {
                displayError("Selected file exceeds max. upload file size (" +
                    String(maxUploadFileSize / (1024 * 1024)) + " MB).");
                return;
            }

            var upload = new UploadOperation();
            upload.start(uri, file);

            // Persist the upload operation in the global array.
            uploadOperations.push(upload);
        });
    }

    function uploadFiles() {
        // Validating the URI is required since it was received from an untrusted source (user input).
        // The URI is validated by catching exceptions thrown by the Uri constructor.
        // Note that when enabling the text box users may provide URIs to machines on the intranet that require
        // the "Private Networking" capability.
        var uri = null;
        try {
            uri = new Windows.Foundation.Uri(document.getElementById("serverAddressField").value);
        } catch (error) {
            displayError("Error: Invalid URI. " + error.message);
            return;
        }

        var filePicker = new Windows.Storage.Pickers.FileOpenPicker();
        filePicker.fileTypeFilter.replaceAll(["*"]);
        filePicker.pickMultipleFilesAsync().then(function (files) {
            uploadMultipleFilesAsync(uri, files);
        }).done(null, displayException);
    }

    function uploadMultipleFilesAsync(uri, files) {
        var promise = validateFilesAsync(files);
        if (promise === null) {
            return;
        }

        return promise.then(function (validatedFiles) {
            if (!validatedFiles) {
                return;
            }

            var upload = new UploadOperation();
            upload.startMultipart(uri, validatedFiles);

            // Persist the upload operation in the global array.
            uploadOperations.push(upload);
        });
    }

    function validateFilesAsync(files) {
        if (files.size === 0) {
            displayError("Error: No file selected.");
            return;
        }

        var getPropertiesPromises = [];
        var totalFileSize = 0;

        // Get file size of all files. If the sum exceeds the maximum upload size, return null to indicate
        // invalid files.
        files.forEach(function (file, index) {
            getPropertiesPromises.push(file.getBasicPropertiesAsync().then(function (properties) {
                totalFileSize += properties.size;
            }));
        });

        return WinJS.Promise.join(getPropertiesPromises).then(function () {
            if (totalFileSize > maxUploadFileSize) {
                displayError("Size of selected files exceeds max. upload file size (" +
                    String(maxUploadFileSize / (1024 * 1024)) + " MB).");
                return null;
            }

            return files;
        });
    }

    // Cancel all uploads.
    function cancelAll() {
        printLog("Canceling Uploads: " + String(uploadOperations.length) + "<br/>");
        for (var i = 0; i < uploadOperations.length; i++) {
            uploadOperations[i].cancel();
        }
    }

})();
