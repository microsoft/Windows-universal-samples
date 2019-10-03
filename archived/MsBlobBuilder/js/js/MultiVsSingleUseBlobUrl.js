//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var blobUrl = null;
    var videoCounter = 0;

    var page = WinJS.UI.Pages.define("/html/MultiVsSingleUseBlobUrl.html", {
        ready: function (element, options) {
            actionsChangeVisibility("hidden");
            document.getElementById("openSingle").addEventListener("click", /*static_cast(EventListener)*/openSingleUseBlobUrl, false);
            document.getElementById("openMulti").addEventListener("click", /*static_cast(EventListener)*/openMultiUseBlobUrl, false);
            document.getElementById("revoke").addEventListener("click", /*@static_cast(EventListener)*/revokeUrl, false);
            document.getElementById("displayInNewVideoTag").addEventListener("click", /*@static_cast(EventListener)*/displayInNewVideoTag, false);
            document.getElementById("reset").addEventListener("click", /*@static_cast(EventListener)*/reset, false);
            reset();
        },
        
        unload: function () {
            reset();
        }
    });

    function openSingleUseBlobUrl() {
        reset();
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll([".wmv", ".avi", ".mp4"]);
        picker.pickSingleFileAsync().done(function (file) {
            // Check that the picker returned a file. The picker will return null if the user clicked cancel.
            if (file) {
                // Create a URI for this file.
                blobUrl = URL.createObjectURL(file, {oneTimeOnly: true});
                document.getElementById("video1").src = blobUrl;

                // Show status.
                actionsChangeVisibility("visible");
                WinJS.log && WinJS.log("The src of the first video has been set to " + blobUrl + " as a single use url.\n" +
                    "Single use blob urls are revoked once they have been used once. This blob url has already been revoked for you.", "sample", "status");
            }
            else {
                WinJS.log && WinJS.log("No file was selected", "sample", "status");
            }
        }, BlobSample.asyncError);
    }

    function openMultiUseBlobUrl() {
        reset();
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll([".wmv", ".avi", ".mp4"]);
        picker.pickSingleFileAsync().done(function (file) {
            // Check that the picker returned a file. The picker will return null if the user clicked cancel.
            if (file) {
                // Create a URI for this file.
                blobUrl = URL.createObjectURL(file);
                document.getElementById("video1").src = blobUrl;

                // Show status
                actionsChangeVisibility("visible");
                WinJS.log && WinJS.log("The src of the first video has been set to " + blobUrl + " as a multi use blob url.\n" +
                    "Multi use blob urls must be revoked when you are finished with them as the blob url holds the blob object in memory.", "sample", "status");
            }
            else {
                WinJS.log && WinJS.log("No file was selected", "sample", "status");
            }
        }, BlobSample.asyncError);
    }

    function revokeUrl() {
        URL.revokeObjectURL(blobUrl);

        // Show status
        WinJS.log && WinJS.log(blobUrl + " has been revoked. Using it now in another video tag will fail as it is no longer a valid URL.", "sample", "status");
    }

    function displayInNewVideoTag() {
        // Create Id for new video
        var idString = "additionalVideo" + videoCounter;
        videoCounter++;

        var newVideo = document.createElement("video");
        newVideo.id = idString;
        newVideo.className = "leftItem";
        newVideo.autoplay = "autoplay";
        newVideo.controls = "controls";
        newVideo.src = blobUrl;
        document.getElementById("additionalVideos").appendChild(newVideo);

        // Show status
        WinJS.log && WinJS.log("The src of the second video has been set to " + blobUrl, "sample", "status");
    }

    function reset() {
        document.getElementById("video1").removeAttribute("src");
        document.getElementById("additionalVideos").innerHTML = "";
        videoCounter = 0;
        actionsChangeVisibility("hidden");
        WinJS.log && WinJS.log("", "sample", "error");

        if (blobUrl) {
            URL.revokeObjectURL(blobUrl);
            blobUrl = null;
        }
    }

    function actionsChangeVisibility(value) {
        document.getElementById("revoke").style.visibility = value;
        document.getElementById("displayInNewVideoTag").style.visibility = value;
        document.getElementById("reset").style.visibility = value;
    }

})();
