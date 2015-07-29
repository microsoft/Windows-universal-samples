//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var composition;
    var mediaStreamSource;
    var storageItemAccessList;
    var pickedFile;

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("chooseClip").addEventListener("click", chooseClipClicked, false);
            document.getElementById("trimClip").addEventListener("click", trimClipClicked, false);
            document.getElementById("save").addEventListener("click", saveClicked, false);

            // Make sure we don't run out of entries in StoreItemAccessList.
            // As we don't need to persist this across app sessions/pages, clearing
            // every time is sufficient for this sample
            storageItemAccessList = Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList;
            storageItemAccessList.clear();
        }
    });

    function chooseClipClicked() {
        WinJS.log && WinJS.log("", "sample", "status");

        // Get file
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        picker.fileTypeFilter.replaceAll([".mp4"]);

        picker.pickSingleFileAsync().then(function (videoFile) {
            if (videoFile == null) {
                WinJS.log && WinJS.log("File picking cancelled", "sample", "error");
                return;
            }

            pickedFile = videoFile;

            // These files could be picked from a location that we won't have access to later
            // (especially if persisting the MediaComposition to disk and loading it later). 
            // Use the StorageItemAccessList in order to keep access permissions to that
            // file for later use. Be aware that this access list needs to be cleared
            // periodically or the app will run out of entries.
            storageItemAccessList.add(pickedFile);

            pickedFile.openReadAsync().then(function (videoSource) {
                mediaElement.src = URL.createObjectURL(videoSource, { oneTimeOnly: true });
                trimClip.disabled = false;
                WinJS.log && WinJS.log("Video chosen", "sample", "status");
            });
        });
    }

    function trimClipClicked() {
        WinJS.log && WinJS.log("", "sample", "status");

        Windows.Media.Editing.MediaClip.createFromFileAsync(pickedFile).then(function (clip) {
            // Trim the front and back 25% from the clip            
            clip.trimTimeFromStart = clip.originalDuration * 0.25;
            clip.trimTimeFromEnd = clip.originalDuration * 0.25;

            // Create a MediaComposition containing the clip and set it on the MediaElement.
            composition = new Windows.Media.Editing.MediaComposition();
            composition.clips.append(clip);
            mediaStreamSource = composition.generatePreviewMediaStreamSource(mediaElement.videoWidth, mediaElement.videoHeight);
            mediaElement.src = URL.createObjectURL(mediaStreamSource, { oneTimeOnly: true });
            
            save.disabled = false;
            WinJS.log && WinJS.log("Clip trimmed", "sample", "status");
        });
    }

    function saveClicked() {
        WinJS.log && WinJS.log("Requesting file to save to", "sample", "status");

        var picker = new Windows.Storage.Pickers.FileSavePicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        picker.fileTypeChoices.insert("MP4 Files", [".mp4"]);
        picker.suggestedFileName = "TrimmedClip.mp4";

        picker.pickSaveFileAsync().then(function (file) {
            if (file != null) {
                enableButtons(false);
                var saveOperation = composition.renderToFileAsync(file, Windows.Media.Editing.MediaTrimmingPreference.precise);
                saveOperation.done(function (results) {
                    if (results != Windows.Media.Transcoding.TranscodeFailureReason.none) {
                        WinJS.log && WinJS.log("Saving was unsuccessful", "sample", "error");
                    } else {
                        WinJS.log && WinJS.log("Trimmed clip saved to file", "sample", "status");
                    }
                    enableButtons(true);
                }, function (error) {
                    WinJS.log && WinJS.log(error.message, "sample", "error");
                    enableButtons(true);
                }, function (percent) {
                    WinJS.log && WinJS.log("Save file... Progress: " + percent.toString().split(".")[0] + "%", "sample", "status");
                });
            } else {
                WinJS.log && WinJS.log("User cancelled the file selection", "sample", "error");
            }
        });
    }

    function enableButtons(isEnabled) {
        chooseClip.disabled = !isEnabled;
        save.disabled = !isEnabled;
        trimClip.disabled = !isEnabled;
    }
})();
