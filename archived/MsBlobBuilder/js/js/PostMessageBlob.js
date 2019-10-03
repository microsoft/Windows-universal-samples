//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/PostMessageBlob.html", {
        ready: function (element, options) {
            document.getElementById("load").addEventListener("click", /*@static_cast(EventListener)*/postMessageBlob, false);
            reset();
        },
        
        unload: function () {
            reset();
        }
    });

    function postMessageBlob() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll([".jpg", ".bmp", ".gif", ".png"]);
        picker.pickSingleFileAsync().then(function (file) {
            // Check that the picker returned a file. The picker will return null if the user clicked cancel
            if (file) {
                file.openAsync(Windows.Storage.FileAccessMode.read).done(function (stream) {
                    // The blob takes ownership of stream and manages lifetime. The stream will be closed when the blob is closed.
                    var blob = MSApp.createBlobFromRandomAccessStream(file.contentType, stream);
                    document.getElementById("webFrame").contentWindow.postMessage(blob, "ms-appx-web://" + document.location.hostname);
                    blob.msClose();
            
                }, BlobSample.asyncError);
            }
            else {
                WinJS.log && WinJS.log("No file was selected", "sample", "status");
            }
        }, BlobSample.asyncError);
    }

    function reset() {
        document.getElementById("webFrame").contentWindow.postMessage("reset", "ms-appx-web://" + document.location.hostname);
    }
})();
