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
    var compass;

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            document.getElementById("scenario2Open").addEventListener("click", enableGetReadingScenario, false);
            document.getElementById("scenario2Revoke").addEventListener("click", disableGetReadingScenario, false);
            document.getElementById("scenario2Open").disabled = false;
            document.getElementById("scenario2Revoke").disabled = true;

            compass = Windows.Devices.Sensors.Compass.getDefault();
            if (compass) {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                var minimumReportInterval = compass.minimumReportInterval;
                reportInterval = minimumReportInterval > 16 ? minimumReportInterval : 16;
            } else {
                WinJS.log && WinJS.log("No compass found", "sample", "error");
            }
        },
        unload: function () {
            if (document.getElementById("scenario2Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                clearInterval(intervalId);

                // Return the report interval to its default to release resources while the sensor is not in use
                compass.reportInterval = 0;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario2Open").disabled) {
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
        var reading = compass.getCurrentReading();
        if (reading) {
            document.getElementById("readingOutputMagneticNorth").innerHTML = reading.headingMagneticNorth.toFixed(2);
            if (reading.headingTrueNorth) {
                document.getElementById("readingOutputTrueNorth").innerHTML = reading.headingTrueNorth.toFixed(2);
            } else {
                document.getElementById("readingOutputTrueNorth").innerHTML = "no data";
            }
            switch (reading.headingAccuracy) {
                case Windows.Devices.Sensors.MagnetometerAccuracy.unknown:
                    document.getElementById("eventOutputHeadingAccuracy").innerHTML = "unknown";
                    break;
                case Windows.Devices.Sensors.MagnetometerAccuracy.unreliable:
                    document.getElementById("eventOutputHeadingAccuracy").innerHTML = "unreliable";
                    break;
                case Windows.Devices.Sensors.MagnetometerAccuracy.approximate:
                    document.getElementById("eventOutputHeadingAccuracy").innerHTML = "approximate";
                    break;
                case Windows.Devices.Sensors.MagnetometerAccuracy.high:
                    document.getElementById("eventOutputHeadingAccuracy").innerHTML = "high";
                    break;
                default:
                    document.getElementById("eventOutputHeadingAccuracy").innerHTML = "no data";
                    break;
            }
        }
    }

    function enableGetReadingScenario() {
        if (compass) {
            // Set the report interval to enable the sensor for polling
            compass.reportInterval = reportInterval;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            intervalId = setInterval(getCurrentReading, reportInterval);
            document.getElementById("scenario2Open").disabled = true;
            document.getElementById("scenario2Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No compass found", "sample", "error");
        }
    }

    function disableGetReadingScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        clearInterval(intervalId);
        document.getElementById("scenario2Open").disabled = false;
        document.getElementById("scenario2Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        compass.reportInterval = 0;
    }
})();
