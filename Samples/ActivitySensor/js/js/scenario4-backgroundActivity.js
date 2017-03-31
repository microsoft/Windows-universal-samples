//// Copyright (c) Microsoft Corporation. All rights reserved

/// <dictionary>args,iter,unregister</dictionary>
(function () {
    "use strict";

    var Background = Windows.ApplicationModel.Background;
    var BackgroundExecutionManager = Background.BackgroundExecutionManager;
    var BackgroundTaskRegistration = Background.BackgroundTaskRegistration;
    var BackgroundAccessStatus = Background.BackgroundAccessStatus;

    var sampleBackgroundTaskName = "ActivitySensorBackgroundTask";
    var sampleBackgroundTaskEntryPoint = "js\\activitySensorBackgroundTask.js";
    var backgroundTask;

    var registerTaskButton;
    var unregisterTaskButton;
    var scenarioOutput;
    var taskRegistrationOutput;

    var page = WinJS.UI.Pages.define("/html/scenario4-backgroundActivity.html", {
        ready: function (element, options) {
            registerTaskButton = document.getElementById("registerTask");
            unregisterTaskButton = document.getElementById("unregisterTask");
            scenarioOutput = document.getElementById("scenarioOutput");

            registerTaskButton.addEventListener("click", registerTaskScenario, false);
            unregisterTaskButton.addEventListener("click", unregisterTaskScenario, false);

            // Look for the background task.
            for (var iter = BackgroundTaskRegistration.allTasks.first(); iter.hasCurrent; iter.moveNext) {
                var task = iter.current.value;
                if (task.name == sampleBackgroundTaskName) {
                    backgroundTask = task;
                    break;
                }
            }

            if (backgroundTask) {
                backgroundTask.addEventListener("completed", onBackgroundTaskCompleted);
                updateUI("Registered");
            } else {
                updateUI("Unregistered");
            }
        }
    });

    /// <summary>
    /// This is the click handler for the 'Register Task' button.
    /// </summary>
    function registerTaskScenario() {
        registerTaskButton.disabled = true;
        BackgroundExecutionManager.requestAccessAsync().done(function (status) {
            if (BackgroundAccessStatus.alwaysAllowed === status ||
                BackgroundAccessStatus.allowedSubjectToSystemPolicy === status) {
                registerBackgroundTask();
            } else {
                WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "error");
                registerTaskButton.disabled = false;
            }
        });
    }

    /// <summary>
    /// This is the click handler for the 'Unregister Task' button.
    /// </summary>
    function unregisterTaskScenario() {
        backgroundTask.unregister(true /*cancelTask*/);
        backgroundTask = null;
        updateUI("Unregistered");
    }

    /// <summary>
    /// This is the event handler for background task completion.
    /// </summary>
    /// <param name="task">The task that is reporting completion.</param>
    /// <param name="args">The completion report arguments.</param>
    function onBackgroundTaskCompleted(args) {
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
        var builder = new Background.BackgroundTaskBuilder();
        builder.name = sampleBackgroundTaskName;
        builder.taskEntryPoint = sampleBackgroundTaskEntryPoint;

        var reportIntervalMs = 3000; // 3 seconds
        var trigger = new Background.ActivitySensorTrigger(reportIntervalMs);

        trigger.subscribedActivities.push(
            Windows.Devices.Sensors.ActivityType.walking,
            Windows.Devices.Sensors.ActivityType.running,
            Windows.Devices.Sensors.ActivityType.biking);

        builder.setTrigger(trigger);
        backgroundTask = builder.register();
        backgroundTask.addEventListener("completed", onBackgroundTaskCompleted);

        updateUI("Registered");
    }

    /// <summary>
    /// Updates the scenario UI.
    /// </summary>
    /// <param name="statusText">The status text to display.</param>
    function updateUI(statusText) {
        registerTaskButton.disabled = backgroundTask;
        unregisterTaskButton.disabled = !backgroundTask;

        scenarioOutput.querySelector("#taskRegistration").innerText = statusText;

        var values = Windows.Storage.ApplicationData.current.localSettings.values;
        if (values.hasKey("reportCount")) {
            scenarioOutput.querySelector("#reportCount").innerText = values["reportCount"];
        }
        if (values.hasKey("taskStatus")) {
            scenarioOutput.querySelector("#taskStatus").innerText = values["taskStatus"];
        }
        if (values.hasKey("lastActivity")) {
            scenarioOutput.querySelector("#lastActivity").innerText = values["lastActivity"];
        }
        if (values.hasKey("lastConfidence")) {
            scenarioOutput.querySelector("#lastConfidence").innerText = values["lastConfidence"];
        }
        if (values.hasKey("lastTimestamp")) {
            scenarioOutput.querySelector("#lastTimestamp").innerText = values["lastTimestamp"];
        }
    }
})();
