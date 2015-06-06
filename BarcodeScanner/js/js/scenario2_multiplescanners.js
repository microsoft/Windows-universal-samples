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
    var page = WinJS.UI.Pages.define("/html/scenario2_MultipleScanners.html", {

        // On ready - set button states and add event listeners to the buttons.
        ready: function (element, options) {

            WinJS.log("Click a start scanning button to begin.", "sample", "status");

            document.getElementById("scenarioStartScanningInstance1").addEventListener("click", buttonStartScanningInstance1, false);
            document.getElementById("scenarioEndScanningInstance1").addEventListener("click", buttonEndScanningInstance1, false);
            document.getElementById("scenarioStartScanningInstance1").disabled = false;
            document.getElementById("scenarioEndScanningInstance1").disabled = true;

            document.getElementById("scenarioStartScanningInstance2").addEventListener("click", buttonStartScanningInstance2, false);
            document.getElementById("scenarioEndScanningInstance2").addEventListener("click", buttonEndScanningInstance2, false);
            document.getElementById("scenarioStartScanningInstance2").disabled = false;
            document.getElementById("scenarioEndScanningInstance2").disabled = true;
        },

        // On unload - clean up if claimedscanner objects are not null.
        unload: function () {
            if (_claimedScanner1 !== null) {

                // Remove event listeners that were added before and close.
                _claimedScanner1.removeEventListener("datareceived", onDataReceived);
                _claimedScanner1.removeEventListener("releasedevicerequested", onReleasedeviceRequested1);
                _claimedScanner1.close();
                _claimedScanner1 = null;
            }

            if (_claimedScanner2 !== null) {

                // Remove event listeners that were added before and close.
                _claimedScanner2.removeEventListener("datareceived", onDataReceived);
                _claimedScanner2.removeEventListener("releasedevicerequested", onReleasedeviceRequested2);
                _claimedScanner2.close();
                _claimedScanner2 = null;
            }
            _scanner1 = null;
            _scanner2 = null;
        }
    });

    var _activeBarcodeScannerInstance = { instance1: 0, instance2: 1 };
    var _scanner1 = null;
    var _claimedScanner1 = null;
    var _scanner2 = null;
    var _claimedScanner2 = null;

    var _currentInstance;
    var _retain1 = true;
    var _retain2 = true;

    // Create scanner instance 1 and claim and enable it.
    function buttonStartScanningInstance1() {

        // Create the barcode scanner. 
        createScanner(_activeBarcodeScannerInstance.instance1).then(function (success) {
            if (success === true) {

                // Claim the scanner and enable
                claimAndEnableScanner(_activeBarcodeScannerInstance.instance1).then(function (bAsyncCallStatus) {
                    if (bAsyncCallStatus === true) {
                        resetUI();
                        document.getElementById("scenarioStartScanningInstance1").disabled = true;
                        document.getElementById("scenarioEndScanningInstance1").disabled = false;
                        document.getElementById("scenarioStartScanningInstance2").disabled = false;
                        document.getElementById("scenarioEndScanningInstance2").disabled = true;
                        _currentInstance = _activeBarcodeScannerInstance.instance1;
                    }
                });
            }
        });
    }
    
    // Create scanner instance 2 and claim and enable it.
    function buttonStartScanningInstance2() {

        // Create the barcode scanner. 
        createScanner(_activeBarcodeScannerInstance.instance2).then(function (success) {
            if (success === true) {

                // Claim the scanner and enable
                claimAndEnableScanner(_activeBarcodeScannerInstance.instance2).then(function (bAsyncCallStatus) {
                    if (bAsyncCallStatus === true) {
                        resetUI();
                        document.getElementById("scenarioStartScanningInstance2").disabled = true;
                        document.getElementById("scenarioEndScanningInstance2").disabled = false;
                        document.getElementById("scenarioStartScanningInstance1").disabled = false;
                        document.getElementById("scenarioEndScanningInstance1").disabled = true;
                        _currentInstance = _activeBarcodeScannerInstance.instance2;
                    }
                });
            }
        });
    }

    // Reset UI after ending instance 1
    function buttonEndScanningInstance1() {

        disableScannerAsync1().done(function (success) {

            if (success === true) {
  
                resetUI();
                document.getElementById("scenarioStartScanningInstance1").disabled = false;
                document.getElementById("scenarioEndScanningInstance1").disabled = true;
            }

            WinJS.log("Click a start scanning button to begin.", "sample", "status");
        });
    }

    // Reset UI after ending instance 2
    function buttonEndScanningInstance2() {

        disableScannerAsync2().done(function (success) {

            if (success === true) {
                resetUI();
                document.getElementById("scenarioStartScanningInstance2").disabled = false;
                document.getElementById("scenarioEndScanningInstance2").disabled = true;
            }

            WinJS.log("Click a start scanning button to begin.", "sample", "status");
        });
    }

    // Resets the textContent in the scanned data values.
    function resetUI() {

        switch (_currentInstance) {

            case _activeBarcodeScannerInstance.instance1:

                document.getElementById("scanDataType1").textContent = "No Data";
                document.getElementById("scanData1").textContent = "No Data";
                document.getElementById("scanDataFormatted1").textContent = "No Data";
                break;

            case _activeBarcodeScannerInstance.instance2:

                document.getElementById("scanDataType2").textContent = "No Data";
                document.getElementById("scanData2").textContent = "No Data";
                document.getElementById("scanDataFormatted2").textContent = "No Data";
                break;
        }
    }

    // This method is called upon when a claim request is made on instance 1. If a retain request was placed on the device it rejects the new claim.
    function onReleasedeviceRequested1(args) {

        var retain = document.getElementById("retain1");

        // Check if the instance wants to retain the device
        if (retain.checked === true) {
            try {
                // Retain the device
                _claimedScanner1.retainDevice();
            }
            catch (error) {
                WinJS.log("Retain instance 1 failed: " + error.message, "sample", "error");
            }
        }
    }

    // This method is called upon when a claim request is made on instance 2. If a retain request was placed on the device it rejects the new claim.
    function onReleasedeviceRequested2(args) {

        var retain = document.getElementById("retain2");

        // Check if the instance wants to retain the device
        if (retain.checked === true) {
            try {
                // Retain the device
                _claimedScanner2.retainDevice();
            }
            catch (error) {
                WinJS.log("Retain instance 2 failed: " + error.message, "sample", "error");
            }
        }
    }

    // Claims and enables the scanner for the given scanner instance and returns true if success.
    function claimAndEnableScanner(instance) {
        return new WinJS.Promise(function (complete) {
            switch (instance) {
                case _activeBarcodeScannerInstance.instance1:

                    if (_scanner1 !== null) {

                        // Claim the scanner
                        _scanner1.claimScannerAsync().done(function (claimedScanner) {
                            if (claimedScanner !== null) {
                                _claimedScanner1 = claimedScanner;
                                claimedScanner.isDecodeDataEnabled = true;

                                // Add the event handlers
                                claimedScanner.addEventListener("datareceived", onDataReceived);
                                claimedScanner.addEventListener("releasedevicerequested", onReleasedeviceRequested1);

                                // Enable the Scanner
                                claimedScanner.enableAsync().done(function () {
                                    WinJS.log("Instance 1 ready to scan. Device ID: " + _claimedScanner1.deviceId, "sample", "status");
                                    complete(true);
                                }, function error(e) {
                                    WinJS.log("Instance 1 enable barcode scanner failed: " + e.message, "sample", "error");
                                    complete(false);
                                });
                            } else {
                                WinJS.log("Instance 1 claim barcode scanner failed.", "sample", "error");
                                complete(false);
                            }
                        }, function error(e) {
                            WinJS.log("Instance 1 claim barcode scanner failed: " + e.message, "sample", "error");
                            complete(false);
                        });
                    }
                    break;
                case _activeBarcodeScannerInstance.instance2:
                    if (_scanner2 !== null) {

                        // Claim the scanner
                        _scanner2.claimScannerAsync().done(function (claimedScanner) {
                            if (claimedScanner !== null) {
                                _claimedScanner2 = claimedScanner;
                                claimedScanner.isDecodeDataEnabled = true;

                                // Add the event handlers
                                claimedScanner.addEventListener("datareceived", onDataReceived);
                                claimedScanner.addEventListener("releasedevicerequested", onReleasedeviceRequested2);

                                // Enable the Scanner
                                claimedScanner.enableAsync().done(function () {
                                    WinJS.log("Instance 2 ready to scan. Device ID: " + _claimedScanner2.deviceId, "sample", "status");
                                    complete(true);
                                }, function error(e) {
                                    WinJS.log("Instance 2 enable barcode scanner failed: " + e.message, "sample", "error");
                                    complete(false);
                                });
                            } else {
                                WinJS.log("Instance 2 claim barcode scanner failed.", "sample", "error");
                                complete(false);
                            }
                        }, function error(e) {
                            WinJS.log("Instance 2 claim barcode scanner failed: " + e.message, "sample", "error");
                            complete(false);
                        });
                    }
                    break;
            }
        });
    }

    function createScanner(instance) {

        return new WinJS.Promise(function (complete) {

            // Get the handle to the default scanner 
            Windows.Devices.PointOfService.BarcodeScanner.getDefaultAsync().done(function (scanner) {
                if (scanner !== null) {

                    switch (instance) {
                        case _activeBarcodeScannerInstance.instance1:
                            _scanner1 = scanner;
                            break;
                        case _activeBarcodeScannerInstance.instance2:
                            _scanner2 = scanner;
                            break;
                    }
                    complete(true);

                } else {
                    WinJS.log("Scanner not found. Please connect a barcode scanner.", "sample", "error");
                    complete(false);
                }

            }, function error(e) {
                WinJS.log("Barcode scanner GetDefaultAsync unsuccessful: " + e.message, "sample", "error");
                complete(false);
            });

        });
    }



    // Disables the scanner Instance1.
    function disableScannerAsync1() {
        return new WinJS.Promise(function (complete) {

            if (_claimedScanner1 !== null) {

                // Remove event listeners and close.
                _claimedScanner1.removeEventListener("datareceived", onDataReceived);
                _claimedScanner1.removeEventListener("releasedevicerequested", onReleasedeviceRequested1);
                _claimedScanner1.close();
                _claimedScanner1 = null;
            }

            _scanner1 = null;
            resetUI();
            complete(true);
        });
    }

    // Disables the scanner Instance2.
    function disableScannerAsync2() {
        return new WinJS.Promise(function (complete) {
            if (_claimedScanner2 !== null) {

                // Remove event listeners and close.
                _claimedScanner2.removeEventListener("datareceived", onDataReceived);
                _claimedScanner2.removeEventListener("releasedevicerequested", onReleasedeviceRequested2);
                _claimedScanner2.close();
                _claimedScanner2 = null;
            }

            _scanner2 = null;
            resetUI();
            complete(true);
        });
    }

    // This is an event handler for the claimed scanner Instance when it scans and recieves data
    function onDataReceived(args) {
        var tempScanLabel = "";
        var tempScanData = "";

        if (args.report.scanData !== null) {
            tempScanData = getDataString(args.report.scanData);
        }
        if (args.report.scanDataLabel !== null) {
            tempScanLabel = getDataLabelString(args.report.scanDataLabel, args.report.scanDataType);
        }

        // Now populate the UI
        switch (_currentInstance) {
            case _activeBarcodeScannerInstance.instance1:
                if (args.report.scanDataType === 501) {
                    document.getElementById("scanDataType1").textContent = "OEM - " + Windows.Devices.PointOfService.BarcodeSymbologies.getName(args.report.scanDataType);
                } else {
                    document.getElementById("scanDataType1").textContent = Windows.Devices.PointOfService.BarcodeSymbologies.getName(args.report.scanDataType);
                }

                // DataLabel
                document.getElementById("scanDataFormatted1").textContent = tempScanLabel;

                // Data
                document.getElementById("scanData1").textContent = tempScanData;

                WinJS.log("Instance 1 received data from the barcode scanner.", "sample", "status");

                break;

            case _activeBarcodeScannerInstance.instance2:

                // BarcodeSymbologies.ExtendedBase
                if (args.report.scanDataType === 501) {
                    document.getElementById("scanDataType2").textContent = "OEM - " + Windows.Devices.PointOfService.BarcodeSymbologies.getName(args.report.scanDataType);
                } else {
                    document.getElementById("scanDataType2").textContent = Windows.Devices.PointOfService.BarcodeSymbologies.getName(args.report.scanDataType);
                }

                // DataLabel
                document.getElementById("scanDataFormatted2").textContent = tempScanLabel;

                // Data
                document.getElementById("scanData2").textContent = tempScanData;

                WinJS.log("Instance 2 received data from the barcode scanner.", "sample", "status");

                break;
        }
    }

})();
