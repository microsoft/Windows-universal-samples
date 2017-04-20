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
    var BarcodeSymbologyDecodeLengthKind = Windows.Devices.PointOfService.BarcodeSymbologyDecodeLengthKind;

    // Page elements
    var attributesTable;
    var scenarioStartScanButton;
    var scenarioEndScanButton;
    var setSymbologyAttributesButton;
    var scenarioOutputScanDataType;
    var scenarioOutputScanData;
    var scenarioOutputScanDataFormatted;

    // Will be defined when the page is initialized, otherwise we won't be able to find element
    var symbologyListViewControl = null;

    var page = WinJS.UI.Pages.define("/html/scenario4_SymbologyAttributes.html", {

        // On ready - set button states and add event listeners to the buttons.
        ready: function (element, options) {

            WinJS.log("Click the start scanning button to begin.", "sample", "status");

            attributesTable = document.getElementById("attributesTable");
            scenarioOutputScanDataType = document.getElementById("scenarioOutputScanDataType");
            scenarioOutputScanData = document.getElementById("scenarioOutputScanData");
            scenarioOutputScanDataFormatted = document.getElementById("scenarioOutputScanDataFormatted");

            scenarioStartScanButton = document.getElementById("scenarioStartScanButton");
            scenarioStartScanButton.addEventListener("click", startReceivingData, false);
            scenarioStartScanButton.disabled = false;

            scenarioEndScanButton = document.getElementById("scenarioEndScanButton");
            scenarioEndScanButton.addEventListener("click", endReceivingData, false);
            scenarioEndScanButton.disabled = true;

            document.getElementById("setDecodeRangeLimits").addEventListener("change", decodeRangeLimitsChanged, false);

            setSymbologyAttributesButton = document.getElementById("setSymbologyAttributesButton");
            setSymbologyAttributesButton.addEventListener("click", setSymbologyAttributesClicked, false);
            setSymbologyAttributesButton.disabled = true;

            var symbologyListView = document.getElementById("symbologyListView");
            symbologyListView.addEventListener("selectionchanged", symbologySelectionChanged, false);
            symbologyListViewControl = symbologyListView.winControl;
        },

        // On unload - clean up if claimedscanner object is not null.
        unload: function () {
            closeScanner();
        }

    });

    var _scanner = null;
    var _claimedScanner = null;
    var _symbologyEntry;
    var _symbologyAttributes;

    // Creates the default barcode scanner and claims it. Once, claimed, it adds event listeners for onDataReceived and releaseDeviceRequested events.
    // On error, it logs the error message.
    function startReceivingData() {
        scenarioStartScanButton.disabled = true;

        // Create the barcode scanner.
        WinJS.log("Creating barcode scanner object.", "sample", "status");

        SdkSample.getFirstBarcodeScannerAsync().then(function (scanner) {
            if (scanner) {
                _scanner = scanner;

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
        closeScanner();

        // Reset button states
        WinJS.log("Click the start scanning button to begin.", "sample", "status");

        setSymbologyAttributesButton.disabled = true;
        scenarioStartScanButton.disabled = false;
        scenarioEndScanButton.disabled = true;
        scenarioOutputScanDataType.textContent = "No Data";
        scenarioOutputScanData.textContent = "No Data";
        scenarioOutputScanDataFormatted.textContent = "No Data";

        attributesTable.style.visibility = "hidden";

        // reset symbology list
        symbologyListViewControl.itemDataSource = null;
    }

    function decodeRangeLimitsChanged() {
        minimumDecodeLength.disabled = !setDecodeRangeLimits.checked;
        maximumDecodeLength.disabled = !setDecodeRangeLimits.checked;
    }

    // Wrapper around getSymbologyAttributes which converts an exception to null.
    function safeGetSymbologyAttributesAsync(claimedScanner, symbology) {
        return claimedScanner.getSymbologyAttributesAsync(symbology).then(null, function () { return null; });
    }

    // Wrapper around getSymbologyAttributes which converts an exception to false.
    function safeSetSymbologyAttributesAsync(claimedScanner, symbology, symbologyAttributes) {
        return claimedScanner.setSymbologyAttributesAsync(symbology, symbologyAttributes).then(null, function () { return false; });
    }

    function symbologySelectionChanged() {
        if (_claimedScanner) {
            setSymbologyAttributesButton.disabled = true;
            return symbologyListViewControl.selection.getItems().then(function (items) {
                if (items.length > 0) {
                    _symbologyEntry = items[0].data;
                    return safeGetSymbologyAttributesAsync(_claimedScanner, _symbologyEntry.id);
                }
            }).done(function (symbologyAttributes) {
                _symbologyAttributes = symbologyAttributes;
                if (symbologyAttributes) {
                    attributesTable.style.visibility = "visible";
                    WinJS.log("", "sample", "error");
                    setSymbologyAttributesButton.disabled = false;

                    // initialize attributes UIs
                    enableCheckDigit.disabled = !symbologyAttributes.isCheckDigitValidationSupported;
                    enableCheckDigit.checked = symbologyAttributes.isCheckDigitValidationEnabled;
                    transmitCheckDigit.disabled = !symbologyAttributes.isCheckDigitTransmissionSupported;
                    transmitCheckDigit.checked = symbologyAttributes.isCheckDigitTransmissionEnabled;
                    setDecodeRangeLimits.disabled = !symbologyAttributes.isDecodeLengthSupported;
                    var decodeLengthEnabled = (symbologyAttributes.decodeLengthKind === BarcodeSymbologyDecodeLengthKind.range);
                    setDecodeRangeLimits.checked = decodeLengthEnabled;
                    minimumDecodeLength.disabled = !decodeLengthEnabled;
                    maximumDecodeLength.disabled = !decodeLengthEnabled;
                    if (decodeLengthEnabled) {
                        minimumDecodeLength.value = Math.min(symbologyAttributes.decodeLength1, symbologyAttributes.decodeLength2);
                        maximumDecodeLength.value = Math.max(symbologyAttributes.decodeLength1, symbologyAttributes.decodeLength2);
                    }
                } else {
                    attributesTable.style.visibility = "hidden";
                    WinJS.log("Unable to get symbology attributes.", "sample", "error");
                }
            });
        }
    }

    // This method sets the attributes of the selected symbology
    function setSymbologyAttributesClicked() {
        if (_claimedScanner && _symbologyAttributes) {
            setSymbologyAttributesButton.disabled = true;

            // Update the attributes object to match the UI.
            if (_symbologyAttributes.isCheckDigitValidationSupported) {
                _symbologyAttributes.isCheckDigitValidationEnabled = enableCheckDigit.checked;
            }
            if (_symbologyAttributes.isCheckDigitTransmissionSupported) {
                _symbologyAttributes.isCheckDigitTransmissionEnabled = transmitCheckDigit.checked;
            }
            if (_symbologyAttributes.isDecodeLengthSupported) {
                if (setDecodeRangeLimits.checked) {
                    _symbologyAttributes.decodeLengthKind = BarcodeSymbologyDecodeLengthKind.range;
                    _symbologyAttributes.decodeLength1 = minimumDecodeLength.value;
                    _symbologyAttributes.decodeLength2 = maximumDecodeLength.value;
                }
                else {
                    _symbologyAttributes.decodeLengthKind = BarcodeSymbologyDecodeLengthKind.anyLength;
                }
            }

            // Ask the scanner to use those attributes.
            safeSetSymbologyAttributesAsync(_claimedScanner, _symbologyEntry.id, _symbologyAttributes).then(function (success) {
                if (success) {
                    WinJS.log("Attributes set for symbology '" + _symbologyEntry.name + "'", "sample", "status");
                } else {
                    WinJS.log("Attributes could not be set for symbology '" + _symbologyEntry.name + "'", "sample", "error");
                }
                setSymbologyAttributesButton.disabled = false;
            });
        }
    }

    function closeScanner() {
        if (_claimedScanner) {
            // Remove event listeners that were added before and close.
            _claimedScanner.removeEventListener("datareceived", onDataReceived);
            _claimedScanner.removeEventListener("releasedevicerequested", onReleasedeviceRequested);
            _claimedScanner.close();
            _claimedScanner = null;
        }
        if (_scanner) {
            _scanner.close();
            _scanner = null;
        }
        _symbologyAttributes = null;
    }

})();
