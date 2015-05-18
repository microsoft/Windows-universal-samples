//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    importScripts("//Microsoft.WinJS.2.0/js/base.js");
    importScripts("//microsoft.sdksamples.customusbdeviceaccess.js/js/constants.js");

    // This var is used to get information about the current instance of the background task.
    var backgroundTaskInstance = Windows.UI.WebUI.WebUIBackgroundTaskInstance.current;

    // Trigger details contain device id and arguments that are provided by the caller in the main app
    var deviceSyncDetails = backgroundTaskInstance.triggerDetails;

    var isBackgroundTaskCanceled = false;

    // Usb Device we are syncing with
    var device;

    // Promise for opening the device
    var openDevicePromise;

    // Promise for sending multiple bulk writes to the device
    var syncPromise;

    // Promise for the individual bulk writes to the device
    var individualWritePromise;

    function run() {
        backgroundTaskInstance.addEventListener("canceled", onCanceled);

        backgroundTaskInstance.progress = 0;

        isBackgroundTaskCanceled = false;

        // For simplicity, no error checking will be done after opening the device. Ideally, one should always
        // check if the device was successfully opened and respond accordingly. For an example on how to do this,
        // please see Scenario 1 of this sample.
        //
        // The user may also block the device via the settings charm while we are syncing (in background task). In order to deal with
        // the user changing permissions, we have to listen for DeviceAccessInformation->AccessChanged events. See EventHandlerForDevice 
        // for how to handle DeviceAccessInformation.accessChanged event.
        openDevicePromise = openDeviceAsync();

        openDevicePromise.then(function () {
            openDevicePromise = null;

            // The sample only demonstrates a bulk write for simplicity.
            // IO operations can be done after opening the device.
            // For more information on BackgroundTasks, please see the BackgroundTask sample on MSDN.
            syncPromise = writeToDeviceAsync();

            return syncPromise;
        }).then(function (bytesWritten) {
            Windows.Storage.ApplicationData.current.localSettings.values[SdkSample.Constants.localSettingKeys.syncBackgroundTaskResult] =
                bytesWritten;
            Windows.Storage.ApplicationData.current.localSettings.values[SdkSample.Constants.localSettingKeys.syncBackgroundTaskStatus] =
                SdkSample.Constants.syncBackgroundTaskInformation.taskCompleted;

        }, function (error) {
            if (error.name === "Canceled") {
                Windows.Storage.ApplicationData.current.localSettings.values[SdkSample.Constants.localSettingKeys.syncBackgroundTaskResult] = 0;
                Windows.Storage.ApplicationData.current.localSettings.values[SdkSample.Constants.localSettingKeys.syncBackgroundTaskStatus] =
                    SdkSample.Constants.syncBackgroundTaskInformation.taskCanceled;
            }
        }).done(function () {
            // Close the device because we are finished syncing and so that the app may reopen the device
            device.close();

            device = null;

            // Complete the background task (this raises the OnCompleted event on the corresponding BackgroundTaskRegistration)
            close();
        });
    }

    function openDeviceAsync() {
        return Windows.Devices.Usb.UsbDevice.fromIdAsync(deviceSyncDetails.deviceId).then(function (usbDevice) {
            device = usbDevice;

            // We opened the device, so notify the app that we've completed a bit of the background task
            backgroundTaskInstance.progress = 10;
        });
    }

    /// <summary>
    /// Cancels opening device and the IO operation, whichever is still running
    /// </summary>
    /// <param name="reason"></param>
    function onCanceled(reason) {
        if (openDevicePromise) {
            openDevicePromise.cancel();

            openDevicePromise = null;
        }

        if (syncPromise) {
            syncPromise.cancel();

            syncPromise = null;
        }

        if (individualWritePromise) {
            individualWritePromise.cancel();

            individualWritePromise = null;
        }

        isBackgroundTaskCanceled = true;
    }

    /// <summary>
    /// This method exists to demonstrate IO with the device since that is what is expected for a sync scenario. There already exists
    /// scenarios in this sample that demonstrate how to properly perform IO with the device, please see those for more information on 
    /// performing various kinds of IO.
    ///
    /// Writes to device's first bulkOut endpoint of the default interface and updates progress per write.
    /// When this method finishes, the progress will be 100.
    /// </summary>
    /// <returns>Total number of bytes written to the device</returns>
    function writeToDeviceAsync() {
        return new WinJS.Promise(function (success, error) {
            var totalBytesWritten = 0;

            if (!isBackgroundTaskCanceled) {
                var firstBulkOutEndpoint = device.defaultInterface.bulkOutPipes[0];

                // Evenly distributes the remaining progress (out of 100) among each write
                var progressIncreasePerWrite = (100 - backgroundTaskInstance.progress) / SdkSample.Constants.sync.numberOfTimesToWrite;

                // This progress will be incremented by the more accurate progressIncreasePerWrite value and then rounded up before notifying the app
                var syncProgress = backgroundTaskInstance.progress;

                var dataWriter = Windows.Storage.Streams.DataWriter(firstBulkOutEndpoint.outputStream);

                // Create an array, all default initialized to 0, and write it to the buffer
                // The data inside the buffer will be garbage
                var arrayBuffer = new Array(SdkSample.Constants.sync.bytesToWriteAtATime);

                for (var timesWritten = 0; timesWritten < SdkSample.Constants.sync.numberOfTimesToWrite; timesWritten++) {
                    // This check is used to prevent further chaining of promises in case the task was canceled while we are setting up the promise chain.
                    // The individualWritePromise tasks will be canceled in the onCanceled() method
                    if (!isBackgroundTaskCanceled) {

                        // Chain the current write promise to the end of the previous write promise
                        if (!individualWritePromise) {
                            dataWriter.writeBytes(arrayBuffer);

                            individualWritePromise = dataWriter.storeAsync().then(function (bytesWritten) {
                                totalBytesWritten += bytesWritten;

                                syncProgress += progressIncreasePerWrite;

                                // Rounding our value up because backgroundTaskInstance.progress is an unsigned int
                                backgroundTaskInstance.progress = Math.ceil(syncProgress);
                            });
                        } else {
                            individualWritePromise = individualWritePromise.then(function () {
                                dataWriter.writeBytes(arrayBuffer);

                                return dataWriter.storeAsync();
                            }).then(function (bytesWritten) {
                                totalBytesWritten += bytesWritten;

                                syncProgress += progressIncreasePerWrite;

                                // Rounding our value up because backgroundTaskInstance.progress is an unsigned int
                                backgroundTaskInstance.progress = Math.ceil(syncProgress);
                            });
                        }
                    }
                }

                // When all the writes are complete, pass the total number of bytes written down the promise chain
                individualWritePromise.done(function () {
                    success(totalBytesWritten);
                }, error);
            } else {
                success(totalBytesWritten);
            }
        });
    }

    run();
})();
