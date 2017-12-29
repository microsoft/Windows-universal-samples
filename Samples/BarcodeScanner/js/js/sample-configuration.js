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

    var sampleTitle = "Barcode Scanner";

    var scenarios = [
        { url: "/html/scenario1_BasicFunctionality.html", title: "DataReceived event" },
        { url: "/html/scenario2_MultipleScanners.html", title: "Release/Retain functionality" },
        { url: "/html/scenario3_ActiveSymbologies.html", title: "Active Symbologies" },
        { url: "/html/scenario4_SymbologyAttributes.html", title: "Symbology Attributes" },
    ];

    var BarcodeScanner = Windows.Devices.PointOfService.BarcodeScanner;

    function getFirstBarcodeScannerAsync(connectionTypes) {
        if (connectionTypes === undefined) {
            // By default, use all connections types.
            connectionTypes = Windows.Devices.PointOfService.PosConnectionTypes.all;
        }
        return DeviceHelpers.getFirstDeviceAsync(BarcodeScanner.getDeviceSelector(connectionTypes), (id) => BarcodeScanner.fromIdAsync(id));
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        getFirstBarcodeScannerAsync: getFirstBarcodeScannerAsync
    });
})();