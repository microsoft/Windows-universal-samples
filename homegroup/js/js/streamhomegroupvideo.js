//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/StreamHomegroupVideo.html", {
        ready: function (element, options) {
            document.getElementById("streamVideo").addEventListener("click", streamUNC, false);
        }
    });

    function streamUNC() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.viewMode = Windows.Storage.Pickers.PickerViewMode.thumbnail;
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.homeGroup;
        // We are only searching for mp4 and wmv files
        picker.fileTypeFilter.replaceAll([".mp4", ".wmv"]);
        var vidPlayer = document.getElementById("player");
        vidPlayer.style.visibility = "hidden";
        vidPlayer.pause();
        picker.pickSingleFileAsync().done(function (file) {
            if (file) {
                // The video tag has built in capabilities to stream the video over the network.
                vidPlayer.src = URL.createObjectURL(file, { oneTimeOnly: true });
                vidPlayer.style.visibility = "visible";
                vidPlayer.play();
            }
        },
        function (file) {
            WinJS.log && WinJS.log("File was not returned", "sample", "error");
        });
    }
})();
