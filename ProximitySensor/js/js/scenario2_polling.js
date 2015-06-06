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

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            document.getElementById("scenario2Open").addEventListener("click", getReadingScenario, false);
            document.getElementById("scenario2Open").disabled = false;

            var proximitySensorDeviceSelector = Windows.Devices.Sensors.ProximitySensor.getDeviceSelector();

            watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(proximitySensorDeviceSelector, null);
            watcher.addEventListener("added", onProximitySensorAdded);
            watcher.start();
        },
        unload: function () {
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
                // Optionally check the ProximitySensor.maxDistanceInCentimeters/minDistanceInCentimeters
                // properties here. Refer to Scenario 1 for details.
                proximitySensor = foundSensor;
            } else {
                WinJS.log && WinJS.log("Could not get a proximity sensor from the device id", "sample", "error");
            }
        }
    }

    function getReadingScenario() {
        if (proximitySensor) {
            var reading = proximitySensor.getCurrentReading();
            if (reading) {
                if (document.getElementById("dataOutputDetectionState")) {
                    document.getElementById("dataOutputDetectionState").innerText = reading.isDetected ? "Detected" : "Not detected";
                }
                if (document.getElementById("dataOutputTimestamp")) {
                    document.getElementById("dataOutputTimestamp").innerText = reading.timestamp;
                }
                // Show the detection distance, if available
                if (document.getElementById("dataOutputDistance") && reading.distanceInMillimeters) {
                    document.getElementById("dataOutputDistance").innerText = reading.distanceInMillimeters;
                }
            }
        } else {
            WinJS.log && WinJS.log("No proximity sensor found", "sample", "error");
        }
    }
})();
