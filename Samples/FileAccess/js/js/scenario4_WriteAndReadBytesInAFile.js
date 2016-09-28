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
        if (SdkSample.sampleFile !== null) {
            var textArea = document.getElementById("textarea");
            var userContent = textArea.value;
            if (userContent !== "") {
                var buffer = getBufferFromString(userContent);
                Windows.Storage.FileIO.writeBufferAsync(SdkSample.sampleFile, buffer).done(function () {
                    WinJS.log && WinJS.log("The following " + buffer.length + " bytes of text were written to '" + SdkSample.sampleFile.name + "':\n" + userContent, "sample", "status");
                },
                function (error) {
                    WinJS.log && WinJS.log(error, "sample", "error");
                });
            } else {
                WinJS.log && WinJS.log("The text box is empty, please write something and then click 'Write' again.", "sample", "error");
            }
        }
    }

    function getBufferFromString(str) {
        var memoryStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
        var dataWriter = new Windows.Storage.Streams.DataWriter(memoryStream);
        dataWriter.writeString(str);
        var buffer = dataWriter.detachBuffer();
        dataWriter.close();
        return buffer;
    }

    function readBytes() {
        if (SdkSample.sampleFile !== null) {
            Windows.Storage.FileIO.readBufferAsync(SdkSample.sampleFile).done(function (buffer) {
                var dataReader = Windows.Storage.Streams.DataReader.fromBuffer(buffer);
                var fileContent = dataReader.readString(buffer.length);
                dataReader.close();
                WinJS.log && WinJS.log("The following " + buffer.length + " bytes of text were read from '" + SdkSample.sampleFile.name + "':\n" + fileContent, "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }
})();
