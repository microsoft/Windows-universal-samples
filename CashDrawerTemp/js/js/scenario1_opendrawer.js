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
    var page = WinJS.UI.Pages.define("/html/Scenario1_OpenDrawer.html", {
        ready: function (element, options) {

            document.getElementById("InitDrawerButton").addEventListener("click", initDrawer, false);
            document.getElementById("OpenDrawerButton").addEventListener("click", openDrawer, false);

            resetScenarioState();
        },

        unload: function () {

            if (_claimedDrawer !== null) {
                _claimedDrawer.close();
                _claimedDrawer = null;
            }

            _drawer = null;
        }
    });

    var _drawer = null;
    var _claimedDrawer = null;


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
                        document.getElementById("OpenDrawerButton").disabled = false;

                        WinJS.log("Successfully enabled cash drawer. Device ID: " + _claimedDrawer.deviceId, "sample", "status");
                    }
                });
            });
        });
    }


    //
    // Attempts to open the cash drawer.
    //
    function openDrawer() {

        if (_claimedDrawer == null || !_claimedDrawer.isEnabled) {
            WinJS.log("Need to initialize and enable the drawer before opening it.", "sample", "error");
            return;
        }

        _claimedDrawer.openDrawerAsync().then(function (success) {
            if (success)
                WinJS.log("Cash drawer open succeeded.", "sample", "status");
            else
                WinJS.log("Cash drawer open failed.", "sample", "status");
        });
    }


    //
    // Resets the scenario to its initial state.
    //
    function resetScenarioState() {

        WinJS.log("Click the init drawer button to begin.", "sample", "status");

        document.getElementById("InitDrawerButton").disabled = false;
        document.getElementById("OpenDrawerButton").disabled = true;
    }

})();
