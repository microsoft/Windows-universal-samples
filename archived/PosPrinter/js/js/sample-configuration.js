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

    var sampleTitle = "POS Printer JS Sample";

    var scenarios = [
        { url: "/html/Scenario1_FindClaimEnable.html", title: "Find, claim, and enable printer" },
        { url: "/html/Scenario2_PrintReceipt.html", title: "Print receipt" },
        { url: "/html/Scenario3_MultipleClaims.html", title: "Multiple claims" },
    ];

    var PosPrinter = Windows.Devices.PointOfService.PosPrinter;

    function subscribeToReleaseDeviceRequested() {
        SdkSample.claimedPrinter.addEventListener("releasedevicerequested", claimedPrinter_releaseDeviceRequested);
    }

    function releaseClaimedPrinter() {
        if (SdkSample.claimedPrinter) {
            SdkSample.claimedPrinter.removeEventListener("releasedevicerequested", claimedPrinter_releaseDeviceRequested);
            SdkSample.claimedPrinter.close();
            SdkSample.claimedPrinter = null;
            if (SdkSample.onstatechanged) SdkSample.onstatechanged();
        }
    }

    function releaseAllPrinters() {
        releaseClaimedPrinter();

        if (SdkSample.printer) {
            SdkSample.printer.close();
            SdkSample.printer = null;
            if (SdkSample.onstatechanged) SdkSample.onstatechanged();
        }
    }

    // If the "Retain device" checkbox is checked, we retain the device.
    // Otherwise, we allow the other claimant to claim the device.
    function claimedPrinter_releaseDeviceRequested(args) {
        if (SdkSample.isAnImportantTransaction) {
            SdkSample.claimedPrinter.retainDeviceAsync();
        } else {
            WinJS.log("Lost printer claim.", "sample", "error");
            releaseClaimedPrinter();
        }
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        printer: null,
        deviceInfo: null,
        claimedPrinter: null,
        isAnImportantTransaction: true,
        onstatechanged: null,
        subscribeToReleaseDeviceRequested: subscribeToReleaseDeviceRequested
    });
})();
