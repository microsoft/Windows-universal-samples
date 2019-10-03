//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var composition;
    var baseVideoFile;
    var overlayVideoFile;
    var mediaStreamSource;

    var page = WinJS.UI.Pages.define("/html/scenario4.html", {
        ready: function (element, options) {
            document.getElementById("chooseBaseVideo").addEventListener("click", chooseBaseVideoClicked, false);
            document.getElementById("chooseOverlayVideo").addEventListener("click", chooseOverlayVideoClicked, false);
        }
    });

    function chooseBaseVideoClicked() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        picker.fileTypeFilter.replaceAll([".mp4"]);

        picker.pickSingleFileAsync().then(function (videoFile) {
            if (videoFile == null) {
                WinJS.log && WinJS.log("File picking cancelled", "sample", "error");
                return;
            }

            baseVideoFile = videoFile;
            baseVideoFile.openReadAsync().then(function (videoSource) {
                mediaElement.src = URL.createObjectURL(videoSource, { oneTimeOnly: true });
                chooseOverlayVideo.disabled = false;
                WinJS.log && WinJS.log("Base video chosen", "sample", "status");
            });
        });
    }

    function chooseOverlayVideoClicked() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        picker.fileTypeFilter.replaceAll([".mp4"]);

        picker.pickSingleFileAsync().then(function (videoFile) {
            if (videoFile == null) {
                WinJS.log && WinJS.log("File picking cancelled", "sample", "error");
                return;
            }

            overlayVideoFile = videoFile;
            createOverlays();
            WinJS.log && WinJS.log("Overlay video chosen", "sample", "status");
        });
    }

    function createOverlays() {
        Windows.Media.Editing.MediaClip.createFromFileAsync(baseVideoFile).then(function (baseClip) {
            composition = new Windows.Media.Editing.MediaComposition();
            composition.clips.append(baseClip);

            Windows.Media.Editing.MediaClip.createFromFileAsync(overlayVideoFile).then(function (overlayClip) {
                // Overlay video in upper left corner, retain its native aspect ratio
                var encodingProperties = overlayClip.getVideoEncodingProperties();
                var videoOverlayPosition = {
                    height: mediaElement.videoHeight / 3,
                    width: encodingProperties.width / encodingProperties.height * (mediaElement.videoHeight / 3),
                    x: 0,
                    y: 0
                };

                var videoOverlay = new Windows.Media.Editing.MediaOverlay(overlayClip);
                videoOverlay.position = videoOverlayPosition;
                videoOverlay.opacity = 0.75;

                var overlayLayer = new Windows.Media.Editing.MediaOverlayLayer();
                overlayLayer.overlays.append(videoOverlay);
                composition.overlayLayers.append(overlayLayer);

                // Render to MediaElement
                mediaStreamSource = composition.generatePreviewMediaStreamSource(mediaElement.videoWidth, mediaElement.videoHeight);
                mediaElement.src = URL.createObjectURL(mediaStreamSource, { oneTimeOnly: true });
                WinJS.log && WinJS.log("Overlays created", "sample", "status");
            });
        });
    }
})();
