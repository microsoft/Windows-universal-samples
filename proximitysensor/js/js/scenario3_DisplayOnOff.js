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
    var displayController;
    var watcher;

    var page = WinJS.UI.Pages.define("/html/scenario3_DisplayOnOff.html", {
        ready: function (element, options) {
            document.getElementById("scenario3Open").addEventListener("click", enableMonitoringScenario, false);
            document.getElementById("scenario3Revoke").addEventListener("click", disableMonitoringScenario, false);
            document.getElementById("scenario3Open").disabled = false;
            document.getElementById("scenario3Revoke").disabled = true;

            var proximitySensorDeviceSelector = Windows.Devices.Sensors.ProximitySensor.getDeviceSelector();

            watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(proximitySensorDeviceSelector, null);
            watcher.addEventListener("added", onProximitySensorAdded);
            watcher.start();
        },
        unload: function () {
            if (document.getElementById("scenario3Open").disabled) {
                if (displayController) {
                    displayController.close();
                    displayController = null;
                }
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
                proximitySensor = foundSensor;
            } else {
                WinJS.log && WinJS.log("Could not get a proximity sensor from the device id", "sample", "error");
            }
        }
    }

    function enableMonitoringScenario() {
        if (proximitySensor) {
            // Acquires the display on/off controller for this proximity sensor.
            // This tells the system to use the sensor's IsDetected state to
            // turn the screen on or off.  If the display does not support this
            // feature, this code will do nothing.            
            displayController = proximitySensor.createDisplayOnOffController();

            document.getElementById("scenario3Open").disabled = true;
            document.getElementById("scenario3Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No proximity sensor found", "sample", "error");
        }
    }

    function disableMonitoringScenario() {
        // Releases the display on/off controller that is previously acquired.
        // This tells the system to stop using the sensor's IsDetected state to
        // turn the screen on or off.  If the display does not support this
        // feature, this code will do nothing.        
        if (displayController) {
            displayController.close();
            displayController = null;
        }
        document.getElementById("scenario3Open").disabled = false;
        document.getElementById("scenario3Revoke").disabled = true;
    }
})();
