//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario8_CopyAFile.html", {
        ready: function (element, options) {
            document.getElementById("copy").addEventListener("click", copy, false);
            SdkSample.validateFileExistence();
        }
    });

    function copy() {
        if (SdkSample.sampleFile !== null) {
            SdkSample.sampleFile.copyAsync(Windows.Storage.KnownFolders.picturesLibrary, "sample - Copy.dat", Windows.Storage.NameCollisionOption.replaceExisting).done(function (sampleFileCopy) {
                WinJS.log && WinJS.log("The file '" + SdkSample.sampleFile.name + "' was copied and the new file was named '" + sampleFileCopy.name + "'.", "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }
})();
