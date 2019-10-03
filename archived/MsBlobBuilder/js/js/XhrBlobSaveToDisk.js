//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/XhrBlobSaveToDisk.html", {
        ready: function (element, options) {
            document.getElementById("save").addEventListener("click", /*@static_cast(EventListener)*/downloadAndSave, false);
        }
    });

    var client = null;

    function downloadAndSave() {
        try {
            var url = document.getElementById("targetUrl").value;
            if (url) {
                client = new XMLHttpRequest();
                if (client) {
                    client.open("GET", url, true);
                    client.responseType = "blob";
                    client.onreadystatechange = readyStateCallback;
                    client.send(null);
                } else {
                    WinJS.log && WinJS.log("Cannot create new XMLHttpRequest object", "sample", "error");
                }
            } else {
                WinJS.log && WinJS.log("Enter the full url to an image", "sample", "error");
            }
        }
        catch (e) {
            WinJS.log && WinJS.log("Exception while using XMLHttpRequest object " + e, "sample", "error");
        }
    }

    function readyStateCallback() {
        if (client.readyState === 4) {
            if (client.status !== 200) {
                WinJS.log && WinJS.log("Unable to download blob - status code: " + client.status.toString(), "sample", "error");
            } else {
                var blob = client.response;
                writeBlobToFile(blob);
            }
        }
    }

    function writeBlobToFile(blob) {
        var filename = document.getElementById("fileName").value;
        // Open the picker to create a file to save the blob
        Windows.Storage.KnownFolders.picturesLibrary.createFileAsync(filename, Windows.Storage.CreationCollisionOption.generateUniqueName).then(function (file) {
            // Open the returned file in order to copy the data
            file.openAsync(Windows.Storage.FileAccessMode.readWrite).then(function (output) {

                // Get the IInputStream stream from the blob object
                var input = blob.msDetachStream();

                // Copy the stream from the blob to the File stream
                Windows.Storage.Streams.RandomAccessStream.copyAsync(input, output).then(function () {
                    output.flushAsync().done(function () {
                        input.close();
                        output.close();
                        WinJS.log && WinJS.log("File '" + file.name + "' saved successfully to the Pictures Library!", "sample", "status");
                    }, BlobSample.asyncError);
                }, BlobSample.asyncError);
            }, BlobSample.asyncError);
        }, BlobSample.asyncError);
    }
})();
