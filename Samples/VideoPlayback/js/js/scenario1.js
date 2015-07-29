//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            element.querySelector(".open").addEventListener("click", pickVideo, false);
        }
    });

    function pickVideo() {
        var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
        openPicker.viewMode = Windows.Storage.Pickers.PickerViewMode.thumbnail;
        openPicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        openPicker.fileTypeFilter.replaceAll([".mp4", ".mkv", ".avi"]);

        openPicker.pickSingleFileAsync().then(
            function (file) {
                //HOW DO I SET THE VIDEO TAG SOURCE?
                var video = document.getElementById(mainVideo);
                var URI = encodeURI(file)
                video.setAttribute("src", URI);
            });
    }
})();
