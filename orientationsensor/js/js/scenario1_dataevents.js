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
    var sensor;

    var page = WinJS.UI.Pages.define("/html/scenario1_DataEvents.html", {
        ready: function (element, options) {
            document.getElementById("scenario1Open").addEventListener("click", enableReadingChangedScenario, false);
            document.getElementById("scenario1Revoke").addEventListener("click", disableReadingChangedScenario, false);
            document.getElementById("scenario1Open").disabled = false;
            document.getElementById("scenario1Revoke").disabled = true;

            sensor = Windows.Devices.Sensors.OrientationSensor.getDefault();
            if (sensor) {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                var minimumReportInterval = sensor.minimumReportInterval;
                reportInterval = minimumReportInterval > 16 ? minimumReportInterval : 16;
            } else {
                WinJS.log && WinJS.log("No orientation sensor found", "sample", "error");
            }
        },
        unload: function () {
            if (document.getElementById("scenario1Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                sensor.removeEventListener("readingchanged", onDataChanged);

                // Return the report interval to its default to release resources while the sensor is not in use
                sensor.reportInterval = 0;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario1Open").disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
                sensor.addEventListener("readingchanged", onDataChanged);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                sensor.removeEventListener("readingchanged", onDataChanged);
            }
        }
    }

    function onDataChanged(e) {
        var reading = e.reading;

        // event can still be in queue after unload is called
        // so check if elements are still loaded

        // Quaternion values
        if (document.getElementById("eventOutputQuaternion")) {
            var quaternion = reading.quaternion;    // get a reference to the object to avoid re-creating it for each access
            document.getElementById("eventOutputQuaternion").innerHTML =
                   "W: " + quaternion.w.toFixed(6)
                + " X: " + quaternion.x.toFixed(6)
                + " Y: " + quaternion.y.toFixed(6)
                + " Z: " + quaternion.z.toFixed(6);
        }

        // Rotation Matrix values
        if (document.getElementById("eventOutputRotationMatrix")) {
            var rotationMatrix = reading.rotationMatrix;
            document.getElementById("eventOutputRotationMatrix").innerHTML =
                   "M11: " + rotationMatrix.m11.toFixed(6)
                + " M12: " + rotationMatrix.m12.toFixed(6)
                + " M13: " + rotationMatrix.m13.toFixed(6)
                + " M21: " + rotationMatrix.m21.toFixed(6)
                + " M22: " + rotationMatrix.m22.toFixed(6)
                + " M23: " + rotationMatrix.m23.toFixed(6)
                + " M31: " + rotationMatrix.m31.toFixed(6)
                + " M32: " + rotationMatrix.m32.toFixed(6)
                + " M33: " + rotationMatrix.m33.toFixed(6);
        }

        // Yaw accuracy
        if (document.getElementById("eventOutputYawAccuracy")) {
            switch (e.reading.yawAccuracy) {
                case Windows.Devices.Sensors.MagnetometerAccuracy.unknown:
                    document.getElementById("eventOutputYawAccuracy").innerHTML = "unknown";
                    break;
                case Windows.Devices.Sensors.MagnetometerAccuracy.unreliable:
                    document.getElementById("eventOutputYawAccuracy").innerHTML = "unreliable";
                    break;
                case Windows.Devices.Sensors.MagnetometerAccuracy.approximate:
                    document.getElementById("eventOutputYawAccuracy").innerHTML = "approximate";
                    break;
                case Windows.Devices.Sensors.MagnetometerAccuracy.high:
                    document.getElementById("eventOutputYawAccuracy").innerHTML = "high";
                    break;
                default:
                    document.getElementById("eventOutputYawAccuracy").innerHTML = "no data";
                    break;
            }
        }
    }

    function enableReadingChangedScenario() {
        if (sensor) {
            // Set the reportInterval to enable the sensor events
            sensor.reportInterval = reportInterval;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            sensor.addEventListener("readingchanged", onDataChanged);
            document.getElementById("scenario1Open").disabled = true;
            document.getElementById("scenario1Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No orientation sensor found", "sample", "error");
        }
    }

    function disableReadingChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        sensor.removeEventListener("readingchanged", onDataChanged);
        document.getElementById("scenario1Open").disabled = false;
        document.getElementById("scenario1Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        sensor.reportInterval = 0;
    }
})();
