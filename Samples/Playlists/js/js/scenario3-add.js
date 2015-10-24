//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var pickPlaylistButton;
    var pickAudioButton;
    var playlist;

    var page = WinJS.UI.Pages.define("/html/scenario3-add.html", {
        ready: function (element, options) {
            pickPlaylistButton = document.getElementById("PickPlaylistButton");
            pickPlaylistButton.addEventListener("click", loadPlaylist, false)

            pickAudioButton = document.getElementById("PickAudioButton");
            pickAudioButton.addEventListener("click", addSong, false);
        }
    });

    function loadPlaylist() {
        pickAudioButton.disabled = true;
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.musicLibrary;
        picker.fileTypeFilter.replaceAll(SdkSample.playlistExtensions);

        picker.pickSingleFileAsync().then(function (file) {
            if (file) {
                return Windows.Media.Playlists.Playlist.loadAsync(file);
            }
            return WinJS.Promise.wrapError("No file picked");
        }).done(function (result) {
            playlist = result;
            pickAudioButton.disabled = false;
            WinJS.log && WinJS.log("Playlist loaded.", "sample", "status");
        }, function (error) {
            WinJS.log && WinJS.log(error, "sample", "error");
        });
    }

    function addSong() {
        if (playlist) {
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.musicLibrary;
            picker.fileTypeFilter.replaceAll(SdkSample.audioExtensions);

            var numFilesPicked = 0;
            picker.pickMultipleFilesAsync().then(function (files) {
                if (files.size > 0) {
                    numFilesPicked = files.size;

                    files.forEach(function (file) {
                        playlist.files.append(file);
                    });

                    return playlist.saveAsync();
                } else {
                    return WinJS.Promise.wrapError("No files picked.");
                }
            }).done(function (file) {
                WinJS.log && WinJS.log(numFilesPicked + " files added to playlist.", "sample", "status");
            }, function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }})();
