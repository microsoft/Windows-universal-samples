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

    var page = WinJS.UI.Pages.define("/html/scenario1_Events.html", {
        ready: function (element, options) {
            document.getElementById("registerReadingChanged").addEventListener("click", registerUnregisterReadingChanged, false);
            Windows.Devices.Sensors.Pedometer.getDefaultAsync().then(function (pedometer) {
                if (!pedometer) {
                    WinJS.log && WinJS.log("No Pedomter found", "sample", "error");
                }
                else {
                    WinJS.log && WinJS.log("Default Pedometer opened", "sample", "status");
                    pedometer.reportInterval = 1000;
                }
                defaultPedometer = pedometer;
            },
            function error(e) {
                WinJS.log && WinJS.log("Error when opening default pedometer" + e.message + " " + e.description, "sample", "error");
            }
            )
        },
        unload: function () {
            document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
            defaultPedometer.removeEventListener("readingchanged", onReadingChanged);
            document.getElementById("registerReadingChanged").innerText = "Register ReadingChanged";
            registeredForEvent = false;

            // Return the report interval to its default to release resources while the sensor is not in use
            defaultPedometer.reportInterval = 0;
        }
    });

    var registeredForEvent = false;
    var defaultPedometer = null;
    var totalStepsCount = 0;
    var unknownStepsCount = 0;
    var walkingStepsCount = 0;
    var runningStepsCount = 0;


    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.msVisibilityState === "visible") {
            // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
            defaultPedometer.addEventListener("readingchanged", onReadingChanged);
        } else {
            // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
            defaultPedometer.removeEventListener("readingchanged", onReadingChanged);
        }
    }

    /// <summary>
    /// Helper function to register/un-register to the 'readingchanged' event of the default pedometer
    /// </summary>
    function registerUnregisterReadingChanged() {
        if (registeredForEvent) {
            WinJS.log && WinJS.log("Unregistering for event invoked", "sample", "error");
            defaultPedometer.removeEventListener("readingchanged", onReadingChanged, false);
            document.getElementById("registerReadingChanged").innerText = "Register ReadingChanged";
            registeredForEvent = false;
            document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
            WinJS.log && WinJS.log("Unregsitered for event", "sample", "status");
        }
        else {
            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            defaultPedometer.addEventListener("readingchanged", onReadingChanged);
            document.getElementById("registerReadingChanged").innerText = "Unregister ReadingChanged";
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
        var readingTimeStamp = document.getElementById("readingTimeStamp");
        var duration = 0;

        // Event can still be in queue after unload is called
        // so check if elements are still loaded.

        // update html page
        if (readingTimeStamp) {
            readingTimeStamp.innerText = reading.timestamp;

            switch (reading.stepKind) {
                case Windows.Devices.Sensors.PedometerStepKind.unknown:
                    newStepsTaken = reading.cumulativeSteps - unknownStepsCount;
                    unknownStepsCount = reading.cumulativeSteps;
                    document.getElementById("unknownCount").innerHTML = unknownStepsCount;
                    document.getElementById("unknownDuration").innerHTML = reading.cumulativeStepsDuration;
                    break;
                case Windows.Devices.Sensors.PedometerStepKind.walking:
                    newStepsTaken = reading.cumulativeSteps - walkingStepsCount;
                    walkingStepsCount = reading.cumulativeSteps;
                    document.getElementById("walkingCount").innerHTML = walkingStepsCount;
                    document.getElementById("walkingDuration").innerHTML = reading.cumulativeStepsDuration;
                    break;
                case Windows.Devices.Sensors.PedometerStepKind.running:
                    newStepsTaken = reading.cumulativeSteps - runningStepsCount;
                    runningStepsCount = reading.cumulativeSteps;
                    document.getElementById("runningCount").innerHTML = runningStepsCount;
                    document.getElementById("runningDuration").innerHTML = reading.cumulativeStepsDuration;
                    break;
                default:
                    WinJS.log && WinJS.log("Invalid Step Kind", "sample", "error");
                    break;
            }
            totalStepsCount += newStepsTaken;
            document.getElementById("totalStepsCount").innerText = totalStepsCount.toString();
            WinJS.log && WinJS.log("Getting readings", "sample", "status");
        }
    }
})();
