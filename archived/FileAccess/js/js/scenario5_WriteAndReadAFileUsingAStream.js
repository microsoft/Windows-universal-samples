//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario5_WriteAndReadAFileUsingAStream.html", {
        ready: function (element, options) {
            document.getElementById("writeToStream").addEventListener("click", writeToStream, false);
            document.getElementById("readFromStream").addEventListener("click", readFromStream, false);
            SdkSample.validateFileExistence();
        }
    });

    function writeToStream() {
        if (SdkSample.sampleFile) {
            var textArea = document.getElementById("textarea");
            var userContent = textArea.value;
            var transaction;
            SdkSample.sampleFile.openTransactedWriteAsync().then(function (newTransaction) {
                transaction = newTransaction;
                var buffer = SdkSample.getBufferFromString(userContent);
                return transaction.stream.writeAsync(buffer);
            }).then(function (bytesWritten) {
                transaction.stream.size = bytesWritten; // truncate file
                return transaction.commitAsync();
            }).then(function () {
                WinJS.log && WinJS.log("The following text was written to '" + SdkSample.sampleFile.name + "' using a stream:\n" + userContent, "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            }).done(function () {
                // Ensure we close the transaction even if an error occurs.
                transaction && transaction.close();
            });
        }
    }

    function readFromStream() {
        if (SdkSample.sampleFile) {
            SdkSample.sampleFile.openAsync(Windows.Storage.FileAccessMode.read).then(function (readStream) {
                var size = readStream.size;
                var maxuint32 = 4294967295; // Buffer size is UINT32
                if (size <= maxuint32) {
                    var buffer = new Windows.Storage.Streams.Buffer(size);
                    return readStream.readAsync(buffer, size, Windows.Storage.Streams.InputStreamOptions.none).then(function (buffer) {
                        var fileContent = SdkSample.getStringFromBuffer(buffer);
                        WinJS.log && WinJS.log("The following text was read from '" + SdkSample.sampleFile.name + "' using a stream:\n" + fileContent, "sample", "status");
                    }).then(function () {
                        readStream.close();
                    });
                } else {
                    readStream.close();
                    var error = "File " + SdkSample.sampleFile.name + " is too big for ReadAsync to read in a single chunk. Files larger than 4GB need to be broken into multiple chunks to be loaded by ReadAsync.";
                    WinJS.log && WinJS.log(error, "sample", "error");
                }
            }).then(null,
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
