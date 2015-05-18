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
    var compass;

    var page = WinJS.UI.Pages.define("/html/scenario1_DataEvents.html", {
        ready: function (element, options) {
            document.getElementById("scenario1Open").addEventListener("click", enableReadingChangedScenario, false);
            document.getElementById("scenario1Revoke").addEventListener("click", disableReadingChangedScenario, false);
            document.getElementById("scenario1Open").disabled = false;
            document.getElementById("scenario1Revoke").disabled = true;

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
            if (document.getElementById("scenario1Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                compass.removeEventListener("readingchanged", onDataChanged);

                // Return the report interval to its default to release resources while the sensor is not in use
                compass.reportInterval = 0;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario1Open").disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
                compass.addEventListener("readingchanged", onDataChanged);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                compass.removeEventListener("readingchanged", onDataChanged);
            }
        }
    }

    function onDataChanged(e) {
        var reading = e.reading;

        // event can still be in queue after unload is called
        // so check if elements are still loaded

        if (document.getElementById("eventOutputMagneticNorth")) {
            document.getElementById("eventOutputMagneticNorth").innerHTML = reading.headingMagneticNorth.toFixed(2);
        }
        if (reading.headingTrueNorth) {
            if (document.getElementById("eventOutputTrueNorth")) {
                document.getElementById("eventOutputTrueNorth").innerHTML = reading.headingTrueNorth.toFixed(2);
            }
        }
        if (document.getElementById("eventOutputHeadingAccuracy")) {
            switch (e.reading.headingAccuracy) {
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

    function enableReadingChangedScenario() {
        if (compass) {
            // Set the reportInterval to enable the sensor events
            compass.reportInterval = reportInterval;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            compass.addEventListener("readingchanged", onDataChanged);
            document.getElementById("scenario1Open").disabled = true;
            document.getElementById("scenario1Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No compass found", "sample", "error");
        }
    }

    function disableReadingChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        compass.removeEventListener("readingchanged", onDataChanged);
        document.getElementById("scenario1Open").disabled = false;
        document.getElementById("scenario1Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        compass.reportInterval = 0;
    }
})();
