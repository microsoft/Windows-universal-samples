//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario10_DeleteAFile.html", {
        ready: function (element, options) {
            document.getElementById("delete").addEventListener("click", deleteFile, false);
            SdkSample.validateFileExistence();
        }
    });

    function deleteFile() {
        if (SdkSample.sampleFile !== null) {
            SdkSample.sampleFile.deleteAsync().done(function () {
                SdkSample.sampleFile = null;
                WinJS.log && WinJS.log("The file 'sample.dat' was deleted.", "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }
})();
