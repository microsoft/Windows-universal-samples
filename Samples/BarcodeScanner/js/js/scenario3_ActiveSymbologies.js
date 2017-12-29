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

    // Types
    var BarcodeSymbologies = Windows.Devices.PointOfService.BarcodeSymbologies;

    // Page elements
    var scenarioStartScanButton;
    var scenarioEndScanButton;
    var setActiveSymbologiesButton;
    var scenarioOutputScanDataType;
    var scenarioOutputScanData;
    var scenarioOutputScanDataFormatted;

    // WinJS controls
    var symbologyListViewControl = null;

    var page = WinJS.UI.Pages.define("/html/scenario3_ActiveSymbologies.html", {

        // On ready - set button states and add event listeners to the buttons.
        ready: function (element, options) {

            WinJS.log("Click the start scanning button to begin.", "sample", "status");

            scenarioOutputScanDataType = document.getElementById("scenarioOutputScanDataType");
            scenarioOutputScanData = document.getElementById("scenarioOutputScanData");
            scenarioOutputScanDataFormatted = document.getElementById("scenarioOutputScanDataFormatted");

            scenarioStartScanButton = document.getElementById("scenarioStartScanButton");
            scenarioStartScanButton.addEventListener("click", startReceivingData, false);
            scenarioStartScanButton.disabled = false;

            scenarioEndScanButton = document.getElementById("scenarioEndScanButton");
            scenarioEndScanButton.addEventListener("click", endReceivingData, false);
            scenarioEndScanButton.disabled = true;

            setActiveSymbologiesButton = document.getElementById("setActiveSymbologiesButton");
            setActiveSymbologiesButton.addEventListener("click", setActiveSymbologies, false);
            setActiveSymbologiesButton.disabled = true;

            symbologyListViewControl = document.getElementById("symbologyListView").winControl;
        },

        unload: function () {
            closeScanners();
        }
    });

    var _scanner = null;
    var _claimedScanner = null;

    // Creates the default barcode scanner and claims it. Once, claimed, it adds event listeners for onDataReceived and releaseDeviceRequested events.
    // On error, it logs the error message.
    function startReceivingData() {
        scenarioStartScanButton.disabled = true;

        // Create the barcode scanner.
        WinJS.log("Creating barcode scanner object.", "sample", "status");

        SdkSample.getFirstBarcodeScannerAsync().then(function (scanner) {
            _scanner = scanner;
            if (scanner) {
                // After successful creation, fill the list with supported symbologies.
                return _scanner.getSupportedSymbologiesAsync().then(function (symbologies) {
                    var listOfSymbologies = new WinJS.Binding.List(symbologies.map(function (symbology) {
                        return { id: symbology, name: BarcodeSymbologies.getName(symbology) };
                    }));
                    symbologyListViewControl.itemDataSource = listOfSymbologies.dataSource;

                    // Claim the scanner for exclusive use.
                    return _scanner.claimScannerAsync();
                });
            } else {
                WinJS.log("Barcode scanner not found. Please connect a barcode scanner.", "sample", "error");
            }
        }).then(function (claimedScanner) {
            _claimedScanner = claimedScanner;
            if (claimedScanner) {

                // Ask the API to decode the data by default. By setting this, API will decode the raw data from the barcode scanner and 
                // send the ScanDataLabel and ScanDataType in the DataReceived event
                claimedScanner.isDecodeDataEnabled = true;

                //// After successfully claiming, attach the datareceived event handler.
                claimedScanner.addEventListener("datareceived", onDataReceived);

                // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can 
                // claim ownership of the barcode scanner.
                claimedScanner.addEventListener("releasedevicerequested", onReleasedeviceRequested);

                // Enable the scanner so it starts raising events.
                return claimedScanner.enableAsync();
            } else if (_scanner) {
                _scanner.close();
                _scanner = null;
                WinJS.log("Claim barcode scanner failed.", "sample", "error");
            }
        }).done(function () {
            if (_claimedScanner) {
                WinJS.log("Ready to scan. Device ID: " + _claimedScanner.deviceId, "sample", "status");
                scenarioEndScanButton.disabled = false;
                setActiveSymbologiesButton.disabled = false;
            } else {
                scenarioStartScanButton.disabled = false;
            }
        });
    }

    // Event handler for the Release Device Requested event fired when barcode scanner receives Claim request from another application
    function onReleasedeviceRequested(args) {
        _claimedScanner.retainDevice();
        WinJS.log("Event ReleaseDeviceRequested received. Retaining the barcode scanner.", "sample", "status");
    }

    // Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner 
    function onDataReceived(args) {
        scenarioOutputScanDataType.textContent = BarcodeSymbologies.getName(args.report.scanDataType);
        scenarioOutputScanData.textContent = getDataString(args.report.scanData);
        scenarioOutputScanDataFormatted.textContent = getDataLabelString(args.report.scanDataLabel, args.report.scanDataType);
    }

    // This method removes the event listeners from the claimed scanner object if it is not null and then closes the claimed scanner so that someone else can use it.
    function endReceivingData() {
        closeScanners();

        // Reset button states
        WinJS.log("Click the start scanning button to begin.", "sample", "status");

        setActiveSymbologiesButton.disabled = true;
        scenarioStartScanButton.disabled = false;
        scenarioEndScanButton.disabled = true;
        scenarioOutputScanDataType.textContent = "No Data";
        scenarioOutputScanData.textContent = "No Data";
        scenarioOutputScanDataFormatted.textContent = "No Data";

        // reset symbology list
        symbologyListViewControl.itemDataSource = null;
    }

    // This method sets the active symbologies of the claimed scanner object.
    function setActiveSymbologies() {
        if (_claimedScanner) {

            // Collect the symbology IDs the user selected in the list control.
            symbologyListViewControl.selection.getItems().done(function (items) {
                var activeSymbologies = items.map(function (item) {
                    return item.data.id;
                })
                // Set them as active in the claimed scanner.
                _claimedScanner.setActiveSymbologiesAsync(activeSymbologies);
            });
        }
    }

    function closeScanners() {
        if (_claimedScanner) {
            _claimedScanner.removeEventListener("datareceived", onDataReceived);
            _claimedScanner.removeEventListener("releasedevicerequested", onReleasedeviceRequested);
            _claimedScanner.close();
            _claimedScanner = null;
        }
        if (_scanner) {
            _scanner.close();
            _scanner = null;
        }
    }
})();
