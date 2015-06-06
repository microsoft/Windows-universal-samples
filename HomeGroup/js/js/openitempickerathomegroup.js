//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/OpenItemPickerAtHomegroup.html", {
        ready: function (element, options) {
            document.getElementById("OpenItemPicker").addEventListener("click", openPickerAtHomeGroup, false);
        }
    });

    function openPickerAtHomeGroup() {
        // create a picker and set it up to the homegroup, enabling any content to be returned
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.viewMode = Windows.Storage.Pickers.PickerViewMode.thumbnail;
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.homeGroup;
        picker.fileTypeFilter.replaceAll(["*"]);

        // now open the picker and get a file from the user
        picker.pickSingleFileAsync().done(function (file) {
            if (file) {
                // Our application now has read/write access to the picked file
                WinJS.log && WinJS.log("1 file selected: \"" + file.path + "\"", "sample", "status");
            }
        },
        function (file) {
            // something happened.
            WinJS.log && WinJS.log("File was not returned", "sample", "error");
        });
    }
})();
