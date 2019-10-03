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
    var page = WinJS.UI.Pages.define("/html/Scenario2_EnableSettings.html", {
        ready: function (element, options) {
            document.getElementById("brightnessBtn").addEventListener("click", brightnessBtn_click, false);
            document.getElementById("colorLampBtn").addEventListener("click", colorLampBtn_click, false);
        }
    });

    var Lights = Windows.Devices.Lights;

    /// <summary>
    /// Event Handler for Brightness Button Click
    /// </summary>
    function brightnessBtn_click() {
        logStatus("Initializing lamp");

        Lights.Lamp.getDefaultAsync()
        .then(function (lamp)
        {
            if (lamp === null) {
                logError("Error: No lamp device was found");
                return;
            }

            logStatus("Default lamp instance acquired, Device Id: " + lamp.deviceId);
            logStatus("Lamp Default Brightness: " + lamp.brightnessLevel);

            // Set the Brightness Level
            logStatus("Adjusting Brightness");
            lamp.brightnessLevel = 0.5;
            logStatus("Lamp Brightness Adjustment: Brightness: " + lamp.brightnessLevel);

            // Turn Lamp on
            logStatus("Turning Lamp on");
            lamp.isEnabled = true;

            // Turn Lamp off
            logStatus("Turning Lamp off");
            lamp.isEnabled = false;
            lamp.close();
        }).done();
    }

    /// <summary>
    /// Event handler for Color Lamp Button. Queries the lamp device for color adjustment
    /// support and if device supports being color settable, it sets lamp color to blue. 
    /// Click Event
    /// </summary>
    function colorLampBtn_click() {
        logStatus("Initializing lamp");

        Lights.Lamp.getDefaultAsync()
        .then(function (lamp)
        {
            if (lamp === null) {
                logError("Error: No lamp device was found");
                return;
            }

            // With Lamp instance required, query for the default settings of the Lamp
            logStatus("Default lamp instance acquired");

            // If this lamp device is not Color Settable exit color lamp adjustment
            if (lamp.isColorSettable === false) {
                logError("Selected Lamp device doesn't support Color lamp adjustment");
                return;
            }
            
            // Change Lamp Color
            logStatus("Adjusting Color");
            lamp.color = Windows.UI.Colors.blue;
            logStatus("Lamp color has been changed");

            // Turn Lamp on
            logStatus("Turning Lamp on");
            lamp.isEnabled = true;

            // Turn Lamp off
            logStatus("Turning Lamp off");
            lamp.isEnabled = false;
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
