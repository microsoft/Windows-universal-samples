//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario3_WriteAndReadTextInAFile.html", {
        ready: function (element, options) {
            document.getElementById("writeText").addEventListener("click", writeText, false);
            document.getElementById("readText").addEventListener("click", readText, false);
            SdkSample.validateFileExistence();
        }
    });

    function writeText() {
        if (SdkSample.sampleFile) {
            var textArea = document.getElementById("textarea");
            var userContent = textArea.value;
            Windows.Storage.FileIO.writeTextAsync(SdkSample.sampleFile, userContent).done(function () {
                WinJS.log && WinJS.log("The following text was written to '" + SdkSample.sampleFile.name + "':\n" + userContent, "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }

    function readText() {
        if (SdkSample.sampleFile) {
            Windows.Storage.FileIO.readTextAsync(SdkSample.sampleFile).done(function (fileContent) {
                WinJS.log && WinJS.log("The following text was read from '" + SdkSample.sampleFile.name + "':\n" + fileContent, "sample", "status");
            },
            function (error) {
                if (error.number === SdkSample.E_NO_UNICODE_TRANSLATION) {
                    WinJS.log && WinJS.log("File cannot be decoded as Unicode.", "sample", "error");
                } else {
                    WinJS.log && WinJS.log(error, "sample", "error");
                }
            });
        }
    }
})();
