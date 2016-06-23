//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var registerReadingChanged;
    var scenarioOutput;

    var registeredForEvent = false;
    var pedometer = null;
    var totalSteps = 0;
    var unknownStepsCount = 0;
    var walkingStepsCount = 0;
    var runningStepsCount = 0;

    var page = WinJS.UI.Pages.define("/html/scenario1-events.html", {
        ready: function (element, options) {
            registerReadingChanged = document.getElementById("registerReadingChanged");
            scenarioOutput = document.getElementById("scenarioOutput");

            registerReadingChanged.addEventListener("click", registerUnregisterReadingChanged, false);

            return Windows.Devices.Sensors.Pedometer.getDefaultAsync().then(function (defaultPedometer) {
                pedometer = defaultPedometer;
                if (pedometer) {
                    WinJS.log && WinJS.log("Default Pedometer opened", "sample", "status");
                    pedometer.reportInterval = 1000;
                    // enable button if pedometer is successfully opened
                    registerReadingChanged.disabled = false;
                } else {
                    WinJS.log && WinJS.log("No Pedometer found", "sample", "error");
                }
            }, function error(e) {
                WinJS.log && WinJS.log("Error when opening default pedometer. " + e.message, "sample", "error");
            });
        },
        unload: function () {
            document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
            registerReadingChanged.textContent = "Register ReadingChanged";

            // Return the report interval to its default to release resources while the sensor is not in use
            if (pedometer) {
                pedometer.reportInterval = 0;
                pedometer.removeEventListener("readingchanged", onReadingChanged);
                registeredForEvent = false;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.msVisibilityState === "visible") {
            // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
            pedometer.addEventListener("readingchanged", onReadingChanged);
        } else {
            // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
            pedometer.removeEventListener("readingchanged", onReadingChanged);
        }
    }

    /// <summary>
    /// Helper function to register/un-register to the 'readingchanged' event of the default pedometer
    /// </summary>
    function registerUnregisterReadingChanged() {
        if (registeredForEvent) {
            WinJS.log && WinJS.log("Unregistering for event invoked", "sample", "error");
            pedometer.removeEventListener("readingchanged", onReadingChanged, false);
            registerReadingChanged.textContent = "Register ReadingChanged";
            registeredForEvent = false;
            document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
            WinJS.log && WinJS.log("Unregistered for event", "sample", "status");
        } else {
            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            pedometer.addEventListener("readingchanged", onReadingChanged);
            registerReadingChanged.textContent = "Unregister ReadingChanged";
            registeredForEvent = true;
            WinJS.log && WinJS.log("Regsitered for event", "sample", "status");
        }
    }

    /// <summary>
    /// Invoked when the underlying pedometer sees a change in the step count for a step kind.
    /// </summary>
    /// <param name="sender">unused</param>
    function onReadingChanged(e) {
        var reading = e.reading;
        var newStepsTaken = 0;

        scenarioOutput.querySelector("#readingTimeStamp").textContent = SdkSample.formatTimestamp(reading.timestamp);

        readingTimeStamp.textContent = SdkSample.formatTimestamp(reading.timestamp);

        switch (reading.stepKind) {
            case Windows.Devices.Sensors.PedometerStepKind.unknown:
                newStepsTaken = reading.cumulativeSteps - unknownStepsCount;
                unknownStepsCount = reading.cumulativeSteps;
                scenarioOutput.querySelector("#unknownCount").textContent = unknownStepsCount;
                scenarioOutput.querySelector("#unknownDuration").textContent = reading.cumulativeStepsDuration;
                break;
            case Windows.Devices.Sensors.PedometerStepKind.walking:
                newStepsTaken = reading.cumulativeSteps - walkingStepsCount;
                walkingStepsCount = reading.cumulativeSteps;
                scenarioOutput.querySelector("#walkingCount").textContent = walkingStepsCount;
                scenarioOutput.querySelector("#walkingDuration").textContent = reading.cumulativeStepsDuration;
                break;
            case Windows.Devices.Sensors.PedometerStepKind.running:
                newStepsTaken = reading.cumulativeSteps - runningStepsCount;
                runningStepsCount = reading.cumulativeSteps;
                scenarioOutput.querySelector("#runningCount").textContent = reading.cumulativeSteps;
                scenarioOutput.querySelector("#runningDuration").textContent = reading.cumulativeStepsDuration;
                break;
            default:
                WinJS.log && WinJS.log("Unknown Step Kind", "sample", "error");
                break;
        }
        totalSteps += newStepsTaken;
        scenarioOutput.querySelector("#totalStepsCount").textContent = totalSteps;
        WinJS.log && WinJS.log("Getting readings", "sample", "status");
    }
})();
