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

    var Lights = Windows.Devices.Lights;
    var oLamp = null;

    var page = WinJS.UI.Pages.define("/html/Scenario3_AvailabilityChanged.html", {
        ready: function (element, options) {
            document.getElementById("registerBtn").addEventListener("click", registerBtn_click, false);
            document.getElementById("unregisterBtn").addEventListener("click", unregisterBtn_click, false);
            document.getElementById("lampCheckBox").addEventListener("click", lampCheckBox_click, false);
            InitializeLampAsync().done();
        },

        unload: function () {
            if (oLamp === null) {
                return;
            }

            lampCheckBox.isDisabled = true;
            oLamp.removeEventListener("availabilitychanged", lampAvailabilityChanged);
            oLamp.isEnabled = false;
            oLamp.close();
        }
    });

    /// <summary>
    /// Initialize the lamp acquiring the default instance
    /// </summary>
    /// <returns>promise</returns>
    function InitializeLampAsync() {
        return Lights.Lamp.getDefaultAsync()
        .then(function (lamp) {
            if (lamp === null) {
                logError("Error: No lamp device was found");
                return;
            }

            // Set our global lamp for later use
            oLamp = lamp;

            logStatus("Default lamp instance acquired, Device Id: " + oLamp.deviceId);
        });
    }

    /// <summary>
    /// Event Handler for Lamp Availability Changed. When it fires, we want to update to
    /// Lamp toggle UI to show that lamp is available or not
    /// </summary>
    function lampAvailabilityChanged(event) {
        // Update the Lamp Toggle UI to indicate lamp has been consumed by another application
        if (lampCheckBox.checked) {
            lampCheckBox.checked = event.isAvailable;
            oLamp.isEnabled = event.isAvailable;
        }

        logStatus("Lamp Availability Changed Notification has fired");
    }

    /// <summary>
    /// Event Handler for the register button click event
    /// </summary>
    function registerBtn_click() {
        if (oLamp === null) {
            logError("Error: No lamp device was found");
            return;
        }

        oLamp.addEventListener("availabilitychanged", lampAvailabilityChanged);
        logStatus("Registered for Lamp Availability Changed Notification");
    }

    /// <summary>
    /// Event handler for the Unregister Availability Event
    /// </summary>
    function unregisterBtn_click() {
        if (oLamp == null) {
            logError("Error: No lamp device was found");
            return;
        }

        oLamp.removeEventListener("availabilitychanged", lampAvailabilityChanged);
        logStatus("Unregistered for Lamp Availability Changed Notification");
    }

    /// <summary>
    /// Event handler for Lamp check box
    /// Toggling the box will turn Lamp on or off
    /// </summary>
    function lampCheckBox_click() {
        if (oLamp == null) {
            logError("Error: No lamp device was found");
            return;
        }

        if (lampCheckBox != null) {
            if (lampCheckBox.checked) {
                oLamp.isEnabled = true;
                logStatus("Lamp is Enabled");
            }
            else {
                oLamp.isEnabled = false;
                logStatus("Lamp is Disabled");
            }
        }
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
