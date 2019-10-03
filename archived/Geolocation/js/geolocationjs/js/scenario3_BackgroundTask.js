//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    var Background = Windows.ApplicationModel.Background;
    var geolocTask;
    var sampleBackgroundTaskName = "SampleLocationBackgroundTask";
    var sampleBackgroundTaskEntryPoint = "js\\backgroundtask.js";
    var disposed;

    var page = WinJS.UI.Pages.define("/html/scenario3_BackgroundTask.html", {
        ready: function (element, options) {
            disposed = false;
            document.getElementById("registerBackgroundTaskButton").addEventListener("click", registerBackgroundTask, false);
            document.getElementById("unregisterBackgroundTaskButton").addEventListener("click", unregisterBackgroundTask, false);

            // Loop through all background tasks to see if our task is already registered
            var iter = Windows.ApplicationModel.Background.BackgroundTaskRegistration.allTasks.first();
            var hascur = iter.hasCurrent;
            while (hascur) {
                var cur = iter.current.value;
                if (cur.name === sampleBackgroundTaskName) {
                    geolocTask = cur;
                    break;
                }
                hascur = iter.moveNext();
            }

            if (geolocTask) {

                // Associate an event handler to the existing background task
                geolocTask.addEventListener("completed", onCompleted);
                try {
                    var backgroundAccessStatus = Background.BackgroundExecutionManager.getAccessStatus();
                    switch (backgroundAccessStatus) {
                        case Background.BackgroundAccessStatus.alwaysAllowed:
                        case Background.BackgroundAccessStatus.allowedSubjectToSystemPolicy:
                            WinJS.log && WinJS.log("Background task is already registered. Waiting for next update...", "sample", "status");
                            break;

                        default:
                            WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "status");
                            break;
                    }
                } catch (ex) {
                    WinJS.log && WinJS.log(ex.toString(), "sample", "error");
                }

                TimerTriggerBackgroundTask.updateButtonStates(/*registered:*/ true);
            } else {
                TimerTriggerBackgroundTask.updateButtonStates(/*registered:*/ false);
            }
        },
        dispose: function () {
            if (!disposed) {
                disposed = true;

                if (geolocTask) {
                    geolocTask.removeEventListener("completed", onCompleted);
                }
            }
        }
    });

    // Handle background task completion
    function onCompleted() {
        try {
            var settings = Windows.Storage.ApplicationData.current.localSettings;

            WinJS.log && WinJS.log(settings.values["Status"], "sample", "status");

            document.getElementById("latitude").innerText = settings.values["Latitude"];
            document.getElementById("longitude").innerText = settings.values["Longitude"];
            document.getElementById("accuracy").innerText = settings.values["Accuracy"];
        } catch (ex) {
            WinJS.log && WinJS.log(ex.toString(), "sample", "error");
        }
    }

    function registerBackgroundTask() {
        try {
            // Get permission for a background task from the user. If the user has already answered once,
            // this does nothing and the user must manually update their preference via PC Settings.
            Background.BackgroundExecutionManager.requestAccessAsync().done(
                function (backgroundAccessStatus) {
                    var builder = new Windows.ApplicationModel.Background.BackgroundTaskBuilder();

                    // Regardless of the answer, register the background task. If the user later adds this application
                    // to the lock screen, the background task will be ready to run.

                    // Create a new background task builder
                    builder.name = sampleBackgroundTaskName;
                    builder.taskEntryPoint = sampleBackgroundTaskEntryPoint;

                    // Create a new timer triggering at a 15 minute interval
                    // Associate the timer trigger with the background task builder
                    builder.setTrigger(new Windows.ApplicationModel.Background.TimeTrigger(15, false));

                    // Register the background task
                    geolocTask = builder.register();

                    // Associate an event handler with the new background task
                    geolocTask.addEventListener("completed", onCompleted);

                    TimerTriggerBackgroundTask.updateButtonStates(/*registered:*/ true);

                    switch (backgroundAccessStatus) {
                        case Background.BackgroundAccessStatus.alwaysAllowed:
                        case Background.BackgroundAccessStatus.allowedSubjectToSystemPolicy:
                            WinJS.log && WinJS.log("Background task registered.", "sample", "status");

                            // Need to request access to location
                            // This must be done with the background task registration
                            // because the background task cannot display UI.
                            requestLocationAccess();
                            break;

                        default:
                            WinJS.log && WinJS.log("Background tasks may be disabled for this app", "sample", "error");
                            break;
                    }
                },
                function (e) {
                    // Did you forget to do the background task declaration in the package manifest?
                    WinJS.log && WinJS.log(e.toString(), "sample", "error");
                }
            );
        } catch (ex) {
            WinJS.log && WinJS.log(ex.toString(), "sample", "error");
        }
    }

    function unregisterBackgroundTask() {
        // Unregister the background task
        if (geolocTask) {
            geolocTask.unregister(true);
            geolocTask = null;
        }

        WinJS.log && WinJS.log("Background task unregistered", "sample", "status");

        document.getElementById("latitude").innerHTML = "No data";
        document.getElementById("longitude").innerHTML = "No data";
        document.getElementById("accuracy").innerHTML = "No data";

        TimerTriggerBackgroundTask.updateButtonStates(/*registered:*/ false);
    }

    function requestLocationAccess() {
        Windows.Devices.Geolocation.Geolocator.requestAccessAsync().done(
        function (accessStatus) {
            switch (accessStatus) {
                case Windows.Devices.Geolocation.GeolocationAccessStatus.allowed:
                    break;

                case Windows.Devices.Geolocation.GeolocationAccessStatus.denied:
                    WinJS.log && WinJS.log("Access to location is denied.", "sample", "error");
                    break;

                case Windows.Devices.Geolocation.GeolocationAccessStatus.unspecified:
                    WinJS.log && WinJS.log("Unspecified error!", "sample", "error");
                    break;
            }
        },
        function (err) {
            WinJS.log && WinJS.log(err, "sample", "error");
        });
    }
})();

// Update the disabled state of the register/unregister buttons.
var TimerTriggerBackgroundTask = {
    "updateButtonStates": function (registered) {
        var registerButton = document.getElementById("registerBackgroundTaskButton");
        var unregisterButton = document.getElementById("unregisterBackgroundTaskButton");

        registerButton && (registerButton.disabled = registered);
        unregisterButton && (unregisterButton.disabled = !registered);
    }
};