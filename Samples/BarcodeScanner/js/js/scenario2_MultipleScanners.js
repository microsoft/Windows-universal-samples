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

(function() {
    "use strict";

    // Types
    var BarcodeSymbologies = Windows.Devices.PointOfService.BarcodeSymbologies;

    // Page elements
    var scenarioStartScanningInstance1;
    var scenaioEndScanningInstance1;
    var scenarioStartScanningInstance2;
    var scenaioEndScanningInstance2;

    var retain1;
    var scanDataType1;
    var scanData1;
    var scanDataFormatted1;
    var retain2;
    var scanDataType2;
    var scanData2;
    var scanDataFormatted2;

    var page = WinJS.UI.Pages.define("/html/scenario2_MultipleScanners.html", {

        // On ready - set button states and add event listeners to the buttons.
        ready: function(element, options) {

            WinJS.log("Click a start scanning button to begin.", "sample", "status");

            retain1 = document.getElementById("retain1");
            scanDataType1 = document.getElementById("scanDataType1");
            scanData1 = document.getElementById("scanData1");
            scanDataFormatted1 = document.getElementById("scanDataFormatted1");
            retain2 = document.getElementById("retain2");
            scanDataType2 = document.getElementById("scanDataType2");
            scanData2 = document.getElementById("scanData2");
            scanDataFormatted2 = document.getElementById("scanDataFormatted2");

            scenarioStartScanningInstance1 = document.getElementById("scenarioStartScanningInstance1");
            scenarioStartScanningInstance1.addEventListener("click", buttonStartScanningInstance1, false);
            scenarioStartScanningInstance1.disabled = false;

            scenarioEndScanningInstance1 = document.getElementById("scenarioEndScanningInstance1");
            scenarioEndScanningInstance1.addEventListener("click", buttonEndScanningInstance1, false);
            scenarioEndScanningInstance1.disabled = true;

            scenarioStartScanningInstance2 = document.getElementById("scenarioStartScanningInstance2");
            scenarioStartScanningInstance2.addEventListener("click", buttonStartScanningInstance2, false);
            scenarioStartScanningInstance2.disabled = false;

            scenarioEndScanningInstance2 = document.getElementById("scenarioEndScanningInstance2");
            scenarioEndScanningInstance2.addEventListener("click", buttonEndScanningInstance2, false);
            scenarioEndScanningInstance2.disabled = true;
        },

        unload: function() {
            closeScanner1();
            closeScanner2();
        }
    });

    var _activeBarcodeScannerInstance = { instance1: 0, instance2: 1 };
    var _scanner1 = null;
    var _claimedScanner1 = null;
    var _scanner2 = null;
    var _claimedScanner2 = null;

    var _currentInstance;

    // Create scanner instance 1 and claim and enable it.
    function buttonStartScanningInstance1() {

        // Create the barcode scanner. 
        createScanner(_activeBarcodeScannerInstance.instance1).then(function(success) {
            if (success) {
                // Claim the scanner and enable
                return claimAndEnableScanner(_activeBarcodeScannerInstance.instance1);
            }
        }).done(function(success) {
            if (success) {
                resetUI();
                scenarioStartScanningInstance1.disabled = true;
                scenarioEndScanningInstance1.disabled = false;
                scenarioStartScanningInstance2.disabled = false;
                scenarioEndScanningInstance2.disabled = true;
                _currentInstance = _activeBarcodeScannerInstance.instance1;
            }
        });
    }

    // Create scanner instance 2 and claim and enable it.
    function buttonStartScanningInstance2() {

        // Create the barcode scanner. 
        createScanner(_activeBarcodeScannerInstance.instance2).then(function(success) {
            if (success) {
                // Claim the scanner and enable
                return claimAndEnableScanner(_activeBarcodeScannerInstance.instance2);
            }
        }).done(function(success) {
            if (success) {
                resetUI();
                scenarioStartScanningInstance2.disabled = true;
                scenarioEndScanningInstance2.disabled = false;
                scenarioStartScanningInstance1.disabled = false;
                scenarioEndScanningInstance1.disabled = true;
                _currentInstance = _activeBarcodeScannerInstance.instance2;
            }
        });
    }

    // Reset UI after ending instance 1
    function buttonEndScanningInstance1() {
        closeScanner1();
        resetUI();
        scenarioStartScanningInstance1.disabled = false;
        scenarioEndScanningInstance1.disabled = true;
        WinJS.log("Click a start scanning button to begin.", "sample", "status");
    }

    // Reset UI after ending instance 2
    function buttonEndScanningInstance2() {
        closeScanner2();
        resetUI();
        scenarioStartScanningInstance2.disabled = false;
        scenarioEndScanningInstance2.disabled = true;
        WinJS.log("Click a start scanning button to begin.", "sample", "status");
    }

    // Resets the textContent in the scanned data values.
    function resetUI() {
        switch (_currentInstance) {
            case _activeBarcodeScannerInstance.instance1:
                scanDataType1.textContent = "No Data";
                scanData1.textContent = "No Data";
                scanDataFormatted1.textContent = "No Data";
                break;

            case _activeBarcodeScannerInstance.instance2:
                scanDataType2.textContent = "No Data";
                scanData2.textContent = "No Data";
                scanDataFormatted2.textContent = "No Data";
                break;
        }
    }

    // This method is called upon when a claim request is made on instance 1. If a retain request was placed on the device it rejects the new claim.
    function onReleasedeviceRequested1(args) {
        // Check if the instance wants to retain the device
        if (retain1.checked) {
            // Retain the device
            _claimedScanner1.retainDevice();
        }
    }

    // This method is called upon when a claim request is made on instance 2. If a retain request was placed on the device it rejects the new claim.
    function onReleasedeviceRequested2(args) {
        // Check if the instance wants to retain the device
        if (retain2.checked) {
            // Retain the device
            _claimedScanner2.retainDevice();
        }
    }

    // Claims and enables the scanner for the given scanner instance and returns true if success.
    function claimAndEnableScanner(instance) {
        switch (instance) {
            case _activeBarcodeScannerInstance.instance1:
                if (_scanner1) {
                    // Claim the scanner
                    return _scanner1.claimScannerAsync().then(function(claimedScanner) {
                        if (claimedScanner) {
                            _claimedScanner1 = claimedScanner;
                            claimedScanner.isDecodeDataEnabled = true;

                            // Add the event handlers
                            claimedScanner.addEventListener("datareceived", onDataReceived);
                            claimedScanner.addEventListener("releasedevicerequested", onReleasedeviceRequested1);

                            // Enable the Scanner
                            return claimedScanner.enableAsync().then(function() {
                                WinJS.log("Instance 1 ready to scan. Device ID: " + _claimedScanner1.deviceId, "sample", "status");
                                return true;
                            });
                        } else {
                            WinJS.log("Instance 1 claim barcode scanner failed.", "sample", "error");
                            return false;
                        }
                    });
                }
                break;
            case _activeBarcodeScannerInstance.instance2:
                if (_scanner2) {
                    // Claim the scanner
                    return _scanner2.claimScannerAsync().then(function(claimedScanner) {
                        if (claimedScanner) {
                            _claimedScanner2 = claimedScanner;
                            claimedScanner.isDecodeDataEnabled = true;

                            // Add the event handlers
                            claimedScanner.addEventListener("datareceived", onDataReceived);
                            claimedScanner.addEventListener("releasedevicerequested", onReleasedeviceRequested2);

                            // Enable the Scanner
                            return claimedScanner.enableAsync().then(function() {
                                WinJS.log("Instance 2 ready to scan. Device ID: " + _claimedScanner2.deviceId, "sample", "status");
                                return true;
                            });
                        } else {
                            WinJS.log("Instance 2 claim barcode scanner failed.", "sample", "error");
                            return false;
                        }
                    });
                }
                break;
        }
    }

    function createScanner(instance) {
        return SdkSample.getFirstBarcodeScannerAsync().done(function (scanner) {
            if (scanner) {
                switch (instance) {
                    case _activeBarcodeScannerInstance.instance1:
                        _scanner1 = scanner;
                        break;
                    case _activeBarcodeScannerInstance.instance2:
                        _scanner2 = scanner;
                        break;
                }
                return true;
            } else {
                WinJS.log("Scanner not found. Please connect a barcode scanner.", "sample", "error");
                return false;
            }
        });
    }

    function closeScanner1() {
        if (_claimedScanner1) {
            // Remove event listeners and close.
            _claimedScanner1.removeEventListener("datareceived", onDataReceived);
            _claimedScanner1.removeEventListener("releasedevicerequested", onReleasedeviceRequested1);
            _claimedScanner1.close();
            _claimedScanner1 = null;
        }

        if (_scanner1) {
            _scanner1.close();
            _scanner1 = null;
        }
    }

    function closeScanner2() {
        if (_claimedScanner2) {
            // Remove event listeners and close.
            _claimedScanner2.removeEventListener("datareceived", onDataReceived);
            _claimedScanner2.removeEventListener("releasedevicerequested", onReleasedeviceRequested2);
            _claimedScanner2.close();
            _claimedScanner2 = null;
        }

        if (_scanner2) {
            _scanner2.close();
            _scanner2 = null;
        }
    }

    // This is an event handler for the claimed scanner Instance when it scans and recieves data
    function onDataReceived(args) {
        // Now populate the UI
        switch (_currentInstance) {
            case _activeBarcodeScannerInstance.instance1:
                scanDataType1.textContent = BarcodeSymbologies.getName(args.report.scanDataType);
                scanDataFormatted1.textContent = getDataLabelString(args.report.scanDataLabel, args.report.scanDataType);
                scanData1.textContent = getDataString(args.report.scanData);
                WinJS.log("Instance 1 received data from the barcode scanner.", "sample", "status");
                break;

            case _activeBarcodeScannerInstance.instance2:
                scanDataType2.textContent = BarcodeSymbologies.getName(args.report.scanDataType);
                scanDataFormatted2.textContent = getDataLabelString(args.report.scanDataLabel, args.report.scanDataType);
                scanData2.textContent = getDataString(args.report.scanData);
                WinJS.log("Instance 2 received data from the barcode scanner.", "sample", "status");
                break;
        }
    }
})();
