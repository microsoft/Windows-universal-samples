//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2-display.html", {
        ready: function (element, options) {
            document.getElementById("PickPlaylistButton").addEventListener("click", displayPlaylist, false);
        }
    });

    function displayPlaylist() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.musicLibrary;
        picker.fileTypeFilter.replaceAll(SdkSample.playlistExtensions);

        var playlist;
        picker.pickSingleFileAsync().then(function (item) {
            if (item) {
                return Windows.Media.Playlists.Playlist.loadAsync(item);
            }
            return WinJS.Promise.wrapError("No file picked.");
        }).then(function (result) {
            playlist = result;

            // Request music properties for each file in the playlist.
            var promises = playlist.files.map(function (file) {
                return file.properties.getMusicPropertiesAsync();
            });

            // Wait for all the properties to be retrieved.
            return WinJS.Promise.join(promises);
        }).done(function (results) {
            var output = "Songs in playlist: " + playlist.files.length + "\n";
            results.forEach(function (musicProperties, index) {
                output += "File: " + playlist.files[index].path + "\n";
                output += "Title: " + musicProperties.title + "\n";
                output += "Album: " + musicProperties.album + "\n";
                output += "Artist: " + musicProperties.artist + "\n\n";
            });
            WinJS.log && WinJS.log("", "sample", "status");
            document.getElementById("OutputStatus").innerText = output;
        }, function (error) {
            WinJS.log && WinJS.log(error, "sample", "error");
        });
    }
})();
