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
        if (SdkSample.sampleFile !== null) {
            var textArea = document.getElementById("textarea");
            var userContent = textArea.value;
            if (userContent !== "") {
                SdkSample.sampleFile.openTransactedWriteAsync().then(function (transaction) {
                    var dataWriter = new Windows.Storage.Streams.DataWriter(transaction.stream);
                    dataWriter.writeString(userContent);
                    dataWriter.storeAsync().then(function (size) {
                        transaction.stream.size = size; // reset stream size to override the file
                        transaction.commitAsync().done(function () {
                            WinJS.log && WinJS.log("The following text was written to '" + SdkSample.sampleFile.name + "' using a stream:\n" + userContent, "sample", "status");
                            transaction.close();
                        });
                    });
                },
                function (error) {
                    WinJS.log && WinJS.log(error, "sample", "error");
                });
            } else {
                WinJS.log && WinJS.log("The text box is empty, please write something and then click 'Write' again.", "sample", "error");
            }
        }
    };

    function readFromStream() {
        if (SdkSample.sampleFile !== null) {
            SdkSample.sampleFile.openAsync(Windows.Storage.FileAccessMode.read).then(function (readStream) {
                var size = readStream.size;
                var maxuint32 = 4294967295; // loadAsync only takes UINT32 value.
                if (size <= maxuint32) {
                    var dataReader = new Windows.Storage.Streams.DataReader(readStream);
                    dataReader.loadAsync(size).done(function (numBytesLoaded) {
                        var fileContent = dataReader.readString(numBytesLoaded);
                        WinJS.log && WinJS.log("The following text was read from '" + SdkSample.sampleFile.name + "' using a stream:\n" + fileContent, "sample", "status");
                        dataReader.close();
                    });
                } else {
                    var error = "File " + SdkSample.sampleFile.name + " is too big for LoadAsync to load in a single chunk. Files larger than 4GB need to be broken into multiple chunks to be loaded by LoadAsync.";
                    WinJS.log && WinJS.log(error, "sample", "error");
                }
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    };
})();
