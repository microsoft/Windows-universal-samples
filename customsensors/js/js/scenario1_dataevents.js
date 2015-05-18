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

    var page = WinJS.UI.Pages.define("/html/scenario1_DataEvents.html", {
        ready: function (element, options) {
            document.getElementById("scenario1Open").addEventListener("click", enableReadingChangedScenario, false);
            document.getElementById("scenario1Revoke").addEventListener("click", disableReadingChangedScenario, false);
            document.getElementById("scenario1Open").disabled = false;
            document.getElementById("scenario1Revoke").disabled = true;

            var customSensorSelector = Windows.Devices.Sensors.Custom.CustomSensor.getDeviceSelector(customSensorDeviceVendorDefinedTypeID);

            watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(customSensorSelector, null);
            watcher.addEventListener("added", onCustomSensorAdded);
            watcher.start();
        },
        unload: function () {
            if (document.getElementById("scenario1Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                customSensor.removeEventListener("readingchanged", onDataChanged);

                // Return the report interval to its default to release resources while the sensor is not in use
                customSensor.reportInterval = 0;
            }
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
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                // In the case below, we defined a 200ms report interval as being suitable for the purpose of this app.
                var minimumReportInterval = customSensor.minimumReportInterval;
                reportInterval = minimumReportInterval > 200 ? minimumReportInterval : 200;
            } else {
                WinJS.log && WinJS.log("No custom sensor found", "sample", "error");
            }
        },
        function (e) {
            WinJS.log && WinJS.log("The user may have denied access to the custom sensor. Error: " + e.message, "sample", "error");
        });
    }
     

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario1Open").disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
                customSensor.addEventListener("readingchanged", onDataChanged);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                customSensor.removeEventListener("readingchanged", onDataChanged);
            }
        }
    }

    function onDataChanged(e) {
        var reading = e.reading;

        // The event can still be in queue after unload is called
        // so check if elements are still loaded.

        if (document.getElementById("eventOutputCO2Level") && reading.properties.hasKey(co2LevelKey)) {
            document.getElementById("eventOutputCO2Level").innerText = reading.properties.lookup(co2LevelKey).toFixed(1);
        }
    }

    function enableReadingChangedScenario() {
        if (customSensor) {
            // Set the reportInterval to enable the sensor events
            customSensor.reportInterval = reportInterval;

            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            customSensor.addEventListener("readingchanged", onDataChanged);
            document.getElementById("scenario1Open").disabled = true;
            document.getElementById("scenario1Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No custom sensor found", "sample", "error");
        }
    }

    function disableReadingChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        customSensor.removeEventListener("readingchanged", onDataChanged);
        document.getElementById("scenario1Open").disabled = false;
        document.getElementById("scenario1Revoke").disabled = true;

        // Return the report interval to its default to release resources while the sensor is not in use
        customSensor.reportInterval = 0;
    }
})();
