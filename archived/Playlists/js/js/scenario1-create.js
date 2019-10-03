//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1-create.html", {
        ready: function (element, options) {
            document.getElementById("PickAudioButton").addEventListener("click", pickAudio, false);
        }
    });

    function pickAudio() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.musicLibrary;
        picker.fileTypeFilter.replaceAll(SdkSample.audioExtensions);

        picker.pickMultipleFilesAsync().done(function (files) {
            if (files.size > 0) {
                var playlist = new Windows.Media.Playlists.Playlist();

                files.forEach(function (file) {
                    playlist.files.append(file);
                });

                var folder = Windows.Storage.KnownFolders.musicLibrary;
                var name = "Sample";
                var collisionOption = Windows.Storage.NameCollisionOption.replaceExisting;
                var format = Windows.Media.Playlists.PlaylistFormat.windowsMedia;
                playlist.saveAsAsync(folder, name, collisionOption, format).done(function (file) {
                    WinJS.log && WinJS.log(file.name + " was created and saved with " + playlist.files.size + " files.", "sample", "status");
                }, function (error) {
                    WinJS.log && WinJS.log(error, "sample", "error");
                });
            }
            else {
                WinJS.log && WinJS.log("No files picked.", "sample", "error");
            }
        }, function (error) {
            WinJS.log && WinJS.log(error, "sample", "error");
        });
    }
})();
