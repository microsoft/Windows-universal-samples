//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario4_WriteAndReadBytesInAFile.html", {
        ready: function (element, options) {
            document.getElementById("writeBytes").addEventListener("click", writeBytes, false);
            document.getElementById("readBytes").addEventListener("click", readBytes, false);
            SdkSample.validateFileExistence();
        }
    });

    function writeBytes() {
        if (SdkSample.sampleFile) {
            var textArea = document.getElementById("textarea");
            var userContent = textArea.value;
            var buffer = SdkSample.getBufferFromString(userContent);
            Windows.Storage.FileIO.writeBufferAsync(SdkSample.sampleFile, buffer).done(function () {
                WinJS.log && WinJS.log("The following " + buffer.length + " bytes of text were written to '" + SdkSample.sampleFile.name + "':\n" + userContent, "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }

    function readBytes() {
        if (SdkSample.sampleFile) {
            Windows.Storage.FileIO.readBufferAsync(SdkSample.sampleFile).then(function (buffer) {
                var fileContent = SdkSample.getStringFromBuffer(buffer);
                WinJS.log && WinJS.log("The following " + buffer.length + " bytes of text were read from '" + SdkSample.sampleFile.name + "':\n" + fileContent, "sample", "status");
            }).done(null,
            function (error) {
                if (error.number === SdkSample.E_NO_UNICODE_TRANSLATION) {
                    WinJS.log && WinJS.log("File is not UTF-8 encoded.", "sample", "error");
                } else {
                    WinJS.log && WinJS.log(error, "sample", "error");
                }
            });
        }
    }
})();
