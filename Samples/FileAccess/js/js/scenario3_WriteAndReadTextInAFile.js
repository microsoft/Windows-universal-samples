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
        if (SdkSample.sampleFile !== null) {
            var textArea = document.getElementById("textarea");
            var userContent = textArea.value;
            if (userContent !== "") {
                Windows.Storage.FileIO.writeTextAsync(SdkSample.sampleFile, userContent).done(function () {
                    WinJS.log && WinJS.log("The following text was written to '" + SdkSample.sampleFile.name + "':\n" + userContent, "sample", "status");
                },
                function (error) {
                    WinJS.log && WinJS.log(error, "sample", "error");
                });
            } else {
                WinJS.log && WinJS.log("The text box is empty, please write something and then click 'Write' again.", "sample", "error");
            }
        }
    }

    function readText() {
        if (SdkSample.sampleFile !== null) {
            Windows.Storage.FileIO.readTextAsync(SdkSample.sampleFile).done(function (fileContent) {
                WinJS.log && WinJS.log("The following text was read from '" + SdkSample.sampleFile.name + "':\n" + fileContent, "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }
})();
