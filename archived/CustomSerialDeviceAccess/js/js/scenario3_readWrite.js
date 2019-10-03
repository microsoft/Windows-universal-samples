//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var serialIoClass = WinJS.Class.define(null, {

        writeBytesAvailable: false,
        readingPromise: null,
        writingPromise: null,
        readingWritingPromise: null,
        isReading: false,
        isWriting: false,
        isReadingWriting: false,
        totalBytesWritten: 0,
        totalBytesRead: 0,
        // Indicate if we navigate away from this page or not.
        navigatedAway: false,
        /// <summary>
        /// It is important to be able to cancel tasks that may take a while to complete. Canceling tasks is the only way to stop any pending IO
        /// operations asynchronously. If the UsbDevice is closed/deleted while there are pending IOs, the destructor will cancel all pending IO 
        /// operations.
        /// </summary>
        cancelAllIoTasks: function () {
            if (serialIo.isPerformingIo) {
                WinJS.log && WinJS.log("Canceling...", "sample", "status");
                serialIo.cancelRead();
                serialIo.cancelWrite();
            }
        },

        cancelRead: function () {
            if (serialIo.readingPromise) {
                serialIo.readingPromise.cancel();
            }
        },

        cancelWrite: function () {
            if (serialIo.writingPromise) {
                serialIo.writingPromise.cancel();
            }
        },

        printTotalReadWriteBytes: function () {
            // Prevent output from being printed into other scenarios
            if (!serialIo.navigatedAway) {
                WinJS.log && WinJS.log("Total bytes read: " + serialIo.totalBytesRead + "; Total bytes written: " + serialIo.totalBytesWritten, "sample", "status");
            }
        },
        /// <summary>
        /// Determines if we are reading, writing, or reading and writing.
        /// </summary>
        /// <returns>If we are doing any of the above operations, we return true; false otherwise</returns>
        isPerformingIo: function () {
            return (serialIo.isReading || serialIo.isWriting || serialIo.isReadingWriting);
        },

        /// <summary>
        /// Read async function
        /// </summary>
        readAsync: function () {
            var bytesToRead = 1024;
            var stream = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.inputStream;
            var reader = new Windows.Storage.Streams.DataReader(stream);

            serialIo.readingPromise = reader.loadAsync(bytesToRead).then(function (bytesRead) {
                serialIo.readingPromise = null;
                serialIo.totalBytesRead += bytesRead;
                updateReadBytesCountView();
                serialIo.printTotalReadWriteBytes();
                if (bytesRead > 0) {
                    document.getElementById("ReadBytesTextArea").value += reader.readString(bytesRead) + '\n';
                }
                // Close the reader explicitly to free resources and not have to wait till the GC deletes the reader because reads may be looped
                reader.detachStream();
                reader.close();
            });

            return serialIo.readingPromise;
        },

        /// <summary>
        /// </summary>
        writeAsync: function () {

                var bytesToWrite = document.getElementById("WriteBytesInput").value.length;
                var arrayBuffer = document.getElementById("WriteBytesInput").value;
                document.getElementById("WriteBytesInput").value = "";

                var stream = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.outputStream;

                var writer = new Windows.Storage.Streams.DataWriter(stream);
                writer.writeString(arrayBuffer);

                // This is where the data is flushed out to the device.
                serialIo.writingPromise = writer.storeAsync().then(function (bytesWritten) {
                    serialIo.writingPromise = null;

                    serialIo.totalBytesWritten += bytesWritten;

                    serialIo.printTotalReadWriteBytes();
                    document.getElementById("WriteBytesTextArea").textContent += arrayBuffer + '\n';
                    document.getElementById("WriteBytesTextArea").scrollTop = document.getElementById("WriteBytesTextArea").scrollHeight;
                    updateWriteBytesCountView();


                    // Close the writer explicitly to free resources and not have to wait till the GC deletes the reader because writes may be looped
                    writer.detachStream();
                    writer.close();
                });
            return serialIo.writingPromise;
        },

        /// <summary>
        /// Stop any pending IO operations because the device will be closed when the app suspends
        /// </summary>
        /// <param name="eventArgs"></param>
        onAppSuspension: function (eventArgs) {
            serialIo.cancelAllIoTasks();
        },
        /// <summary>
        /// Reset the buttons when the device is reopened
        /// </summary>
        /// <param name="onDeviceConnectedEventArgs"></param>
        onDeviceConnected: function (onDeviceConnectedEventArgs) {
            updateButtonStates();
        }

    }); // end of serialIoClass

    var serialIo = new serialIoClass();

    function updateWriteTimeoutView() {
        document.getElementById("WriteTimeoutLabel").textContent = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.writeTimeout;
    }

    function updateReadTimeoutView() {
        document.getElementById("ReadTimeoutLabel").textContent = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.readTimeout;
    }

    function updateWriteBytesCountView() {
        document.getElementById("WriteBytesCountLabel").textContent = serialIo.totalBytesWritten + " Bytes";
    }

    function updateReadBytesCountView() {
        document.getElementById("ReadBytesCountLabel").textContent = serialIo.totalBytesRead + " Bytes";
    }

    function writeTimeoutButtonClicked() {
        var input = document.getElementById("WriteTimeoutInput");
        var writeTimeout = parseInt(input.value, 10);
        if (isNaN(writeTimeout) == false) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.writeTimeout = writeTimeout;
            updateWriteTimeoutView();
        } else {
            input.value = "";
        }
    }

    function readTimeoutButtonClicked() {
        var input = document.getElementById("ReadTimeoutInput");
        var readTimeout = parseInt(input.value, 10);
        if (isNaN(readTimeout) == false) {
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device.readTimeout = readTimeout;
            updateReadTimeoutView();
        } else {
            input.value = "";
        }
    }

    function readButtonClicked() {
        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            WinJS.log && WinJS.log("Reading...", "sample", "status");

            serialIo.isReading = true;

            // Read as much data as possible in one packet
            var bytesToRead = 1024;

            // Re-enable read button after completing the read
            serialIo.readAsync().done(function () {
                serialIo.isReading = false;

                updateButtonStates();
            }, function (error) {
                // Promise was canceled
                if (error.name === "Canceled") {
                    WinJS.log && WinJS.log("Read Canceled...", "sample", "status");
                    serialIo.isReading = false;

                    updateButtonStates();
                } else {
                    // Since this is a .done() error callback, there are no other error
                    // handlers after this. So create an error that is unhandled
                    WinJS.Promise.wrapError(error);
                }
            });

            // Disable buttons to prevent multiple reads if the ReadAsync has not completed yet
            updateButtonStates();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function writeButtonClicked() {
        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            if (serialIo.writeBytesAvailable && (document.getElementById("WriteBytesInput").value.length != 0)) {

                WinJS.log && WinJS.log("Writing...", "sample", "status");

                serialIo.isWriting = true;
                serialIo.writingPromise = null;

                // Re-enable write button after completing the write
                serialIo.writeAsync().done(function () {
                    serialIo.isWriting = false;

                    updateButtonStates();
                }, function (error) {
                    // Promise was canceled
                    if (error.name === "Canceled") {
                        WinJS.log && WinJS.log("Write Canceled...", "sample", "status");
                        serialIo.isWriting = false;
                        updateButtonStates();
                    } else {
                        // Since this is a .done() error callback, there are no other error
                        // handlers after this. So create an error that is unhandled
                        WinJS.Promise.wrapError(error);
                    }
                });

                // Disable buttons to prevent multiple writes if the WriteAsync has not completed yet
                updateButtonStates();
            }
        } else {
            SdkSample.CustomSerialDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function writeCancelButtonClicked() {
        serialIo.cancelWrite();
    }

    function readCancelButtonClicked() {
        serialIo.cancelRead();
    }

    function writeBytesInputChanged() {
        if (serialIo.writeBytesAvailable == false) {
            serialIo.writeBytesAvailable = true;
            document.getElementById("WriteBytesInput").value = "";
        }
    }

    var page = WinJS.UI.Pages.define("/html/Scenario3_readWrite.html", {
        ready: function (element, options) {

            if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.isDeviceConnected == false) {
                document.getElementById("scenarioContent").hidden = true;
                WinJS.log && WinJS.log("Device is NOT connected", "sample", "status");
                return;
            }

            document.getElementById("scenarioContent").hidden = false;
            document.getElementById("WriteTimeoutButton").addEventListener("click", writeTimeoutButtonClicked, false);
            document.getElementById("WriteButton").addEventListener("click", writeButtonClicked, false);
            document.getElementById("WriteCancelButton").addEventListener("click", writeCancelButtonClicked, false);
            document.getElementById("WriteBytesInput").addEventListener("focus", writeBytesInputChanged, false);

            document.getElementById("ReadTimeoutButton").addEventListener("click", readTimeoutButtonClicked, false);
            document.getElementById("ReadButton").addEventListener("click", readButtonClicked, false);
            document.getElementById("ReadCancelButton").addEventListener("click", readCancelButtonClicked, false);

            updateWriteTimeoutView();
            updateReadTimeoutView();

            updateWriteBytesCountView();
            updateReadBytesCountView();

            updateButtonStates();

            // Reset the buttons if the app resumed and the device is reconnected
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.onAppSuspendCallback = serialIo.onAppSuspension;
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.onDeviceConnectedCallback = serialIo.onDeviceConnected;

        },

        unload: function () {
            serialIo.navigatedAway = true;

            serialIo.cancelAllIoTasks();

            // We don't need to worry about app suspend for this scenario anymore
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.onAppSuspendCallback = null;
            SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.onDeviceConnectedCallback = null;

            // The page is not "reloaded" when navigated away and back, so we need to make reset all button states
            updateButtonStates();
        }


    });

    function updateButtonStates() {
        if (serialIo.isWriting) {
            window.WriteButton.disabled = true;
            window.WriteCancelButton.disabled = false;
        } else {
            window.WriteButton.disabled = false;
            window.WriteCancelButton.disabled = true;
        }

        if (serialIo.isReading) {
            window.ReadButton.disabled = true;
            window.ReadCancelButton.disabled = false;
        } else {
            window.ReadButton.disabled = false;
            window.ReadCancelButton.disabled = true;
        }
    }

    function doSomething() {
        WinJS.log && WinJS.log("Error message here", "sample", "error");
    }
})();
