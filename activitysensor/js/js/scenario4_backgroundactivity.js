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

    var sampleBackgroundTaskName = "ActivitySensorBackgroundTask";
    var sampleBackgroundTaskEntryPoint = "Tasks.ActivitySensorBackgroundTask";
    var registered = false;

    var page = WinJS.UI.Pages.define("/html/scenario4_backgroundActivity.html", {
        ready: function (element, options) {
            document.getElementById("scenario4RegisterTask").addEventListener("click", registerTaskScenario, false);
            document.getElementById("scenario4UnregisterTask").addEventListener("click", unregisterTaskScenario, false);

            var iter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();
            var hasCur = iter.hasCurrent;
            while (hasCur && !registered) {
                var cur = iter.current.value;
                if (cur.name === sampleBackgroundTaskName) {
                    registered = true;
                }
                hasCur = iter.moveNext();
            }
            updateUI(registered ? "Registered" : "Unregistered");
        }
    });

    /// <summary>
    /// This is the click handler for the 'Register Task' button.
    /// </summary>
    function registerTaskScenario() {
        Windows.ApplicationModel.Background.BackgroundExecutionManager.requestAccessAsync().done(function (status) {
            var backgroundAccessStatus = Windows.ApplicationModel.Background.BackgroundAccessStatus;
            if (backgroundAccessStatus.allowedWithAlwaysOnRealTimeConnectivity === status ||
                backgroundAccessStatus.allowedMayUseActiveRealTimeConnectivity === status) {
                registerBackgroundTask();
            } else {
                WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "error");
            }
        });
    }

    /// <summary>
    /// This is the click handler for the 'Unregister Task' button.
    /// </summary>
    function unregisterTaskScenario() {
        // Loop through all background tasks and unregister any with SampleBackgroundTaskName
        var iter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();
        var hasCur = iter.hasCurrent;
        while (hasCur) {
            var cur = iter.current.value;
            if (cur.name === sampleBackgroundTaskName) {
                cur.unregister(true /*cancelTask*/);
                registered = false;
            }
            hasCur = iter.moveNext();
        }

        if (!registered) {
            updateUI("Unregistered");
        }
    }

    /// <summary>
    /// This is the event handler for background task completion.
    /// </summary>
    /// <param name="task">The task that is reporting completion.</param>
    /// <param name="args">The completion report arguments.</param>
    function onCompleted(args) {
        var status = "Completed";
        try {
            args.checkResult();
        } catch (e) {
            status = e.message;
        }
        updateUI(status);
    }

    /// <summary>
    /// Registers the background task.
    /// </summary>
    function registerBackgroundTask() {
        var builder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();
        builder.name = sampleBackgroundTaskName;
        builder.taskEntryPoint = sampleBackgroundTaskEntryPoint;

        var reportIntervalMs = 3000; // 3 seconds
        var trigger = new Windows.ApplicationModel.Background.ActivitySensorTrigger(reportIntervalMs);

        trigger.subscribedActivities.push(Windows.Devices.Sensors.ActivityType.walking);
        trigger.subscribedActivities.push(Windows.Devices.Sensors.ActivityType.running);
        trigger.subscribedActivities.push(Windows.Devices.Sensors.ActivityType.biking);

        builder.setTrigger(trigger);
        var task = builder.register();
        task.addEventListener("completed", onCompleted);

        registered = true;
        updateUI("Registered");
    }

    /// <summary>
    /// Updates the scenario UI.
    /// </summary>
    /// <param name="statusText">The status text to display.</param>
    function updateUI(statusText) {
        document.getElementById("scenario4RegisterTask").disabled = registered;
        document.getElementById("scenario4UnregisterTask").disabled = !registered;
        document.getElementById("taskRegistrationOutput").innerText = statusText;

        var values = Windows.Storage.ApplicationData.current.localSettings.values;
        if (values.hasKey("reportCount")) {
            document.getElementById("reportCountOutput").innerText = values["reportCount"];
        }
        if (values.hasKey("taskStatus")) {
            document.getElementById("taskStatusOutput").innerText = values["taskStatus"];
        }
        if (values.hasKey("lastActivity")) {
            document.getElementById("lastActivityOutput").innerText = values["lastActivity"];
        }
        if (values.hasKey("lastConfidence")) {
            document.getElementById("lastConfidenceOutput").innerText = values["lastConfidence"];
        }
        if (values.hasKey("lastTimestamp")) {
            document.getElementById("lastTimestampOutput").innerText = values["lastTimestamp"];
        }
    }
})();
