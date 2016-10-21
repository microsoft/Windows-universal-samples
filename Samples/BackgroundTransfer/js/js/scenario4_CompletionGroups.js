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

    var page = WinJS.UI.Pages.define("/html/scenario4_CompletionGroups.html", {
        ready: function (element, options) {
            // Assign event listeners for each button on click.
            id("startDownloadsButton").addEventListener("click", startDownloadsButtonClick, false);
        }
    });

    // Entry point for the Completion Group background task.
    var taskEntryPoint = "js\\completionGroupBackgroundTask.js";

    // Count of the downloads that have completed so far.
    var downloadsCompleted;

    function createBackgroundDownloader() {
        var completionGroup = new Windows.Networking.BackgroundTransfer.BackgroundTransferCompletionGroup();

        var builder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();
        builder.taskEntryPoint = taskEntryPoint;
        builder.setTrigger(completionGroup.trigger);
        var taskRegistration = builder.register();

        var downloader = new Windows.Networking.BackgroundTransfer.BackgroundDownloader(completionGroup);
        return downloader;
    }

    function startDownloadsButtonClick() {
        // Validating the URI is required since it was received from an untrusted source (user input).
        // The URI is validated by catching exceptions thrown by the Uri constructor.
        // Note that when enabling the text box users may provide URIs to machines on the intranet that require the
        // "Private Networking" capability.
        var baseUri = null;
        try {
            baseUri = new Windows.Foundation.Uri(document.getElementById("serverAddressField").value);
        } catch (ex) {
            displayError("Error: Invalid URI. " + ex.message);
            return;
        }

        var downloader = createBackgroundDownloader();
        downloadsCompleted = 0;

        var promiseArray = [];
        for (var i = 0; i < 10; i++) {
            var uri = new Windows.Foundation.Uri(baseUri, "?id=" + String(i));
            promiseArray[i] = Windows.Storage.KnownFolders.picturesLibrary.createFileAsync(
                "picture" + String(i) + ".png", Windows.Storage.CreationCollisionOption.generateUniqueName)
                .then(function (destinationFile) {
                    var download = downloader.createDownload(uri, destinationFile);
                    download.startAsync().then(complete, complete, null);
                }, error);
        }

        // Only call enable() once all of the downloads associated with the completion group have been created.
        WinJS.Promise.join(promiseArray).done(function () {
            downloader.completionGroup.enable();
            printLog("Completion group enabled.<br/>");
        }, error);
    }

    // Completion callback.
    function complete() {
        displayStatus(String(++downloadsCompleted) + " downloads completed.");
    }

    // Error callback.
    function error(err) {
        displayException(err);
    }

    function displayException(err) {
        var message;
        if (err.stack) {
            message = err.stack;
        } else {
            message = err.message;
        }

        var errorStatus = Windows.Networking.BackgroundTransfer.BackgroundTransferError.getStatus(err.number);
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

})();
