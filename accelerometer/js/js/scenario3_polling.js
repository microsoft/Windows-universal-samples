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
    var intervalId = 0;
    var accelerometer;

    var page = WinJS.UI.Pages.define("/html/scenario3_Polling.html", {
        ready: function (element, options) {
            document.getElementById("scenario3Open").addEventListener("click", enableGetReadingScenario, false);
            document.getElementById("scenario3Revoke").addEventListener("click", disableGetReadingScenario, false);
            document.getElementById("scenario3Open").disabled = false;
            document.getElementById("scenario3Revoke").disabled = true;

            accelerometer = Windows.Devices.Sensors.Accelerometer.getDefault();
            if (accelerometer) {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                var minimumReportInterval = accelerometer.minimumReportInterval;
                reportInterval = minimumReportInterval > 16 ? minimumReportInterval : 16;
            } else {
                WinJS.log && WinJS.log("No accelerometer found", "sample", "error");
            }
        },
        unload: function () {
            if (document.getElementById("scenario3Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                clearInterval(intervalId);

                // Return the report interval to its default to release resources while the sensor is not in use
                accelerometer.reportInterval = 0;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario3Open").disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
                intervalId = setInterval(getCurrentReading, reportInterval);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                clearInterval(intervalId);
            }
        }
    }

    function getCurrentReading() {
        var reading = accelerometer.getCurrentReading();
        if (reading) {
            document.getElementById("readingOutputX").innerHTML = reading.accelerationX.toFixed(2);
            document.getElementById("readingOutputY").innerHTML = reading.accelerationY.toFixed(2);
            document.getElementById("readingOutputZ").innerHTML = reading.accelerationZ.toFixed(2);
        }
    }

    function enableGetReadingScenario() {
        if (accelerometer) {
            // Set the report interval to enable the sensor for polling
            accelerometer.reportInterval = reportInterval;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            intervalId = setInterval(getCurrentReading, reportInterval);
            document.getElementById("scenario3Open").disabled = true;
            document.getElementById("scenario3Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No accelerometer found", "sample", "error");
        }
    }

    function disableGetReadingScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        clearInterval(intervalId);
        document.getElementById("scenario3Open").disabled = false;
        document.getElementById("scenario3Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        accelerometer.reportInterval = 0;
    }
})();
