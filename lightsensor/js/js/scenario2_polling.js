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
    var lightSensor;

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            document.getElementById("scenario2Open").addEventListener("click", enableGetReadingScenario, false);
            document.getElementById("scenario2Revoke").addEventListener("click", disableGetReadingScenario, false);
            document.getElementById("scenario2Open").disabled = false;
            document.getElementById("scenario2Revoke").disabled = true;

            lightSensor = Windows.Devices.Sensors.LightSensor.getDefault();
            if (lightSensor) {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                var minimumReportInterval = lightSensor.minimumReportInterval;
                reportInterval = minimumReportInterval > 16 ? minimumReportInterval : 16;
            } else {
                WinJS.log && WinJS.log("No light sensor found", "sample", "error");
            }
        },
        unload: function () {
            if (document.getElementById("scenario2Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                clearInterval(intervalId);

                // Return the report interval to its default to release resources while the sensor is not in use
                lightSensor.reportInterval = 0;
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
        var reading = lightSensor.getCurrentReading();
        if (reading) {
            document.getElementById("readingOutputIlluminance").innerHTML = reading.illuminanceInLux.toFixed(2);
        }
    }

    function enableGetReadingScenario() {
        if (lightSensor) {
            // Set the report interval to enable the sensor for polling
            lightSensor.reportInterval = reportInterval;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            intervalId = setInterval(getCurrentReading, reportInterval);
            document.getElementById("scenario2Open").disabled = true;
            document.getElementById("scenario2Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No light sensor found", "sample", "error");
        }
    }

    function disableGetReadingScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        clearInterval(intervalId);
        document.getElementById("scenario2Open").disabled = false;
        document.getElementById("scenario2Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        lightSensor.reportInterval = 0;
    }
})();
