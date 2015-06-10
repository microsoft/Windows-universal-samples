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
    var page = WinJS.UI.Pages.define("/html/Scenario2_CloseDrawer.html", {
        ready: function (element, options) {

            document.getElementById("InitDrawerButton").addEventListener("click", initDrawer, false);
            document.getElementById("DrawerWaitButton").addEventListener("click", waitForDrawerClose, false);

            resetScenarioState();
        },

        unload: function () {

            if (_claimedDrawer !== null) {
                _claimedDrawer.removeEventListener("statusupdated", statusUpdated);
                _claimedDrawer.close();
                _claimedDrawer = null;
            }

            _drawer = null;
        }
    });

    var _drawer = null;
    var _claimedDrawer = null;

    var statusNames = [
        "Online",
        "Off",
        "Offline",
        "OffOrOffline",
        "Extended"
    ];

    //
    // Claims and enables the default cash drawer.
    //
    function initDrawer() {

        WinJS.log("Creating cash drawer object.", "sample", "status");

        Windows.Devices.PointOfService.CashDrawer.getDefaultAsync().then(function (drawer) {
            if (drawer == null) {
                WinJS.log("Cash drawer not found. Please connect a cash drawer.", "sample", "error");
                return;
            }

            _drawer = drawer;

            // Attempt to claim the cash drawer
            _drawer.claimDrawerAsync().done(function (claimedDrawer) {

                if (claimedDrawer == null) {
                    WinJS.log("Failed to claim cash drawer.", "sample", "error");
                    return;
                }

                _claimedDrawer = claimedDrawer;

                // Attempt to enable the claimed cash drawer
                _claimedDrawer.enableAsync().then(function (success) {
                    if (!success) {
                        WinJS.log("Failed to enable cash drawer.", "sample", "error");
                    }
                    else {
                        document.getElementById("InitDrawerButton").disabled = true;
                        document.getElementById("DrawerWaitButton").disabled = false;

                        showDrawerStatus(_drawer.status);
                        WinJS.log("Successfully enabled cash drawer. Device ID: " + _claimedDrawer.deviceId, "sample", "status");
                    }
                });
            });
        });
    }


    //
    // Configures an alarm and waits for the drawer to be closed.
    //
    function waitForDrawerClose() {

        if (_claimedDrawer == null) {
            WinJS.log("Drawer must be initialized.", "sample", "error");
            return;
        }

        WinJS.log("Waiting for drawer to close.", "sample", "status");

        var alarm = _claimedDrawer.closeAlarm;

        // Specifying time periods in milliseconds
        alarm.alarmTimeout = new Date(30000);
        alarm.beepDelay = new Date(3000);
        alarm.beepDuration = new Date(1000);
        alarm.beepFrequency = 700;

        _claimedDrawer.addEventListener("alarmtimeoutexpired", onAlarmExpired);

        try {
            alarm.startAsync().then(function (success) {
                if (success)
                    WinJS.log("Successfully waited for drawer close.", "sample", "status");
                else
                    WinJS.log("Failed to wait for drawer close.", "sample", "error");
            });
        }
        catch (err) {
            WinJS.log("error caught" + err.message);
        }

    }


    //
    // Event callback for device status updates.
    //
    function statusUpdated(args) {
        showDrawerStatus(args.status);
        WinJS.log("Status updated event: " + args.status.statusKind.ToString(), "sample", "status");
    }


    //
    // Event callback for alarm expiration.
    //
    function onAlarmExpired(args) {
        WinJS.log("Alarm expired.  Still waiting for drawer to be closed.", "sample", "status");
    }


    //
    // Write the drawer status to the screen.
    //
    function showDrawerStatus(status) {
        document.getElementById("drawerStatusText").textContent = statusNames[status.statusKind];
    }


    //
    // Resets the scenario to its initial state.
    //
    function resetScenarioState() {

        WinJS.log("Click the init drawer button to begin.", "sample", "status");

        document.getElementById("InitDrawerButton").disabled = false;
        document.getElementById("DrawerWaitButton").disabled = true;

        document.getElementById("drawerStatusText").textContent = "Not connected";
    }

})();

