//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var composition;
    var firstVideoFile;
    var secondVideoFile;
    var mediaStreamSource;

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("chooseFirstFile").addEventListener("click", chooseFirstFileClicked, false);
            document.getElementById("chooseSecondFile").addEventListener("click", chooseSecondFileClicked, false);
            document.getElementById("appendFiles").addEventListener("click", appendFilesClicked, false);
        }
    });

    function chooseFirstFileClicked() {
        // Get first file
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        picker.fileTypeFilter.replaceAll([".mp4"]);

        picker.pickSingleFileAsync().then(function (videoFile) {
            if (videoFile == null) {            
                WinJS.log && WinJS.log("File picking cancelled", "sample", "error");
                return;
            }

            firstVideoFile = videoFile;
            firstVideoFile.openReadAsync().then(function (videoSource) {
                mediaElement.src = URL.createObjectURL(videoSource, { oneTimeOnly: true });
                chooseSecondFile.disabled = false;
                WinJS.log && WinJS.log("First video chosen", "sample", "status");
            });
        });
    }

    function chooseSecondFileClicked() {
        // Get second file
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        picker.fileTypeFilter.replaceAll([".mp4"]);

        picker.pickSingleFileAsync().then(function (videoFile) {
            if (videoFile == null) {
                WinJS.log && WinJS.log("File picking cancelled", "sample", "error");
                return;
            }

            secondVideoFile = videoFile;
            secondVideoFile.openReadAsync().then(function (videoSource) {
                mediaElement.src = URL.createObjectURL(videoSource, { oneTimeOnly: true });
                appendFiles.disabled = false;
                WinJS.log && WinJS.log("Second video chosen", "sample", "status");
            });
        });
    }

    function appendFilesClicked() {
        // Combine two video files together into one
        Windows.Media.Editing.MediaClip.createFromFileAsync(firstVideoFile).then(function (firstClip) {
            Windows.Media.Editing.MediaClip.createFromFileAsync(secondVideoFile).then(function (secondClip) {
                composition = new Windows.Media.Editing.MediaComposition();
                composition.clips.append(firstClip);
                composition.clips.append(secondClip);

                // Render to MediaElement
                mediaStreamSource = composition.generatePreviewMediaStreamSource(mediaElement.videoWidth, mediaElement.videoHeight);
                mediaElement.src = URL.createObjectURL(mediaStreamSource, { oneTimeOnly: true });
                WinJS.log && WinJS.log("Clips appended", "sample", "status");
            });
        });
    }
})();
