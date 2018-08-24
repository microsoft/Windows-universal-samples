//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var readCounter = 0;
    var writeCounter = 0;

    var page = WinJS.UI.Pages.define("/html/scenario5_deviceReadWrite.html", {
        ready: function (element, options) {
            document.getElementById("device-readwrite-read").addEventListener("click", onReadBlock, false);
            document.getElementById("device-readwrite-write").addEventListener("click", onWriteBlock, false);
        }
    });

    function logMessage(msg) {
        var output = document.getElementById("device-readwrite-output");
        output.innerHTML = "<p>" + msg + "</p>" + output.innerHTML;
        WinJS.log && WinJS.log(msg, "sample");
    }

    function onReadBlock() {

        var fx2Device = DeviceList.getSelectedFx2Device();

        if (!fx2Device) {
            WinJS.log && WinJS.log("Fx2 device not connected or accessible", "sample", "error");
            return;
        }

        var button = document.getElementById("device-readwrite-read");
        button.disabled = true;

        var inputStream = fx2Device.inputStream;
        var dataReader = new Windows.Storage.Streams.DataReader(inputStream);

        var counter = readCounter++;

        logMessage("Read " + counter + " begin");

        // Read up to 64 bytes into the data reader
        dataReader.loadAsync(64).
        then(
            function (operation) {
                var message = dataReader.readString(dataReader.unconsumedBufferLength);
                logMessage("Read " + counter + " end: " + message);
                button.disabled = false;
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
                button.disabled = false;
            }
        );
    }

    function onWriteBlock() {

        var fx2Device = DeviceList.getSelectedFx2Device();

        if (!fx2Device) {
            WinJS.log && WinJS.log("Fx2 device not connected or accessible", "sample", "error");
            return;
        }

        var button = document.getElementById("device-readwrite-write");
        button.disabled = true;

        var dataWriter = new Windows.Storage.Streams.DataWriter(fx2Device.outputStream);

        var counter = writeCounter++;

        var message = "This is message " + counter;

        dataWriter.writeString(message);

        logMessage("Write " + counter + " begin: " + message);

        dataWriter.storeAsync().
        then(
            function (bytesWritten) {
                logMessage("Write " + counter + " end: " + bytesWritten + " bytes written");
                button.disabled = false;
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
                button.disabled = false;
            }
        );
    }

})();
