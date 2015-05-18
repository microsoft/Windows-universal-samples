//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    /// <summary>
    /// Scenario will demonstrate how to change interface settings (also known as alternate interface setting).
    /// 
    /// This scenario can work for any device as long as it is "added" into the SdkSample.CustomUsbDeviceAccess. For more information on how to add a 
    /// device to make it compatible with this scenario, please see Scenario1_DeviceConnect.
    /// </summary>
    var SyncDeviceClass = WinJS.Class.define(null, {
        _syncDeviceInformation: null,
        _syncDeviceSelector: null,
        // Indicate if we navigate away from this page or not.
        navigatedAway: false,
        backgroundTaskRegistration: null,
        /// <summary>
        /// Save trigger so that we may start the background task later
        /// Only one instance of the trigger can exist at a time. Since the trigger does not implement
        /// IDisposable, it may still be in memory when a new trigger is created.
        /// </summary>
        syncBackgroundTaskTrigger: new Windows.ApplicationModel.Background.DeviceUseTrigger(),
        isSyncing: false,
        cancelSyncWithDevice: function () {
            if (this.isSyncing) {
                this.backgroundTaskRegistration.unregister(true);

                this.backgroundTaskRegistration = null;

                // We are canceling the task, so we are no longer syncing. If the task is registered but never run,
                // the cancel completion is never called
                this.isSyncing = false;
            }
        },
        /// <summary>
        /// Starts the background task that will sync with the device.
        /// </summary>
        /// <returns></returns>
        syncWithDeviceAsync: function () {
            this._setupBackgroundTask();

            return this._startSyncBackgroundTaskAsync().then(function (deviceTriggerResult) {
                // Determine if we are allowed to sync
                var errorMessage = null;

                switch (deviceTriggerResult) {
                    case Windows.ApplicationModel.Background.DeviceTriggerResult.allowed:
                        syncDevice.isSyncing = true;
                        break;

                    case Windows.ApplicationModel.Background.DeviceTriggerResult.lowBattery:
                        syncDevice.isSyncing = false;
                        errorMessage = "Insufficient battery to start sync";
                        break;

                    case Windows.ApplicationModel.Background.DeviceTriggerResult.deniedByUser:
                        syncDevice.isSyncing = false;
                        errorMessage = "User declined the operation";
                        break;

                    case Windows.ApplicationModel.Background.DeviceTriggerResult.deniedBySystem:
                        syncDevice.isSyncing = false;
                        errorMessage = "Sync operation was denied by the system";
                        break;

                    default:
                        syncDevice.isSyncing = false;
                        errorMessage = "Failed to initiate sync";
                        break;
                }

                if (!syncDevice.isSyncing && errorMessage) {
                    SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.openDeviceAsync(syncDevice._syncDeviceInformation, syncDevice._syncDeviceSelector)
                        .then(function (openDeviceSuccess) {
                            if (!openDeviceSuccess) {
                                SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isEnabledAutoReconnect = false;
                            }

                            syncDevice._syncDeviceInformation = null;
                            syncDevice._syncDeviceSelector = null;

                            WinJS.log && WinJS.log("Unable to sync: " + errorMessage, "sample", "error");
                        });
                }

            });
        },
        _findSyncTask: function () {
            // Unregister and cancel any background tasks may have persisted from this scenario
            var backgroundTaskIter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();

            while (backgroundTaskIter.hasCurrent) {
                var backgroundTask = backgroundTaskIter.current.value;

                if (backgroundTask.name === SdkSample.Constants.syncBackgroundTaskInformation.name) {
                    return backgroundTask;
                }

                backgroundTaskIter.moveNext();
            }

            return null;
        },
        /// <summary>
        /// Starts the background task that syncs with the device
        /// </summary>
        /// <returns></returns>
        _startSyncBackgroundTaskAsync: function () {
            // Save the current device information so that we can reopen it after syncing
            this._syncDeviceInformation = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.deviceInformation;
            this._syncDeviceSelector = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.deviceSelector;

            // Allow the background task to open the device and sync with it.
            // We must close the device because Background tasks need to create new handle to the device and cannot reuse the one from this app.
            // Since the device is opened exclusively, the app must close the device before the background task can use the device.
            SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.closeDevice();

            return this.syncBackgroundTaskTrigger.requestAsync(this._syncDeviceInformation.id);
        },
        /// <summary>
        /// Registers for background task that will write to the device.
        /// </summary>
        _setupBackgroundTask: function () {
            // Create background task to write
            var backgroundTaskBuilder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();

            backgroundTaskBuilder.name = SdkSample.Constants.syncBackgroundTaskInformation.name;
            backgroundTaskBuilder.taskEntryPoint = SdkSample.Constants.syncBackgroundTaskInformation.taskEntryPoint;
            backgroundTaskBuilder.setTrigger(this.syncBackgroundTaskTrigger);

            this.backgroundTaskRegistration = backgroundTaskBuilder.register();

            // Make sure we're notified when the task completes or if there is an update
            this.backgroundTaskRegistration.addEventListener("completed", syncDevice._onSyncWithDeviceCompleted);
            this.backgroundTaskRegistration.addEventListener("progress", syncDevice._onSyncWithDeviceProgress);
        },
        /// <summary>
        /// Reopen the device after the background task is done syncing. Notify the UI of how many bytes we wrote to the device.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        _onSyncWithDeviceCompleted: function (args) {
            syncDevice.isSyncing = false;

            // Exception may be thrown if an error occurs during running the background task
            args.checkResult();

            SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.openDeviceAsync(syncDevice._syncDeviceInformation, syncDevice._syncDeviceSelector)
                .then(function (openDeviceSuccess) {
                    if (!openDeviceSuccess) {
                        SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isEnabledAutoReconnect = false;
                    }

                    syncDevice._syncDeviceInformation = null;
                    syncDevice._syncDeviceSelector = null;

                    // Since we are navigating away, don't touch the UI; we don't care what the output/result of the background task is
                    if (!syncDevice.navigatedAway) {
                        var taskCompleteStatus = Windows.Storage.ApplicationData.current.localSettings.values[SdkSample.Constants.localSettingKeys.syncBackgroundTaskStatus];

                        if (taskCompleteStatus === SdkSample.Constants.syncBackgroundTaskInformation.taskCompleted) {
                            var totalBytesWritten = Windows.Storage.ApplicationData.current.localSettings.values[SdkSample.Constants.localSettingKeys.syncBackgroundTaskResult];

                            // Set the progress bar to be completely filled in case the progress was not updated (this can happen if the app is suspended)
                            window.syncProgressBar.value = 100;

                            WinJS.log && WinJS.log("Sync: Wrote " + totalBytesWritten + " bytes to the device", "sample", "status");
                        } else if (taskCompleteStatus === SdkSample.Constants.syncBackgroundTaskInformation.taskCanceled) {
                            // Reset the progress bar in case the progress was not updated (this can happen if the app is suspended)
                            window.syncProgressBar.value = 0;

                            WinJS.log && WinJS.log("Syncing was canceled", "sample", "status");
                        }

                        updateButtonStates();
                    }

                    // Remove all local setting values
                    Windows.Storage.ApplicationData.current.localSettings.values.clear();

                    // Unregister the background task and let the remaining task finish until completion
                    if (syncDevice.backgroundTaskRegistration) {
                        syncDevice.backgroundTaskRegistration.unregister(false);
                    }
                });
        },
        /// <summary>
        /// Updates the UI with the progress of the sync
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        _onSyncWithDeviceProgress: function (args) {
            window.syncProgressBar.value = args.progress;
        }

    },
    null);

    var syncDevice = new SyncDeviceClass();

    var page = WinJS.UI.Pages.define("/html/scenario7_syncDevice.html", {
        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// 
        /// We will enable/disable parts of the UI if the device doesn't support it.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        ready: function (element, options) {

            syncDevice.navigatedAway = false;

            // Set up Button listeners before hiding scenarios in case the button is removed when hiding scenario
            document.getElementById("buttonSync").addEventListener("click", syncClick, false);
            document.getElementById("buttonCancelSync").addEventListener("click", cancelSyncClick, false);

            // Available scenarios
            var deviceScenarios = {};
            deviceScenarios[SdkSample.Constants.deviceType.osrFx2] = document.querySelector(".generalScenario");
            deviceScenarios[SdkSample.Constants.deviceType.superMutt] = document.querySelector(".generalScenario");

            SdkSample.CustomUsbDeviceAccess.utilities.setUpDeviceScenarios(deviceScenarios, document.querySelector(".deviceScenarioContainer"));

            // Unregister any existing tasks that persisted; there should be none unless the app closed/crashed
            var existingSyncTask = syncDevice._findSyncTask();
            if (existingSyncTask) {
                existingSyncTask.unregister(true);
            }

            syncDevice.isSyncing = false;

            // Don't update button if we removed them (this happens if no supermutt or osrfx2 are connected)
            if (window.buttonSync) {
                updateButtonStates();
            }
        },
        /// <summary>
        /// Cancel the background task so the device can be reopened
        /// </summary>
        /// <param name="eventArgs"></param>
        unload: function () {

            syncDevice.navigatedAway = true;

            syncDevice.cancelSyncWithDevice();
        }
    });

    function syncClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            WinJS && WinJS.log("Syncing in the background...", "sample", status);

            // Reset progress bar
            window.syncProgressBar.value = 0;

            syncDevice.syncWithDeviceAsync().done(function () {
                updateButtonStates();
            });
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function cancelSyncClick(eventArgs) {
        if (syncDevice.isSyncing) {
            syncDevice.cancelSyncWithDevice();

            updateButtonStates();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function updateButtonStates() {
        window.buttonSync.disabled = syncDevice.isSyncing;
        window.buttonCancelSync.disabled = !syncDevice.isSyncing;
    }
})();
