//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2_MultiFile.html", {
        ready: function (element, options) {
            element.querySelector(".open").addEventListener("click", pickMultipleFiles, false);
        }
    });

    function pickMultipleFiles() {
        // Clean scenario output
        WinJS.log && WinJS.log("", "sample", "status");

        // Create the picker object and set options
        var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
        openPicker.viewMode = Windows.Storage.Pickers.PickerViewMode.list;
        openPicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.documentsLibrary;
        openPicker.fileTypeFilter.replaceAll(["*"]);

        // Open the picker for the user to pick a file
        openPicker.pickMultipleFilesAsync().then(function (files) {
            if (files.size > 0) {
                // Application now has read/write access to the picked file(s)
                var outputString = "Picked files:\n";
                for (var i = 0; i < files.size; i++) {
                    outputString = outputString + files[i].name + "\n";
                }
                WinJS.log && WinJS.log(outputString, "sample", "status");
            } else {
                // The picker was dismissed with no selected file
                WinJS.log && WinJS.log("Operation cancelled.", "sample", "status");
            }
        });
    }
})();
