//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var BulkPipesClass = WinJS.Class.define(null, {
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
            if (bulkPipes.isPerformingIo) {
                WinJS.log && WinJS.log("Canceling...", "sample", "status");

                if (bulkPipes.readingPromise) {
                    bulkPipes.readingPromise.cancel();
                }

                if (bulkPipes.writingPromise) {
                    bulkPipes.writingPromise.cancel();
                }

                if (bulkPipes.readingWritingPromise) {
                    bulkPipes.readingWritingPromise.cancel();
                }
            }
        },
        printTotalReadWriteBytes: function () {
            // Prevent output from being printed into other scenarios
            if (!bulkPipes.navigatedAway) {
                WinJS.log && WinJS.log("Total bytes read: " + bulkPipes.totalBytesRead + "; Total bytes written: " + bulkPipes.totalBytesWritten, "sample", "status");
            }
        },
        /// <summary>
        /// Determines if we are reading, writing, or reading and writing.
        /// </summary>
        /// <returns>If we are doing any of the above operations, we return true; false otherwise</returns>
        isPerformingIo: function () {
            return (bulkPipes.isReading || bulkPipes.isWriting || bulkPipes.isReadingWriting);
        },
        /// <summary>
        /// Will read data from the specified input pipe. The data being read is garbage data because the samples devices are giving us garbage data.
        /// We save the read promise so that we can cancel it in the future.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        /// </summary>
        /// <param name="bulkPipeIndex">Index of pipe in the list of Device->DefaultInterface->BulkInPipes</param>
        /// <param name="bytesToRead">Bytes of garbage data to read</param>
        bulkReadAsync: function (bulkPipeIndex, bytesToRead) {
            var stream = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface.bulkInPipes.getAt(bulkPipeIndex).inputStream;

            var reader = new Windows.Storage.Streams.DataReader(stream);

            bulkPipes.readingPromise = reader.loadAsync(bytesToRead).then(function (bytesRead) {
                bulkPipes.readingPromise = null;

                bulkPipes.totalBytesRead += bytesRead;

                bulkPipes.printTotalReadWriteBytes();

                // Close the reader explicitly to free resources and not have to wait till the GC deletes the reader because reads may be looped
                reader.close();
            });

            return bulkPipes.readingPromise;
        },
        /// <summary>
        /// Will write garbage data to the specified output pipe. Since writing to the device may take a while to complete,
        /// we save the write promise so that we can cancel it in the future.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        /// </summary>
        /// <param name="bulkPipeIndex">Index of pipe in the list of Device->DefaultInterface->BulkOutPipes</param>
        /// <param name="bytesToWrite">Bytes of garbage data to write</param>
        bulkWriteAsync: function (bulkPipeIndex, bytesToWrite) {
            // Create an array, all default initialized to 0, and write it to the buffer
            // The data inside the buffer will be garbage
            var arrayBuffer = new Array(bytesToWrite);

            var stream = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface.bulkOutPipes.getAt(bulkPipeIndex).outputStream;

            var writer = new Windows.Storage.Streams.DataWriter(stream);
            writer.writeBytes(arrayBuffer);

            // This is where the data is flushed out to the device.
            bulkPipes.writingPromise = writer.storeAsync().then(function (bytesWritten) {
                bulkPipes.writingPromise = null;

                bulkPipes.totalBytesWritten += bytesWritten;

                bulkPipes.printTotalReadWriteBytes();

                // Close the writer explicitly to free resources and not have to wait till the GC deletes the reader because writes may be looped
                writer.close();
            });

            return bulkPipes.writingPromise;
        },
        /// <summary>
        /// A read and a write will be initiated simultaneously. Reads and writes are looped; after each read/write succeeds, another read/write is initiated.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        /// </summary>
        /// <param name="bulkInPipeIndex">Index of pipe in the list of Device->DefaultInterface->BulkInPipes</param>
        /// <param name="bulkOutPipeIndex">Index of pipe in the list of Device->DefaultInterface->BulkOutPipes</param>
        /// <param name="bytesToReadWrite">Bytes of garbage data to read/write</param>
        /// <param name="state">State of the BulkPipes class</param>
        bulkReadWriteAsync: function (bulkInPipeIndex, bulkOutPipeIndex, bytesToReadWrite, state) {
            var readingWritingPromiseArray = [];

            readingWritingPromiseArray.push(new WinJS.Promise(function (success, error) {
                function readLoop() {
                    if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                        if (state.isReadingWriting) {
                            state.bulkReadAsync(bulkInPipeIndex, bytesToReadWrite).done(function () {
                                readLoop();
                            }, error);
                        }
                    } else {
                        // Prevent output from being printed into other scenarios
                        if (!bulkPipes.navigatedAway) {
                            WinJS && WinJS.log("Device was disconnected before we could read/write to it", "sample", "error");
                        }
                    }
                }

                readLoop();
            }));

            readingWritingPromiseArray.push = new WinJS.Promise(function (success, error) {
                function writeLoop() {
                    if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                        if (state.isReadingWriting) {
                            state.bulkWriteAsync(bulkOutPipeIndex, bytesToReadWrite).done(function () {
                                writeLoop();
                            }, error);
                        }
                    } else {
                        // Prevent output from being printed into other scenarios
                        if (!bulkPipes.navigatedAway) {
                            WinJS && WinJS.log("Device was disconnected before we could read/write to it", "sample", "error");
                        }
                    }
                }

                writeLoop();
            });

            bulkPipes.readingWritingPromise = WinJS.Promise.join(readingWritingPromiseArray).then(function () {
                state.readingWritingPromise = null;
            });

            return bulkPipes.readingWritingPromise;
        },
        /// <summary>
        /// Stop any pending IO operations because the device will be closed when the app suspends
        /// </summary>
        /// <param name="eventArgs"></param>
        onAppSuspension: function (eventArgs) {
            bulkPipes.cancelAllIoTasks();
        },
        /// <summary>
        /// Reset the buttons when the device is reopened
        /// </summary>
        /// <param name="onDeviceConnectedEventArgs"></param>
        onDeviceConnected: function (onDeviceConnectedEventArgs) {
            updateButtonStates();
        }
    },
    null);

    var bulkPipes = new BulkPipesClass();

    var page = WinJS.UI.Pages.define("/html/scenario4_bulkPipes.html", {
        ready: function (element, options) {
            bulkPipes.navigatedAway = false;
            bulkPipes.totalBytesWritten = 0;
            bulkPipes.totalBytesRead = 0;

            // Set up Button listeners before hiding scenarios in case the button is removed when hiding scenario
            document.getElementById("buttonBulkRead").addEventListener("click", bulkReadClick, false);
            document.getElementById("buttonBulkWrite").addEventListener("click", bulkWriteClick, false);
            document.getElementById("buttonBulkReadWrite").addEventListener("click", bulkReadWriteClick, false);
            document.getElementById("buttonCancelAllIoTasks").addEventListener("click", cancelAllIoTasksClick, false);

            // Available scenarios
            var deviceScenarios = {};
            deviceScenarios[SdkSample.Constants.deviceType.osrFx2] = document.querySelector(".generalScenario");
            deviceScenarios[SdkSample.Constants.deviceType.superMutt] = document.querySelector(".generalScenario");

            SdkSample.CustomUsbDeviceAccess.utilities.setUpDeviceScenarios(deviceScenarios, document.querySelector(".deviceScenarioContainer"));

            // Reset the buttons if the app resumed and the device is reconnected
            SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.onAppSuspendCallback = bulkPipes.onAppSuspension;
            SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.onDeviceConnectedCallback = bulkPipes.onDeviceConnected;

            updateButtonStates();
        },
        unload: function () {
            bulkPipes.navigatedAway = true;

            bulkPipes.cancelAllIoTasks();

            // We don't need to worry about app suspend for this scenario anymore
            SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.onAppSuspendCallback = null;
            SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.onDeviceConnectedCallback = null;

            // The page is not "reloaded" when navigated away and back, so we need to make reset all button states
            updateButtonStates();
        }
    });

    function bulkReadClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            WinJS.log && WinJS.log("Reading...", "sample", "status");

            bulkPipes.isReading = true;

            // Both supported devices have the bulk in pipes on index 0
            var bulkInPipeIndex = 0;

            // Read as much data as possible in one packet
            var bytesToRead = 512;

            // Re-enable read button after completing the read
            bulkPipes.bulkReadAsync(bulkInPipeIndex, bytesToRead).done(function () {
                bulkPipes.isReading = false;

                updateButtonStates();
            }, function (error) {
                // Promise was canceled
                if (error.name === "Canceled") {
                    bulkPipes.isReading = false;

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

    function bulkWriteClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            WinJS.log && WinJS.log("Writing...", "sample", "status");

            bulkPipes.isWriting = true;

            // Both supported devices have the bulk out pipes on index 0
            var bulkOutPipeIndex = 0;

            // Write as much data as possible in one packet
            var bytesToWrite = 512;

            // Re-enable write button after completing the write
            bulkPipes.bulkWriteAsync(bulkOutPipeIndex, bytesToWrite).done(function () {
                bulkPipes.isWriting = false;

                updateButtonStates();
            }, function (error) {
                // Promise was canceled
                if (error.name === "Canceled") {
                    bulkPipes.isWriting = false;

                    updateButtonStates();
                } else {
                    // Since this is a .done() error callback, there are no other error
                    // handlers after this. So create an error that is unhandled
                    WinJS.Promise.wrapError(error);
                }
            });

            // Disable buttons to prevent multiple writes if the WriteAsync has not completed yet
            updateButtonStates();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function bulkReadWriteClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            WinJS.log && WinJS.log("Reading/Writing...", "sample", "status");

            bulkPipes.isReadingWriting = true;

            // Both supported devices have the bulk in/out pipes on index 0
            var bulkInPipeIndex = 0;
            var bulkOutPipeIndex = 0;

            // Write as much data as possible in one packet
            var bytesToWrite = 512;

            // Re-enable read/write button after completing the read/write
            bulkPipes.bulkReadWriteAsync(bulkInPipeIndex, bulkOutPipeIndex, bytesToWrite, bulkPipes).done(function () {
                bulkPipes.isReadingWriting = false;

                updateButtonStates();
            }, function (error) {
                // Promise was canceled
                if (error.name === "Canceled") {
                    bulkPipes.isReadingWriting = false;

                    updateButtonStates();
                } else {
                    // Since this is a .done() error callback, there are no other error
                    // handlers after this. So create an error that is unhandled
                    WinJS.Promise.wrapError(error);
                }
            });

            // Disable buttons to prevent the other read/write buttons from working
            updateButtonStates();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function cancelAllIoTasksClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            disableAllButtons();

            bulkPipes.cancelAllIoTasks();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    /// <summary>
    /// Disable all buttons to prevent multiple cancel requests.
    /// </summary>
    function disableAllButtons() {
        window.buttonBulkReadWrite.disabled = true;
        window.buttonBulkRead.disabled = true;
        window.buttonBulkWrite.disabled = true;
        window.buttonCancelAllIoTasks.disabled = true;
    }

    function updateButtonStates() {
        if (window.buttonBulkReadWrite) {
            window.buttonBulkReadWrite.disabled = bulkPipes.isPerformingIo();
        }

        if (window.buttonBulkRead) {
            window.buttonBulkRead.disabled = bulkPipes.isReadingWriting || bulkPipes.isReading;
        }

        if (window.buttonBulkWrite) {
            window.buttonBulkWrite.disabled = bulkPipes.isReadingWriting || bulkPipes.isWriting;
        }

        if (window.buttonCancelAllIoTasks) {
            window.buttonCancelAllIoTasks.disabled = !(bulkPipes.isPerformingIo());
        }
    }
})();
