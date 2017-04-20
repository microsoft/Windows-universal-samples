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

    var sampleTitle = "POS Printer";

    var scenarios = [
        { url: "/html/Scenario1_ReceiptPrinter.html", title: "Receipt Printer" },
        { url: "/html/Scenario2_ErrorHandling.html", title: "Error Handling" },
        { url: "/html/Scenario3_MultipleReceipt.html", title: "Multiple Receipt Printers" }
    ];

    var PosPrinter = Windows.Devices.PointOfService.PosPrinter;

    function getFirstReceiptPrinterAsync(connectionTypes) {
        if (connectionTypes === undefined) {
            // By default, use all connections types.
            connectionTypes = Windows.Devices.PointOfService.PosConnectionTypes.all;
        }
        return DeviceHelpers.getFirstDeviceAsync(PosPrinter.getDeviceSelector(connectionTypes), (id) => {
            return PosPrinter.fromIdAsync(id).then((printer) => {
                if (printer && printer.capabilities.receipt.isPrinterPresent) {
                    return printer;
                }
                // Close the unwanted printer.
                printer && printer.close();
                return null;
            });
        });
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        getFirstReceiptPrinterAsync: getFirstReceiptPrinterAsync
    });
})();
