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
    var desiredReportIntervalMs = 0;
    var barometer;

    var page = WinJS.UI.Pages.define("/html/scenario1_DataEvents.html", {
        ready: function (element, options) {
            document.getElementById("scenario1Open").addEventListener("click", enableReadingChangedScenario, false);
            document.getElementById("scenario1Revoke").addEventListener("click", disableReadingChangedScenario, false);
            document.getElementById("scenario1Open").disabled = false;
            document.getElementById("scenario1Revoke").disabled = true;

            barometer = Windows.Devices.Sensors.Barometer.getDefault();
            if (barometer) {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                var minReportIntervalMs = barometer.minReportIntervalMs;
                desiredReportIntervalMs = minReportIntervalMs > 1000 ? minReportIntervalMs : 1000;
            } else {
                WinJS.log && WinJS.log("No barometer found", "sample", "error");
            }
        },
        unload: function () {
            if (document.getElementById("scenario1Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                barometer.removeEventListener("readingchanged", onDataChanged);

                // Return the report interval to its default to release resources while the sensor is not in use
                barometer.desiredReportIntervalMs = 0;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario1Open").disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired desiredReportIntervalMs (it is restored for us upon app resume)
                barometer.addEventListener("readingchanged", onDataChanged);
            } else {
                // Disable sensor input. No need to restore the default desiredReportIntervalMs (resources will be released upon app suspension)
                barometer.removeEventListener("readingchanged", onDataChanged);
            }
        }
    }

    function onDataChanged(e) {
        var reading = e.reading;

        // The event can still be in queue after unload is called,
        // so check if element is still loaded.

        if (document.getElementById("eventOutputPressure")) {
            document.getElementById("eventOutputPressure").innerHTML = e.reading.stationPressureInHectopascals.toFixed(2);
        }
    }

    function enableReadingChangedScenario() {
        if (barometer) {
            // Set the desiredReportIntervalMs to enable the sensor events
            barometer.desiredReportIntervalMs = desiredReportIntervalMs;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            barometer.addEventListener("readingchanged", onDataChanged);
            document.getElementById("scenario1Open").disabled = true;
            document.getElementById("scenario1Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No barometer found", "sample", "error");
        }
    }

    function disableReadingChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        barometer.removeEventListener("readingchanged", onDataChanged);
        document.getElementById("scenario1Open").disabled = false;
        document.getElementById("scenario1Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        barometer.desiredReportIntervalMs = 0;
    }
})();
