//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/FileToBlobUrl.html", {
        ready: function (element, options) {
            document.getElementById("reset").addEventListener("click", /*@static_cast(EventListener)*/reset, false);
            document.getElementById("openImage").addEventListener("click", /*@static_cast(EventListener)*/openImage, false);
            document.getElementById("openVideo").addEventListener("click", /*@static_cast(EventListener)*/openVideo, false);
            document.getElementById("openAudio").addEventListener("click", /*@static_cast(EventListener)*/openAudio, false);
            reset();
        },
        
        unload: function () {
            reset();
        }
    });

    function reset() {
        WinJS.log && WinJS.log("", "sample", "error");
        document.getElementById("image").src = "/images/placeholder-sdk.png";
        document.getElementById("video").removeAttribute("src");
        document.getElementById("audio").removeAttribute("src");
        actionsChangeVisibility("hidden");
    }

    function openImage() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll([".jpg", ".bmp", ".gif", ".png"]);
        picker.pickSingleFileAsync().done(function (file) {
            // Check that the picker returned a file. The picker will return null if the user clicked cancel.
            if (file) {
                var imageBlobUrl = URL.createObjectURL(file, {oneTimeOnly: true});
                document.getElementById("image").src = imageBlobUrl;

                // Show status
                actionsChangeVisibility("visible");
                WinJS.log && WinJS.log("The src of Image has been set to: " + imageBlobUrl, "sample", "status");
            }
            else {
                WinJS.log && WinJS.log("No file was selected", "sample", "status");
            }
        }, BlobSample.asyncError);
    }

    function openVideo() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll([".wmv", ".avi", ".mp4"]);
        picker.pickSingleFileAsync().done(function (file) {
            // Check that the picker returned a file. The picker will return null if the user clicked cancel.
            if (file) {
                var videoBlobUrl = URL.createObjectURL(file, {oneTimeOnly: true});
                document.getElementById("video").src = videoBlobUrl;

                // Show status
                actionsChangeVisibility("visible");
                WinJS.log && WinJS.log("The src of Video has been set to: " + videoBlobUrl, "sample", "status");
            }
            else {
                WinJS.log && WinJS.log("No file was selected", "sample", "status");
            }
        }, BlobSample.asyncError);
    }

    function openAudio() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll([".mp3", ".wav"]);
        picker.pickSingleFileAsync().done(function (file) {
            // Check that the picker returned a file. The picker will return null if the user clicked cancel
            if (file) {
                var audioBlobUrl = URL.createObjectURL(file, {oneTimeOnly: true});
                document.getElementById("audio").src = audioBlobUrl;

                // Show status
                actionsChangeVisibility("visible");
                WinJS.log && WinJS.log("The src of Audio has been set to: " + audioBlobUrl, "sample", "status");
            }
            else {
                WinJS.log && WinJS.log("No file was selected", "sample", "status");
            }
        }, BlobSample.asyncError);
    }

    function actionsChangeVisibility(value) {
        document.getElementById("reset").style.visibility = value;
    }
})();
