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
    var customSensor;
    var watcher;

    // The following ID is defined by vendors and is unique to a custom sensor type. Each custom sensor driver should define one unique ID.
    // 
    // The ID below is defined in the custom sensor driver sample available in the SDK. It identifies the custom sensor CO2 emulation sample driver.
    var customSensorDeviceVendorDefinedTypeID = "{4025a865-638c-43aa-a688-98580961eeae}";

    // A property key is defined by vendors for each data field property a custom sensor driver exposes. Property keys are defined
    // per custom sensor driver and is unique to each custom sensor type.
    // 
    // The following property key represents the CO2 level as defined in the custom sensor CO2 emulation driver sample available in the WDK.
    // In this example only one key is defined, but other drivers may define more than one key by rev'ing up the property key index.
    var co2LevelKey = "{74879888-a3cc-45c6-9ea9-058838256433} 1";

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            document.getElementById("getCO2Level").addEventListener("click", getCO2Level, false);
            document.getElementById("getCO2Level").disabled = false;

            var customSensorSelector = Windows.Devices.Sensors.Custom.CustomSensor.getDeviceSelector(customSensorDeviceVendorDefinedTypeID);

            watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(customSensorSelector, null);
            watcher.addEventListener("added", onCustomSensorAdded);
            watcher.start();
        },
        unload: function () {
            watcher && watcher.removeEventListener("added", onCustomSensorAdded);
        }
    });

    /// <summary>
    /// Invoked when the device watcher finds a matching custom sensor device 
    /// </summary>
    /// <param name="watcher">device watcher</param>
    /// <param name="customSensorDevice">device information for the custom sensor that was found</param>
    function onCustomSensorAdded(customSensorDevice) {
        Windows.Devices.Sensors.Custom.CustomSensor.fromIdAsync(customSensorDevice.id).done(function (newCustomSensor) {
            if (newCustomSensor) {
                customSensor = newCustomSensor;
            } else {
                WinJS.log && WinJS.log("No custom sensor found", "sample", "error");
            }
        },
        function (e) {
            WinJS.log && WinJS.log("The user may have denied access to the custom sensor. Error: " + e.message, "sample", "error");
        });
    }


    function getCO2Level() {
        if (customSensor) {
            var reading = customSensor.getCurrentReading();
            if (reading && reading.properties.hasKey(co2LevelKey)) {
                document.getElementById("eventOutputCO2Level").innerText = reading.properties.lookup(co2LevelKey).toFixed(1);
            }
        } else {
            WinJS.log && WinJS.log("No custom sensor found", "sample", "error");
        }
    }
})();
