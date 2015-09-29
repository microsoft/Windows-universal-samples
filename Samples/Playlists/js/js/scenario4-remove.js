//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario4-remove.html", {
        ready: function (element, options) {
            document.getElementById("PickPlaylistButton").addEventListener("click", removeSong, false);
        }
    });

    function removeSong() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.musicLibrary;
        picker.fileTypeFilter.replaceAll(SdkSample.playlistExtensions);

        picker.pickSingleFileAsync().then(function (file) {
            if (file) {
                return Windows.Media.Playlists.Playlist.loadAsync(file);
            }
            return WinJS.Promise.wrapError("No file picked.");
        }).then(function (playlist) {
            playlist.files.removeAtEnd();
            return playlist.saveAsync();
        }).done(function (file) {
            WinJS.log && WinJS.log("Item removed from end of playlist.", "sample", "status");
        }, function (error) {
            WinJS.log && WinJS.log(error, "sample", "error");
        });
    }
})();
