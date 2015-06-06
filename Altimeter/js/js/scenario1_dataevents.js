//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var desiredReportIntervalMs = 0;
    var altimeter;

    var page = WinJS.UI.Pages.define("/html/scenario1_DataEvents.html", {
        ready: function (element, options) {
            document.getElementById("scenario1Open").addEventListener("click", enableReadingChangedScenario, false);
            document.getElementById("scenario1Revoke").addEventListener("click", disableReadingChangedScenario, false);
            document.getElementById("scenario1Open").disabled = false;
            document.getElementById("scenario1Revoke").disabled = true;

            altimeter = Windows.Devices.Sensors.Altimeter.getDefault();
            if (altimeter) {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                var minReportIntervalMs = altimeter.minReportIntervalMs;
                desiredReportIntervalMs = minReportIntervalMs > 1000 ? minReportIntervalMs : 1000;
            } else {
                WinJS.log && WinJS.log("No altimeter found", "sample", "error");
            }
        },
        unload: function () {
            if (document.getElementById("scenario1Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                altimeter.removeEventListener("readingchanged", onDataChanged);

                // Return the report interval to its default to release resources while the sensor is not in use
                altimeter.desiredReportIntervalMs = 0;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario1Open").disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired desiredReportIntervalMs (it is restored for us upon app resume)
                altimeter.addEventListener("readingchanged", onDataChanged);
            } else {
                // Disable sensor input. No need to restore the default desiredReportIntervalMs (resources will be released upon app suspension)
                altimeter.removeEventListener("readingchanged", onDataChanged);
            }
        }
    }

    function onDataChanged(e) {
        var reading = e.reading;

        // The event can still be in queue after unload is called,
        // so check if element is still loaded.

        if (document.getElementById("eventOutputChangeInMeters")) {
            document.getElementById("eventOutputChangeInMeters").innerText = e.reading.altitudeChangeInMeters.toFixed(2);
        }
    }

    function enableReadingChangedScenario() {
        if (altimeter) {
            // Set the desiredReportIntervalMs to enable the sensor events
            altimeter.desiredReportIntervalMs = desiredReportIntervalMs;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            altimeter.addEventListener("readingchanged", onDataChanged);
            document.getElementById("scenario1Open").disabled = true;
            document.getElementById("scenario1Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No altimeter found", "sample", "error");
        }
    }

    function disableReadingChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        altimeter.removeEventListener("readingchanged", onDataChanged);
        document.getElementById("scenario1Open").disabled = false;
        document.getElementById("scenario1Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        altimeter.desiredReportIntervalMs = 0;
    }
})();
