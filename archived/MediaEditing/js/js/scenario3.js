//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var composition;
    var pickedFile;
    var mediaStreamSource;

    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("chooseVideo").addEventListener("click", chooseVideoClicked, false);
            document.getElementById("addAudio").addEventListener("click", addAudioClicked, false);
        }
    });

    function chooseVideoClicked() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        picker.fileTypeFilter.replaceAll([".mp4"]);

        picker.pickSingleFileAsync().then(function (videoFile) {
            if (videoFile == null) {
                WinJS.log && WinJS.log("File picking cancelled", "sample", "error");
                return;
            }

            pickedFile = videoFile;
            pickedFile.openReadAsync().then(function (videoSource) {
                mediaElement.src = URL.createObjectURL(videoSource, { oneTimeOnly: true });
                addAudio.disabled = false;
                WinJS.log && WinJS.log("Video chosen", "sample", "status");
            });
        });
    }

    function addAudioClicked() {
        Windows.Media.Editing.MediaClip.createFromFileAsync(pickedFile).then(function (clip) {
            composition = new Windows.Media.Editing.MediaComposition();
            composition.clips.append(clip);

            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
            picker.fileTypeFilter.replaceAll([".mp3",".wav",".flac"]);

            picker.pickSingleFileAsync().then(function (audioFile) {
                if (audioFile == null) {
                    WinJS.log && WinJS.log("File picking cancelled", "sample", "error");
                    return;
                }

                Windows.Media.Editing.BackgroundAudioTrack.createFromFileAsync(audioFile).then(function (backgroundTrack) {
                    composition.backgroundAudioTracks.append(backgroundTrack);

                    // Render to MediaElement
                    mediaStreamSource = composition.generatePreviewMediaStreamSource(mediaElement.videoWidth, mediaElement.videoHeight);
                    mediaElement.src = URL.createObjectURL(mediaStreamSource, { oneTimeOnly: true });
                    WinJS.log && WinJS.log("Background audio added", "sample", "status");
                });
            });
        });
    }
})();
