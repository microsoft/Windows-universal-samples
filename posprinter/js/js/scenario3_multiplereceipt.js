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
    var page = WinJS.UI.Pages.define("/html/Scenario3_MultipleReceipt.html", {

        ready: function (element, options) {

            WinJS.log("Create the printers to begin.", "sample", "status");

            document.getElementById("findClaimEnableButton").addEventListener("click", findClaimEnable, false);

            document.getElementById("claim1").addEventListener("click", claim1_Click, false);
            document.getElementById("printLine1").addEventListener("click", printLine1_Click, false);
            document.getElementById("release1").addEventListener("click", release1_Click, false);

            document.getElementById("claim2").addEventListener("click", claim2_Click, false);
            document.getElementById("printLine2").addEventListener("click", printLine2_Click, false);
            document.getElementById("release2").addEventListener("click", release2_Click, false);

            document.getElementById("scenarioEndButton").addEventListener("click", endScenario, false);

            setInitialUI();
        },

        unload: function () {
            if (_claimedPrinter1 !== null) {
                _claimedPrinter1.close();
                _claimedPrinter1 = null;
            }
            _printerInstance1 = null;
            if (_claimedPrinter2 !== null) {
                _claimedPrinter2.close();
                _claimedPrinter2 = null;
            }
            _printerInstance2 = null;
        }
    });

    var _printerInstance1 = null;
    var _printerInstance2 = null;
    var _claimedPrinter1 = null;
    var _claimedPrinter2 = null;

    //
    //PosPrinter GetDeviceSelector gets the string format used to search for pos printer. This is then used to find any pos printers.
    //The method then takes the first printer id found and tries to create two instances for that printer.
    //
    function findClaimEnable() {

        if (_printerInstance1 == null || _printerInstance2 == null) {

            WinJS.log("Finding printer...", "sample", "status");
            var devicestr = Windows.Devices.PointOfService.PosPrinter.getDeviceSelector();

            Windows.Devices.Enumeration.DeviceInformation.findAllAsync(devicestr, null).then(function (deviceCollection) {

                if (deviceCollection.length == 0) {
                    WinJS.log("No printers found during enumeration.", "sample", "error");
                }

                var id = deviceCollection[0].id;
                Windows.Devices.PointOfService.PosPrinter.fromIdAsync(id).then(function (printer) {

                    if (printer != null) {
                        _printerInstance1 = printer;

                        if (_printerInstance1.capabilities.receipt.isPrinterPresent) {

                            //Now create second instance of printer as well with the same id.
                            Windows.Devices.PointOfService.PosPrinter.fromIdAsync(id).then(function (printer) {

                                if (printer != null) {
                                    _printerInstance2 = printer;
                                    WinJS.log("Created two receipt printer instances. Device ID: " + printer.deviceId, "sample", "status");
                                    setUnclaimedUI();
                                }
                            });
                        }
                    } else {
                        WinJS.log("FromIdAsync was unsuccessful.", "sample", "error");
                    }
                },
                function error(e) {
                    WinJS.log("FromIdAsync was unsuccessful: " + e.message, "sample", "error");
                });//end of fromIdAsync
            }, function error(e) {
                WinJS.log("Printer device enumeration unsuccessful: " + e.message, "sample", "error");
            });
        }
    }

    //
    //Actual claim method task that claims and enables printer instance 1 asynchronously. It then adds the releasedevicerequested event handler as well to the claimed printer.
    //
    function claim1_Click() {

        if (_printerInstance1 != null) {

            _printerInstance1.claimPrinterAsync().done(function (claimedPrinter) {

                if (claimedPrinter !== null) {

                    _claimedPrinter1 = claimedPrinter;

                    //Add event listener for releasedevicerequest
                    _claimedPrinter1.addEventListener("releasedevicerequested", onReleasedeviceRequested1);

                    //Enable printer
                    _claimedPrinter1.enableAsync().done(function (success) {
                        if (success) {
                            WinJS.log("Claimed instance 1.", "sample", "status");
                            setInstance1ClaimedUI();
                        }
                        else {
                            WinJS.log("Could not enable printer.", "sample", "error");
                        }
                        return;
                    },
                    function error(e) {
                        WinJS.log("Error enabling claimed printer instance 1: " + e.message, "sample", "error");
                    });
                    return;
                }
                else {
                    WinJS.log("Could not claim the printer instance 1.", "sample", "error");
                }
            });
        }
        else {
            WinJS.log("Could not claim. Printer instance 1 is null.", "sample", "error");
        }
    }

    //
    //Actual claim method task that claims and enables printer instance 2 asynchronously. It then adds the releasedevicerequested event handler as well to the claimed printer.
    //
    function claim2_Click() {

        if (_printerInstance2 != null) {

            _printerInstance2.claimPrinterAsync().done(function (claimedPrinter) {

                if (claimedPrinter !== null) {

                    _claimedPrinter2 = claimedPrinter;

                    //Add event listener for releasedevicerequest
                    _claimedPrinter2.addEventListener("releasedevicerequested", onReleasedeviceRequested2);

                    //Enable printer
                    _claimedPrinter2.enableAsync().done(function (success) {
                        if (success) {
                            WinJS.log("Claimed instance 2.", "sample", "status");
                            setInstance2ClaimedUI();
                        }
                        else {
                            WinJS.log("Could not enable printer.", "sample", "error");
                        }
                        return;
                    },
                    function error(e) {
                        WinJS.log("Error enabling claimed printer instance 2: " + e.message, "sample", "error");
                    });
                    return;
                }
                else {
                    WinJS.log("Could not claim the printer instance 2.", "sample", "error");
                }
            });
        }
        else {
            WinJS.log("Could not claim. Printer instance 2 is null.", "sample", "error");
        }
    }

    function release1_Click() {

        if (_claimedPrinter1 != null) {
            _claimedPrinter1.removeEventListener("releasedevicerequested", onReleasedeviceRequested1);
            _claimedPrinter1.close();
            _claimedPrinter1 = null;

            WinJS.log("Instance 1 released claim.", "sample", "status");

            setUnclaimedUI();
        }
        else {
            WinJS.log("Instance 1 not claimed to release.", "sample", "status");
        }
    }

    function release2_Click() {

        if (_claimedPrinter2 != null) {
            _claimedPrinter2.removeEventListener("releasedevicerequested", onReleasedeviceRequested2);
            _claimedPrinter2.close();
            _claimedPrinter2 = null;

            WinJS.log("Instance 2 released claim.", "sample", "status");

            setUnclaimedUI();
        }
        else {
            WinJS.log("Instance 2 not claimed to release.", "sample", "status");
        }
    }

    function printLine1_Click() {

        if (_claimedPrinter1 == null) {
            WinJS.log("Claimed printer instance 1 is null. Cannot print.", "sample", "error");
            return;
        }

        var job = _claimedPrinter1.receipt.createJob();
        job.printLine(document.getElementById("txtPrintLine1").value);

        job.executeAsync().done(function () {
            WinJS.log("Printed line.", "sample", "status");
        },
        function error(e) {
            WinJS.log("Was not able to print line: " + e.message, "sample", "error");
        });
    }

    function printLine2_Click() {

        if (_claimedPrinter2 == null) {
            WinJS.log("Claimed printer instance 2 is null. Cannot print.", "sample", "error");
            return;
        }

        var job = _claimedPrinter2.receipt.createJob();
        job.printLine(document.getElementById("txtPrintLine2").value);

        job.executeAsync().done(function () {
            WinJS.log("Printed line.", "sample", "status");
        },
        function error(e) {
            WinJS.log("Was not able to print line: " + e.message, "sample", "error");
        });
    }

    //
    //If user checks the check box to not release instance 1, it then calls RetainDeviceAsync method to retain the claim on that printer else it releases the claim.
    //
    function onReleasedeviceRequested1(args) {
        if (document.getElementById("chkInstance1").checked && _claimedPrinter1) {
            _claimedPrinter1.retainDeviceAsync();
        } else if (_claimedPrinter1) {
            _claimedPrinter1.close();
            _claimedPrinter1 = null;
        }
    }

    //
    //If user checks the check box to not release instance 2, it then calls RetainDeviceAsync method to retain the claim on that printer else it releases the claim.
    //
    function onReleasedeviceRequested2(args) {
        if (document.getElementById("chkInstance2").checked && _claimedPrinter2) {
            _claimedPrinter2.retainDeviceAsync();
        } else if (_claimedPrinter2) {
            _claimedPrinter2.close();
            _claimedPrinter2 = null;
        }
    }

    function endScenario() {

        setInitialUI();

        //Remove releasedevicerequested handler and dispose claimed printer object.
        if (_claimedPrinter1 != null) {
            _claimedPrinter1.removeEventListener("releasedevicerequested", onReleasedeviceRequested1);
            _claimedPrinter1.close();
            _claimedPrinter1 = null;
        }

        if (_claimedPrinter2 != null) {
            _claimedPrinter2.removeEventListener("releasedevicerequested", onReleasedeviceRequested2);
            _claimedPrinter2.close();
            _claimedPrinter2 = null;
        }
        _printerInstance1 = null;
        _printerInstance2 = null;

        WinJS.log("Create the printers to begin.", "sample", "status");
    }

    //
    //Update the UI elements for instance 1 claim
    //
    function setInstance1ClaimedUI() {

        document.getElementById("claim1").disabled = true;
        document.getElementById("printLine1").disabled = false;
        document.getElementById("release1").disabled = false;
        
        document.getElementById("claim2").disabled = false;
        document.getElementById("printLine2").disabled = true;
        document.getElementById("release2").disabled = true
    }

    //
    //Update the UI elements for instance 2 claim
    //
    function setInstance2ClaimedUI() {

        document.getElementById("claim1").disabled = false;
        document.getElementById("printLine1").disabled = true;
        document.getElementById("release1").disabled = true;

        document.getElementById("claim2").disabled = true;
        document.getElementById("printLine2").disabled = false;
        document.getElementById("release2").disabled = false
    }

    //
    //Update the UI elements for no claimed instances
    //
    function setUnclaimedUI() {

        document.getElementById("claim1").disabled = false;
        document.getElementById("printLine1").disabled = true;
        document.getElementById("release1").disabled = true;

        document.getElementById("claim2").disabled = false;
        document.getElementById("printLine2").disabled = true;
        document.getElementById("release2").disabled = true;
    }

    //
    //Update the UI elements to initial state with no printer instances initialized
    //
    function setInitialUI() {

        setUnclaimedUI();

        document.getElementById("claim1").disabled = true;
        document.getElementById("claim2").disabled = true;
    }

})();
