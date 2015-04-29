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
    var page = WinJS.UI.Pages.define("/html/Scenario3_MultipleDrawers.html", {

        ready: function (element, options) {
            document.getElementById("ClaimButton1").addEventListener("click", claimButton1Click, false);
            document.getElementById("ReleaseButton1").addEventListener("click", releaseButton1Click, false);
            document.getElementById("ClaimButton2").addEventListener("click", claimButton2Click, false);
            document.getElementById("ReleaseButton2").addEventListener("click", releaseButton2Click, false);

            resetScenarioState();
        },

        unload: function () {

            disposeClaimedDrawer(_activeDrawerInstance.instance1);
            disposeClaimedDrawer(_activeDrawerInstance.instance2);

            _drawerInstance1 = null;
            _drawerInstance2 = null;
        }
    });

    var _activeDrawerInstance = { instance1: 0, instance2: 1 };

    var _drawerInstance1 = null;
    var _drawerInstance2 = null;
    var _claimedInstance1 = null;
    var _claimedInstance2 = null;


    //
    // Creates a claimed and enabled cash drawer object for instance 1.
    //
    function claimButton1Click() {

        createDefaultCashDrawerObject(_activeDrawerInstance.instance1).then(function (success) {
            if (success) {
                claimCashDrawerObject(_activeDrawerInstance.instance1).then(function (success) {
                    if (success) {
                        _claimedInstance1.addEventListener("releasedevicerequested", onReleaseDeviceRequested1);
                        setClaimedUI(_activeDrawerInstance.instance1);
                        WinJS.log("Successfully claimed instance 1.", "sample", "status");
                    }
                });
            }
        });
    }


    //
    // Creates a claimed and enabled cash drawer object for instance 2.
    //
    function claimButton2Click() {

        createDefaultCashDrawerObject(_activeDrawerInstance.instance2).then(function (success) {
            if (success) {
                claimCashDrawerObject(_activeDrawerInstance.instance2).then(function (success) {
                    if (success) {
                        _claimedInstance2.addEventListener("releasedevicerequested", onReleaseDeviceRequested2);
                        setClaimedUI(_activeDrawerInstance.instance2);
                        WinJS.log("Successfully claimed instance 2.", "sample", "status");
                    }
                });
            }
        });
    }


    //
    // Releases the claim on instance 1.
    //
    function releaseButton1Click() {

        disposeClaimedDrawer(_activeDrawerInstance.instance1);
        setReleasedUI(_activeDrawerInstance.instance1);

        WinJS.log("Claimed instance 1 was released.", "sample", "status");
    }


    //
    // Releases the claim on instance 2.
    //
    function releaseButton2Click() {

        disposeClaimedDrawer(_activeDrawerInstance.instance2);
        setReleasedUI(_activeDrawerInstance.instance2);

        WinJS.log("Claimed instance 2 was released.", "sample", "status");
    }


    //
    // Creates the default cash drawer.
    //
    function createDefaultCashDrawerObject(instance) {

        WinJS.log("Creating cash drawer object.", "sample", "status");

        return new WinJS.Promise(function (complete) {

            Windows.Devices.PointOfService.CashDrawer.getDefaultAsync().then(function (drawer) {
                if (drawer == null) {
                    WinJS.log("Cash drawer not found. Please connect a cash drawer.", "sample", "error");
                    complete(false);
                }
                else {
                    switch (instance) {
                        case _activeDrawerInstance.instance1:
                            _drawerInstance1 = drawer;
                            break;
                        case _activeDrawerInstance.instance2:
                            _drawerInstance2 = drawer;
                            break;
                    }
                    complete(true);
                }
            });
        });
    }


    //
    // Claims the specified cash drawer instance.
    //
    function claimCashDrawerObject(instance) {

        return new WinJS.Promise(function (complete) {

            switch (instance) {
                case _activeDrawerInstance.instance1:
                    _drawerInstance1.claimDrawerAsync().then(function (claimed) {
                        if (claimed == null) {
                            WinJS.log("Failed to claim instance 1.", "sample", "error");
                            complete(false);
                        }
                        _claimedInstance1 = claimed;
                        complete(true);
                    });
                    break;

                case _activeDrawerInstance.instance2:
                    _drawerInstance2.claimDrawerAsync().then(function (claimed) {
                        if (claimed == null) {
                            WinJS.log("Failed to claim instance 2.", "sample", "error");
                            complete(false);
                        }
                        _claimedInstance2 = claimed;
                        complete(true);
                    });
                    break;
            }
        });
    }


    //
    // Event handler for instance 1 requested.
    //
    function onReleaseDeviceRequested1(args) {

        if (document.getElementById("retainInstance1Chk").checked) {
            _claimedInstance1.retainDeviceAsync();
        }
        else {
            disposeClaimedDrawer(_activeDrawerInstance.instance1);
        }
    }


    //
    // Event handler for instance 2 requested.
    //
    function onReleaseDeviceRequested2(args) {

        if (document.getElementById("retainInstance2Chk").checked) {
            _claimedInstance2.retainDeviceAsync();
        }
        else {
            disposeClaimedDrawer(_activeDrawerInstance.instance2);
        }
    }


    //
    // Dispose of the claimed drawer instance.
    //
    function disposeClaimedDrawer(instance) {

        if (instance === _activeDrawerInstance.instance1) {
            if (_claimedInstance1 !== null) {
                _claimedInstance1.removeEventListener("releasedevicerequested", onReleaseDeviceRequested1);
                _claimedInstance1.close();
                _claimedInstance1 = null;
            }
        }
        else {
            if (_claimedInstance2 !== null) {
                _claimedInstance2.removeEventListener("releasedevicerequested", onReleaseDeviceRequested2);
                _claimedInstance2.close();
                _claimedInstance2 = null;
            }
        }
    }


    //
    // Sets the UI elements to a state corresponding to the specified instance being claimed.
    //
    function setClaimedUI(instance) {

        switch (instance) {
            case _activeDrawerInstance.instance1:
                document.getElementById("ClaimButton1").disabled = true;
                document.getElementById("ReleaseButton1").disabled = false;
                document.getElementById("ClaimButton2").disabled = false;
                document.getElementById("ReleaseButton2").disabled = true;
                break;

            case _activeDrawerInstance.instance2:
                document.getElementById("ClaimButton1").disabled = false;
                document.getElementById("ReleaseButton1").disabled = true;
                document.getElementById("ClaimButton2").disabled = true;
                document.getElementById("ReleaseButton2").disabled = false;
                break;
        }
    }


    //
    // Sets the UI elements to a state corresponding to the specified instance being released.
    //
    function setReleasedUI(instance) {

        switch (instance) {
            case _activeDrawerInstance.instance1:
                document.getElementById("ClaimButton1").disabled = false;
                document.getElementById("ReleaseButton1").disabled = true;
                break;

            case _activeDrawerInstance.instance2:
                document.getElementById("ClaimButton2").disabled = false;
                document.getElementById("ReleaseButton2").disabled = true;
                break;
        }
    }


    //
    // Resets the scenario to its initial state.
    //
    function resetScenarioState() {

        WinJS.log("Click a claim button to begin.", "sample", "status");

        document.getElementById("ClaimButton1").disabled = false;
        document.getElementById("ReleaseButton1").disabled = true;
        document.getElementById("ClaimButton2").disabled = false;
        document.getElementById("ReleaseButton2").disabled = true;

        disposeClaimedDrawer(_activeDrawerInstance.instance1);
        disposeClaimedDrawer(_activeDrawerInstance.instance2);
    }

})();
