//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

(function () {
    "use strict";

    // Proximity sensor that will be used for background monitoring
    var proximitySensor = null;
    // Proximity sensor watcher
    var watcher = null;
    // The background task registration for the background proximity sensor
    var taskRegistration = null;
    // A name is given to the task in order for it to be identifiable across context.
    var taskName = "Scenario4_ProximitySensorBackgroundTask";
    // Entry point for the background task.
    var taskEntryPoint = "js\\proximitySensorBackgroundTask.js";
    // Current registration status
    var registered = false;

    var page = WinJS.UI.Pages.define("/html/scenario4_BackgroundProximitySensor.html", {
        ready: function (element, options) {
            document.getElementById("registerBackgroundTask").addEventListener("click", registerBackgroundTask, false);
            document.getElementById("unregisterBackgroundTask").addEventListener("click", unregisterBackgroundTask, false);
            
            // Loop through all background tasks and unregister any with SampleBackgroundTaskName
            var iter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();
            while (iter.hasCurrent) {
                var task = iter.current.value;
                if (task.name === taskName) {
                    registered = true;
                    break;
                }
                iter.moveNext();
            }
            updateUI("");

            var proximitySensorDeviceSelector = Windows.Devices.Sensors.ProximitySensor.getDeviceSelector();

            watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(proximitySensorDeviceSelector, null);
            watcher.addEventListener("added", onProximitySensorAdded);
            watcher.start();
        },
        unload: function () {
            watcher && watcher.removeEventListener("added", onProximitySensorAdded);
        }
    });

    /// <summary>
    /// Invoked when the device watcher finds a matching proximity sensor device
    /// </summary>
    /// <param name="device">device information for the proximity sensor that was found</param>
    function onProximitySensorAdded(device) {
        if (!proximitySensor) {
            // get the sensor from the id of the device that is reported
            var foundSensor = Windows.Devices.Sensors.ProximitySensor.fromId(device.id);
            if (foundSensor) {
                proximitySensor = foundSensor;
            } else {
                WinJS.log && WinJS.log("Could not get a proximity sensor from the device id", "sample", "error");
            }
        }
    }

    /// <summary>
    /// registers the proximity sensor background task for the changes on 'proximitySensor'
    /// </summary>
    function registerBackgroundTask() {
        if (proximitySensor) {
            // Applications registering for background trigger must request for permission.
            Windows.ApplicationModel.Background.BackgroundExecutionManager.requestAccessAsync().done(function (backgroundAccessStatus) {
                // Here, we do not fail the registration even if the access is not granted. Instead, we allow 
                // the trigger to be registered and when the access is granted for the Application at a later time,
                // the trigger will automatically start working again.

                // At this point we assume we haven't found any existing tasks matching the one we want to register
                // First, create a proximity sensor data threshold
                var threshold = new Windows.Devices.Sensors.ProximitySensorDataThreshold(proximitySensor);
                // create a trigger using the threshold
                var trigger = new Windows.ApplicationModel.Background.SensorDataThresholdTrigger(threshold);
                
                // Next, configure the task entry point, trigger and name
                var builder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();
                builder.taskEntryPoint = taskEntryPoint;
                builder.setTrigger(trigger);
                builder.name = taskName;

                // Now perform the registration
                taskRegistration = builder.register();

                // For this scenario, attach an event handler to display the result processed from the background task
                taskRegistration.addEventListener("completed", onBackgroundTaskCompleted, false);

                // Even though the trigger is registered successfully, it might be blocked. Notify the user if that is the case.
                if ((backgroundAccessStatus == Windows.ApplicationModel.Background.BackgroundAccessStatus.alwaysAllowed) ||
                    (backgroundAccessStatus == Windows.ApplicationModel.Background.BackgroundAccessStatus.allowedSubjectToSystemPolicy)) {
                    WinJS.log && WinJS.log("Proximity sensor background watcher registered.", "sample", "status");
                } else {
                    WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "error");
                }
                registered = true;
                updateUI("");
            });
        } else {
            WinJS.log && WinJS.log("No proximity sensor found", "sample", "error");
        }
    }

    /// <summary>
    /// Handle background task completion.
    /// </summary>
    /// <param name="e">Arguments of the completion report.</param>
    function onBackgroundTaskCompleted(e) {
        WinJS.log && WinJS.log("Proximity sensor background task completed.", "sample", "status");
        var status = "Completed";
        try {
            e.checkResult();
        }
        catch (ex) {
            status = ex.message;
        }
        updateUI(status);
    }

    /// <summary>
    /// udpates UI with the given status.
    /// </summary>
    /// <param name="status">status to be reported</param>
    function updateUI(status) {
        // update the button states based on the registration status
        document.getElementById("registerBackgroundTask").disabled = registered;
        document.getElementById("unregisterBackgroundTask").disabled = !registered;
        if (status === "") {
            if (registered) {
                status = "Registered";
            } else {
                status = "Unregistered";
            }
        }
        document.getElementById("registrationStatus").textContent = status;

        var values = Windows.Storage.ApplicationData.current.localSettings.values;

        // Display application data cached from background task
        if (values.hasKey("TaskStatus")) {
            document.getElementById("taskStatus").textContent = values["TaskStatus"];
        }

        if (values.hasKey("ReportCount")) {
            document.getElementById("reportCount").textContent = values["ReportCount"];
        }

        if (values.hasKey("LastTimestamp")) {
            document.getElementById("lastReadingTimestamp").textContent = values["LastTimestamp"];
        }

        if (values.hasKey("Detected")) {
            document.getElementById("detectionStatus").textContent = values["Detected"];
        }

    }

    /// <summary>
    /// Unregisters the proximity sensor background task if found in the current set of tasks
    /// </summary>
    function unregisterBackgroundTask() {
        // Loop through all background tasks and unregister any with SampleBackgroundTaskName
        var iter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();
        while (iter.hasCurrent) {
            var task = iter.current.value;
            if (task.name === taskName) {
                task.unregister(true);
                registered = false;
                updateUI("");
                WinJS.log && WinJS.log("Background watcher unregistered.", "sample", "status");
                break;
            }
            iter.moveNext();
        }
    }
})();
