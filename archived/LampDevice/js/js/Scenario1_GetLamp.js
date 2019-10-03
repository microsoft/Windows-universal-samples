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
    var page = WinJS.UI.Pages.define("/html/Scenario1_GetLamp.html", {
        ready: function (element, options) {
            document.getElementById("initLampBtn").addEventListener("click", initLampBtn_click, false);
            document.getElementById("getDefaultAsyncBtn").addEventListener("click", getDefaultAsyncBtn_click, false);
        }
    });

    var Lights = Windows.Devices.Lights;
    
    /// <summary>
    /// Acquires Lamp instance by getting class selection string 
    /// for Lamp devices, then selecting the first lamp device on
    /// the back of the device
    /// </summary>
    function initLampBtn_click() {        
        var DeviceInformation = Windows.Devices.Enumeration.DeviceInformation;
        logStatus("Getting class selection string for lamp devices...");

        // Returns class selection string for Lamp devices
        var selectorStr = Lights.Lamp.getDeviceSelector();
        logStatus("Lamp class selection string:\n" + selectorStr);

        logStatus("Finding all lamp devices...");

        // Finds all devices based on lamp class selector string
        DeviceInformation.findAllAsync(selectorStr, null)
        .then(function (devices) {
            logStatus("Number of lamp devices found: " + devices.length);

            // Select the first lamp device found on the back of the device
            logStatus("Selecting first lamp device found on back of the device");
            var deviceInfo = null;
            devices.forEach(function (device) {
                if (device.enclosureLocation != null && device.enclosureLocation.panel === Windows.Devices.Enumeration.Panel.back) {
                    deviceInfo = device;
                }
            });
            if (deviceInfo === null) {
                logError("Error: No lamp device on the back panel was found");
                return;
            }

            logStatus("Acquiring Lamp instance from Id:\n" + deviceInfo.id);
            return Lights.Lamp.fromIdAsync(deviceInfo.id)
            .then(function (lamp) {
                logStatus("Lamp instance acquired, Device Id:\n" + lamp.deviceId);

                // Here we must Dispose of the lamp object once we are no longer 
                // using it to release any native resources
                logStatus("Disposing of lamp instance");
                lamp.close();
                logStatus("Disposed");
            });
        }).done();
    }

    /// <summary>
    /// Gets the Default Lamp instance
    /// </summary>
    function getDefaultAsyncBtn_click() {
        logStatus("Initializing lamp");

        Lights.Lamp.getDefaultAsync()
        .then(function (lamp) {
            if (lamp === null) {
                logError("Error: No Lamp device found");
                return;
            }

            logStatus("Default lamp instance acquired, Device Id: " + lamp.deviceId);
            lamp.close();
        }).done();
    }

    function logError(msg) {
        WinJS.log && WinJS.log(msg, "", "error");
    }

    function logStatus(msg) {
        msg += "\r\n";
        outputBox.textContent += msg;
        WinJS.log && WinJS.log(msg, "", "status");
    }
})();
