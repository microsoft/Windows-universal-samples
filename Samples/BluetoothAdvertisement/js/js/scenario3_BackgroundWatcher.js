//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    // The background task registration for the background advertisement watcher
    var taskRegistration;
    // The watcher trigger used to configure the background task registration
    var trigger;
    // A name is given to the task in order for it to be identifiable across context.
    var taskName = "Scenario3_BackgroundTask";
    // Entry point for the background task.
    var taskEntryPoint = "js\\backgroundWatcherTask.js";

    // Create and initialize a new trigger to configure it.
    trigger = new Windows.ApplicationModel.Background.BluetoothLEAdvertisementWatcherTrigger();

    // Configure the advertisement filter to look for the data advertised by the publisher in Scenario 2 or 4.
    // You need to run Scenario 2 on another Windows platform within proximity of this one for Scenario 3 to 
    // take effect.

    // Unlike the APIs in Scenario 1 which operate in the foreground. This API allows the developer to register a background
    // task to process advertisement packets in the background. It has more restrictions on valid filter configuration.
    // For example, exactly one single matching filter condition is allowed (no more or less) and the sampling interval.

    // For determining the filter restrictions programatically across APIs, use the following properties:
    //      MinSamplingInterval, MaxSamplingInterval, MinOutOfRangeTimeout, MaxOutOfRangeTimeout

    // Part 1A: Configuring the advertisement filter to watch for a particular advertisement payload

    // First, let create a manufacturer data section we wanted to match for. These are the same as the one 
    // created in Scenario 2 and 4. Note that in the background only a single filter pattern is allowed per trigger.
    var manufacturerData = new Windows.Devices.Bluetooth.Advertisement.BluetoothLEManufacturerData();

    // Then, set the company ID for the manufacturer data. Here we picked an unused value: 0xFFFE
    manufacturerData.companyId = 0xFFFE;

    // Finally set the data payload within the manufacturer-specific section
    // Here, use a 16-bit UUID: 0x1234 -> {0x34, 0x12} (little-endian)
    var writer = new Windows.Storage.Streams.DataWriter();
    writer.writeUInt16(0x1234);

    // Make sure that the buffer length can fit within an advertisement payload. Otherwise you will get an exception.
    manufacturerData.data = writer.detachBuffer();

    // Add the manufacturer data to the advertisement filter on the trigger:
    trigger.advertisementFilter.advertisement.manufacturerData.append(manufacturerData);

    // Part 1B: Configuring the signal strength filter for proximity scenarios

    // Configure the signal strength filter to only propagate events when in-range
    // Please adjust these values if you cannot receive any advertisement 
    // Set the in-range threshold to -70dBm. This means advertisements with RSSI >= -70dBm 
    // will start to be considered "in-range".
    trigger.signalStrengthFilter.inRangeThresholdInDBm = -70;

    // Set the out-of-range threshold to -75dBm (give some buffer). Used in conjunction with OutOfRangeTimeout
    // to determine when an advertisement is no longer considered "in-range"
    trigger.signalStrengthFilter.outOfRangeThresholdInDBm = -75;

    // Set the out-of-range timeout to be 2 seconds. Used in conjunction with OutOfRangeThresholdInDBm
    // to determine when an advertisement is no longer considered "in-range"
    trigger.signalStrengthFilter.outOfRangeTimeout = 2000;

    // By default, the sampling interval is set to be disabled, or the maximum sampling interval supported.
    // The sampling interval set to MaxSamplingInterval indicates that the event will only trigger once after it comes into range.
    // Here, set the sampling period to 1 second, which is the minimum supported for background.
    trigger.signalStrengthFilter.samplingInterval = 1000;

    var page = WinJS.UI.Pages.define("/html/scenario3_BackgroundWatcher.html", {
        ready: function (element, options) {
            document.getElementById("runButton").addEventListener("click", onRunButtonClick, false);
            document.getElementById("stopButton").addEventListener("click", onStopButtonClick, false);

            if (taskRegistration == null) {
                // Find the task if we previously registered it
                var iter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();
                while (iter.hasCurrent) {
                    var task = iter.current.value;
                    if (task.name === taskName) {
                        taskRegistration = task;
                        taskRegistration.addEventListener("completed", onBackgroundTaskCompleted, false);
                        break;
                    }
                    iter.moveNext();
                }
            } else {
                taskRegistration.addEventListener("completed", onBackgroundTaskCompleted, false);
            }

            // Attach handlers for suspension to stop the watcher when the App is suspended.
            Windows.UI.WebUI.WebUIApplication.addEventListener("suspending", onSuspending, false);
            Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", onResuming, false);

            WinJS.log && WinJS.log("Press Run to register watcher.", "sample", "status");
        },
        unload: function (element, options) {
            // Remove local suspension handlers from the App since this page is no longer active.
            Windows.UI.WebUI.WebUIApplication.removeEventListener("suspending", onSuspending);
            Windows.UI.WebUI.WebUIApplication.removeEventListener("resuming", onResuming);

            // Since the watcher is registered in the background, the background task will be triggered when the App is closed 
            // or in the background. To unregister the task, press the Stop button.
            if (taskRegistration != null) {
                // Always unregister the handlers to release the resources to prevent leaks.
                taskRegistration.removeEventListener("completed", onBackgroundTaskCompleted);
            }
        }
    });

    /// <summary>
    /// Invoked when application execution is being suspended.  Application state is saved
    /// without knowing whether the application will be terminated or resumed with the contents
    /// of memory still intact.
    /// </summary>
    /// <param name="args">Details about the suspend request.</param>
    function onSuspending(args) {
        if (taskRegistration != null) {
            // Always unregister the handlers to release the resources to prevent leaks.
            taskRegistration.removeEventListener("completed", onBackgroundTaskCompleted);
        }

        WinJS.log && WinJS.log("App suspending.", "sample", "status");
    }

    /// <summary>
    /// Invoked when application execution is being resumed.
    /// </summary>
    /// <param name="args"></param>
    function onResuming(args) {
        if (taskRegistration == null) {
            // Find the task if we previously registered it
            var iter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();
            while (iter.hasCurrent) {
                var task = iter.current.value;
                if (task.name === taskName) {
                    taskRegistration = task;
                    taskRegistration.addEventListener("completed", onBackgroundTaskCompleted, false);
                    break;
                }
                iter.moveNext();
            }
        } else {
            taskRegistration.addEventListener("completed", onBackgroundTaskCompleted, false);
        }
    }

    /// <summary>
    /// Invoked as an event handler when the Run button is pressed.
    /// </summary>
    /// <param name="args">Event data describing the conditions that led to the event.</param>
    function onRunButtonClick(args) {
        // Registering a background trigger if it is not already registered. It will start background scanning.
        // First get the existing tasks to see if we already registered for it
        if (taskRegistration != null) {
            WinJS.log && WinJS.log("Background watcher already registered.", "sample", "status");
        } else {
            // Applications registering for background trigger must request for permission.
            Windows.ApplicationModel.Background.BackgroundExecutionManager.requestAccessAsync().done(function (backgroundAccessStatus) {
                // Here, we do not fail the registration even if the access is not granted. Instead, we allow 
                // the trigger to be registered and when the access is granted for the Application at a later time,
                // the trigger will automatically start working again.

                // At this point we assume we haven't found any existing tasks matching the one we want to register
                // First, configure the task entry point, trigger and name
                var builder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();
                builder.taskEntryPoint = taskEntryPoint;
                builder.setTrigger(trigger);
                builder.name = taskName;

                // Now perform the registration. The registration can throw an exception if the current 
                // hardware does not support background advertisement offloading
                try {
                    taskRegistration = builder.register();

                    // For this scenario, attach an event handler to display the result processed from the background task
                    taskRegistration.addEventListener("completed", onBackgroundTaskCompleted, false);

                    // Even though the trigger is registered successfully, it might be blocked. Notify the user if that is the case.
                    if ((backgroundAccessStatus == Windows.ApplicationModel.Background.BackgroundAccessStatus.alwaysAllowed) ||
                        (backgroundAccessStatus == Windows.ApplicationModel.Background.BackgroundAccessStatus.allowedSubjectToSystemPolicy)) {
                        WinJS.log && WinJS.log("Background watcher registered.", "sample", "status");
                    } else {
                        WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "error");
                    }
                } catch (ex) {
                    switch (ex.number) {
                        case (-2147024846): // ERROR_NOT_SUPPORTED
                            WinJS.log && WinJS.log("The hardware does not support background advertisement offload.", "sample", "error");
                            break;
                        default:
                            throw ex;
                    }
                }
            });
        }
    }

    /// <summary>
    /// Invoked as an event handler when the Stop button is pressed.
    /// </summary>
    /// <param name="args">Event data describing the conditions that led to the event.</param>
    function onStopButtonClick(args) {
        // Unregistering the background task will stop scanning if this is the only client requesting scan
        // First get the existing tasks to see if we already registered for it
        if (taskRegistration != null) {
            taskRegistration.unregister(true);
            taskRegistration = null;
            WinJS.log && WinJS.log("Background watcher unregistered.", "sample", "status");
        } else {
            // At this point we assume we haven't found any existing tasks matching the one we want to unregister
            WinJS.log && WinJS.log("No registered background watcher found.", "sample", "status");
        }
    }

    /// <summary>
    /// Handle background task completion.
    /// </summary>
    /// <param name="e">Arguments of the completion report.</param>
    function onBackgroundTaskCompleted(e) {
        // We get the advertisement(s) processed by the background task
        var values = Windows.Storage.ApplicationData.current.localSettings.values;
        if (values.hasKey(taskName)) {
            // Display these information on the list
            document.getElementById("receivedAdvertisementListBox").add(new Option(values[taskName]));
        }
    }
})();
