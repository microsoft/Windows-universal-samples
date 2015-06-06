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
    var proximitySensor;
    var watcher;

    var page = WinJS.UI.Pages.define("/html/scenario1_DataEvents.html", {
        ready: function (element, options) {
            document.getElementById("scenario1Open").addEventListener("click", enableReadingChangedScenario, false);
            document.getElementById("scenario1Revoke").addEventListener("click", disableReadingChangedScenario, false);
            document.getElementById("scenario1Open").disabled = false;
            document.getElementById("scenario1Revoke").disabled = true;

            var proximitySensorDeviceSelector = Windows.Devices.Sensors.ProximitySensor.getDeviceSelector();

            watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(proximitySensorDeviceSelector, null);
            watcher.addEventListener("added", onProximitySensorAdded);
            watcher.start();
        },
        unload: function () {
            if (document.getElementById("scenario1Open").disabled) {
                proximitySensor.removeEventListener("readingchanged", onReadingChanged);
            }
            watcher && watcher.removeEventListener("added", onProximitySensorAdded);
        }
    });

    /// <summary>
    /// Invoked when the device watcher finds a matching proximity sensor device
    /// </summary>
    /// <param name="device">device information for the proximity sensor that was found</param>
    function onProximitySensorAdded(device) {
        if (!proximitySensor) {
            var foundSensor = Windows.Devices.Sensors.ProximitySensor.fromId(device.id);
            if (foundSensor) {
                if (foundSensor.maxDistanceInMillimeters) {
                    // Check if this is the sensor that matches our ranges.

                    // TODO: Customize these values to your application's needs.
                    // Here, we are looking for a sensor that can detect close objects
                    // up to 3cm away, so we check the upper bound of the detection range.
                    var distanceInMillimetersLValue = 30; // 3 cm
                    var distanceInMillimetersRValue = 50; // 5 cm

                    if (foundSensor.maxDistanceInMillimeters >= distanceInMillimetersLValue &&
                        foundSensor.maxDistanceInMillimeters <= distanceInMillimetersRValue) {
                        WinJS.log && WinJS.log("Found a proximity sensor that meets the detection range", "sample", "status");
                    } else {
                        foundSensor = null;
                    }
                } else {
                    WinJS.log && WinJS.log("Proximity sensor does not report detection ranges, using it anyway", "sample", "status");
                }

                if (foundSensor) {
                    proximitySensor = foundSensor;
                }
            } else {
                WinJS.log && WinJS.log("Could not get a proximity sensor from the device id", "sample", "error");
            }
        }
    }

    function onReadingChanged(e) {
        var reading = e.reading;

        // The event can still be in queue after unload is called,
        // so check if element is still loaded.

        if (document.getElementById("eventOutputDetectionState")) {
            document.getElementById("eventOutputDetectionState").innerText = e.reading.isDetected ? "Detected" : "Not detected";
        }
        if (document.getElementById("eventOutputTimestamp")) {
            document.getElementById("eventOutputTimestamp").innerText = e.reading.timestamp;
        }

        // Show the detection distance, if available
        if (document.getElementById("eventOutputDistance") && e.reading.distanceInMillimeters) {
            document.getElementById("eventOutputDistance").innerText = e.reading.distanceInMillimeters;
        }
    }

    function enableReadingChangedScenario() {
        if (proximitySensor) {

            proximitySensor.addEventListener("readingchanged", onReadingChanged);
            document.getElementById("scenario1Open").disabled = true;
            document.getElementById("scenario1Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No proximity sensor found", "sample", "error");
        }
    }

    function disableReadingChangedScenario() {
        proximitySensor.removeEventListener("readingchanged", onReadingChanged);
        document.getElementById("scenario1Open").disabled = false;
        document.getElementById("scenario1Revoke").disabled = true;
    }
})();
