//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario9_CompareTwoFilesToSeeIfTheyAreTheSame.html", {
        ready: function (element, options) {
            document.getElementById("compareFiles").addEventListener("click", compareFiles, false);
            SdkSample.validateFileExistence();
        }
    });

    function compareFiles() {
        if (SdkSample.sampleFile !== null) {
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
            picker.fileTypeFilter.replaceAll(["*"]);
            picker.pickSingleFileAsync().done(function (comparand) {
                if (comparand !== null) {
                    if (SdkSample.sampleFile.isEqual(comparand)) {
                        WinJS.log && WinJS.log("Files are equal", "sample", "status");
                    } else {
                        WinJS.log && WinJS.log("Files are not equal", "sample", "status");
                    }
                } else {
                    WinJS.log && WinJS.log("Operation cancelled", "sample", "status");
                }
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }
})();
