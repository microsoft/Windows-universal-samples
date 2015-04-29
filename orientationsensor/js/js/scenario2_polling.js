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
    var sensor;

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            document.getElementById("scenario2Open").addEventListener("click", enableGetReadingScenario, false);
            document.getElementById("scenario2Revoke").addEventListener("click", disableGetReadingScenario, false);
            document.getElementById("scenario2Open").disabled = false;
            document.getElementById("scenario2Revoke").disabled = true;

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
            if (document.getElementById("scenario2Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                clearInterval(intervalId);

                // Return the report interval to its default to release resources while the sensor is not in use
                sensor.reportInterval = 0;
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
        var reading = sensor.getCurrentReading();
        if (reading) {
            // Quaternion values
            var quaternion = reading.quaternion;    // get a reference to the object to avoid re-creating it for each access
            document.getElementById("readingOutputQuaternion").innerHTML =
                   "W: " + quaternion.w.toFixed(6)
                + " X: " + quaternion.x.toFixed(6)
                + " Y: " + quaternion.y.toFixed(6)
                + " Z: " + quaternion.z.toFixed(6);

            // Rotation Matrix values
            var rotationMatrix = reading.rotationMatrix;
            document.getElementById("readingOutputRotationMatrix").innerHTML =
                   "M11: " + rotationMatrix.m11.toFixed(6)
                + " M12: " + rotationMatrix.m12.toFixed(6)
                + " M13: " + rotationMatrix.m13.toFixed(6)
                + " M21: " + rotationMatrix.m21.toFixed(6)
                + " M22: " + rotationMatrix.m22.toFixed(6)
                + " M23: " + rotationMatrix.m23.toFixed(6)
                + " M31: " + rotationMatrix.m31.toFixed(6)
                + " M32: " + rotationMatrix.m32.toFixed(6)
                + " M33: " + rotationMatrix.m33.toFixed(6);

            // Yaw accuracy
            switch (reading.yawAccuracy) {
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

    function enableGetReadingScenario() {
        if (sensor) {
            // Set the report interval to enable the sensor for polling
            sensor.reportInterval = reportInterval;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            intervalId = setInterval(getCurrentReading, reportInterval);
            document.getElementById("scenario2Open").disabled = true;
            document.getElementById("scenario2Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No orientation sensor found", "sample", "error");
        }
    }

    function disableGetReadingScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        clearInterval(intervalId);
        document.getElementById("scenario2Open").disabled = false;
        document.getElementById("scenario2Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        sensor.reportInterval = 0;
    }
})();
