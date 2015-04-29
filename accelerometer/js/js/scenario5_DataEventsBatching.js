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
    var reportInterval = 0;
    var reportLatency = 0;
    var accelerometer;

    var page = WinJS.UI.Pages.define("/html/scenario5_DataEventsBatching.html", {
        ready: function (element, options) {
            document.getElementById("scenario5Open").addEventListener("click", enableReadingChangedScenario, false);
            document.getElementById("scenario5Revoke").addEventListener("click", disableReadingChangedScenario, false);
            document.getElementById("scenario5Open").disabled = false;
            document.getElementById("scenario5Revoke").disabled = true;

            accelerometer = Windows.Devices.Sensors.Accelerometer.getDefault();
            if (accelerometer) {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                var minimumReportInterval = accelerometer.minimumReportInterval;
                reportInterval = minimumReportInterval > 16 ? minimumReportInterval : 16;
                var maxLatency = accelerometer.MaxBatchSize * reportLatency;
                reportLatency = maxLatency < 10 ? maxLatency : 10;
            } else {
                WinJS.log && WinJS.log("No accelerometer found", "sample", "error");
            }
        },
        unload: function () {
            if (document.getElementById("scenario5Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                accelerometer.removeEventListener("readingchanged", onDataChanged);

                // Return the report interval to its default to release resources while the sensor is not in use
                accelerometer.reportInterval = 0;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario5Open").disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
                accelerometer.addEventListener("readingchanged", onDataChanged);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                accelerometer.removeEventListener("readingchanged", onDataChanged);
            }
        }
    }

    function onDataChanged(e) {
        var reading = e.reading;

        // event can still be in queue after unload is called
        // so check if elements are still loaded

        if (document.getElementById("eventOutputX")) {
            document.getElementById("eventOutputX").innerHTML = reading.accelerationX.toFixed(2);
        }
        if (document.getElementById("eventOutputY")) {
            document.getElementById("eventOutputY").innerHTML = reading.accelerationY.toFixed(2);
        }
        if (document.getElementById("eventOutputZ")) {
            document.getElementById("eventOutputZ").innerHTML = reading.accelerationZ.toFixed(2);
        }
    }

    function enableReadingChangedScenario() {
        if (accelerometer) {
            // Set the reportInterval to enable the sensor events
            accelerometer.reportInterval = reportInterval;
            // Set the reportLatency 
            accelerometer.reportLatency = reportLatency;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            accelerometer.addEventListener("readingchanged", onDataChanged);
            document.getElementById("scenario5Open").disabled = true;
            document.getElementById("scenario5Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No accelerometer found", "sample", "error");
        }
    }

    function disableReadingChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        accelerometer.removeEventListener("readingchanged", onDataChanged);
        document.getElementById("scenario5Open").disabled = false;
        document.getElementById("scenario5Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        accelerometer.reportInterval = 0;
    }
})();
