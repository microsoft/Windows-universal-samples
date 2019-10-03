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

    // Namespaces and types
    var Background = Windows.ApplicationModel.Background;
    var BackgroundExecutionManager = Background.BackgroundExecutionManager;
    var BackgroundTaskRegistration = Background.BackgroundTaskRegistration;
    var BackgroundAccessStatus = Background.BackgroundAccessStatus;

    // Pedometer that will be used for background monitoring
    var pedometer = null;
    // The background task registration for the background pedometer
    var taskRegistration = null;
    // A name is given to the task in order for it to be identifiable across context.
    var taskName = "scenario4-pedometerBackgroundTask";
    // Entry point for the background task.
    var taskEntryPoint = "js\\pedometerBackgroundTask.js";
    // Current background task, if active
    var backgroundTask = false;
    // current step count of the default pedometer
    var currentStepCount = 0;
    // goal for this illustration
    var stepGoalOffset = 50;
    // Cumulative step count goal that will be used
    var stepCountGoal = 0;

    var registerBackgroundTaskButton;
    var unregisterBackgroundTaskButton;
    var scenarioOutput;

    var page = WinJS.UI.Pages.define("/html/scenario4-backgroundPedometer.html", {
        ready: function (element, options) {
            registerBackgroundTaskButton = document.getElementById("registerBackgroundTaskButton");
            unregisterBackgroundTaskButton = document.getElementById("unregisterBackgroundTaskButton");
            scenarioOutput = document.getElementById("scenarioOutput");

            registerBackgroundTaskButton.addEventListener("click", registerBackgroundTask, false);
            unregisterBackgroundTaskButton.addEventListener("click", unregisterBackgroundTask, false);

            // Look for the background task.
            for (var iter = BackgroundTaskRegistration.allTasks.first(); iter.hasCurrent; iter.moveNext) {
                var task = iter.current.value;
                if (task.name == taskName) {
                    backgroundTask = task;
                    break;
                }
            }

            if (backgroundTask) {
                // For this scenario, attach an event handler to display the result processed from the background task
                backgroundTask.addEventListener("completed", onBackgroundTaskCompleted, false);
            } else {
                // reset the state of the application data if we were not already registered
                var values = Windows.Storage.ApplicationData.current.localSettings.values;
                values["ReportCount"] = "0";
                values["LastTimestamp"] = "";
                values["StepKind0"] = "0";
                values["StepKind1"] = "0";
                values["StepKind2"] = "0";
                values["TaskStatus"] = "";
            }
            updateUI();

            Windows.Devices.Sensors.Pedometer.getDefaultAsync().then(function (defaultPedometer) {
                pedometer = defaultPedometer;
                if (pedometer) {
                    WinJS.log && WinJS.log("Pedometer is available to register for background monitoring", "sample", "status");
                    pedometer.reportInterval = 1000;
                } else {
                    WinJS.log && WinJS.log("No Pedometer found", "sample", "error");
                }
            }, function error(e) {
                WinJS.log && WinJS.log("Error when opening default pedometer. " + e.message, "sample", "error");
            });
        },
        unload: function () {
            // Unregister our completion handler since we won't be around to respond to it.
            if (backgroundTask) {
                backgroundTask.removeEventListener("completed", onBackgroundTaskCompleted);
            }
        }
    });

    /// <summary>
    /// registers the pedometer background task for the changes on the default pedometer
    /// </summary>
    function registerBackgroundTask() {
        if (pedometer) {
            // Applications registering for background trigger must request for permission.
            BackgroundExecutionManager.requestAccessAsync().done(function (backgroundAccessStatus) {
                // Here, we do not fail the registration even if the access is not granted. Instead, we allow 
                // the trigger to be registered and when the access is granted for the Application at a later time,
                // the trigger will automatically start working again.

                // Get Current pedometer reading to compute the new step goal
                var readings = pedometer.getCurrentReadings();
                currentStepCount = 0;
                for (var iter = readings.first(); iter.hasCurrent; iter.moveNext()) {
                    var reading = iter.current.value;
                    currentStepCount += reading.cumulativeSteps;
                }

                // use the 'stepGoalOffset' to arrive at the step goal
                stepCountGoal = currentStepCount + stepGoalOffset;
                
                scenarioOutput.querySelector("#currentStepCount").textContent = currentStepCount;
                scenarioOutput.querySelector("#stepGoal").textContent = stepCountGoal;

                // First, create a pedometer data threshold
                var threshold = new Windows.Devices.Sensors.PedometerDataThreshold(pedometer, stepCountGoal);
                // Next, create a trigger using the threshold
                var trigger = new Background.SensorDataThresholdTrigger(threshold);
                // Finally, configure the task entry point, trigger and name
                var builder = new Background.BackgroundTaskBuilder();
                builder.taskEntryPoint = taskEntryPoint;
                builder.setTrigger(trigger);
                builder.name = taskName;

                // Now perform the registration
                backgroundTask = builder.register();

                // For this scenario, attach an event handler to display the result processed from the background task
                backgroundTask.addEventListener("completed", onBackgroundTaskCompleted, false);

                // Even though the trigger is registered successfully, it might be blocked. Notify the user if that is the case.
                if ((backgroundAccessStatus == BackgroundAccessStatus.alwaysAllowed) ||
                    (backgroundAccessStatus == BackgroundAccessStatus.allowedSubjectToSystemPolicy)) {
                    WinJS.log && WinJS.log("Pedometer background watcher registered.", "sample", "status");
                } else {
                    WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "error");
                }
                updateUI();
            });
        } else {
            WinJS.log && WinJS.log("No Pedometer found", "sample", "error");
        }
    }

    /// <summary>
    /// Handles background task completion.
    /// </summary>
    /// <param name="e">Arguments of the completion task.</param>
    function onBackgroundTaskCompleted(e) {
        WinJS.log && WinJS.log("Pedometer background task completed.", "sample", "status");
        var status = "Completed and Unregistered";
        try {
            e.checkResult();
        } catch (ex) {
            status = ex.message;
        }
        // Pedometer background task is special as the trigger is one-shot.
        // Once the step goal has been met, we need to re-register with a new step goal 
        // or unregister it. This sample unregisters it.
        backgroundTask.removeEventListener("completed", onBackgroundTaskCompleted);
        backgroundTask.unregister(true /* cancelTask */);
        backgroundTask = null;
        WinJS.log && WinJS.log("Background watcher unregistered.", "sample", "status");

        updateUI(status);
    }

    /// <summary>
    /// udpates UI with the given status.
    /// </summary>
    /// <param name="status">status to be reported</param>
    function updateUI(status) {
        // update the button states based on the registration status
        registerBackgroundTaskButton.disabled = backgroundTask;
        unregisterBackgroundTaskButton.disabled = !backgroundTask;
        if (!status) {
            if (backgroundTask) {
                status = "Registered";
            } else {
                status = "Unregistered";
            }
        }
        scenarioOutput.querySelector("#registrationStatus").textContent = status;

        var values = Windows.Storage.ApplicationData.current.localSettings.values;

        // Display application data cached from background task
        if (values.hasKey("TaskStatus")) {
            scenarioOutput.querySelector("#taskStatus").textContent = values["TaskStatus"];
        }

        if (values.hasKey("ReportCount")) {
            scenarioOutput.querySelector("#reportCount").textContent = values["ReportCount"];
        }

        if (values.hasKey("LastTimestamp")) {
            scenarioOutput.querySelector("#lastReadingTimestamp").textContent = values["LastTimestamp"];
        }

        if (values.hasKey("StepKind0")) {
            scenarioOutput.querySelector("#unknownStepsCount").textContent = values["StepKind0"];
        }

        if (values.hasKey("StepKind1")) {
            scenarioOutput.querySelector("#walkingStepsCount").textContent = values["StepKind1"];
        }

        if (values.hasKey("StepKind2")) {
            scenarioOutput.querySelector("#runningStepsCount").textContent = values["StepKind2"];
        }

    }

    /// <summary>
    /// Unregisters the pedometer background task if found in the current set of tasks
    /// </summary>
    function unregisterBackgroundTask() {
        backgroundTask.removeEventListener("completed", onBackgroundTaskCompleted);
        backgroundTask.unregister(true /* cancelTask */);
        backgroundTask = null;

        updateUI();
        WinJS.log && WinJS.log("Background watcher unregistered.", "sample", "status");
    }
})();
