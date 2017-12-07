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
                var video = document.getElementById("mainVideo");
                video.src = URL.createObjectURL(file, { oneTimeOnly: true });
            });
    }
})();
