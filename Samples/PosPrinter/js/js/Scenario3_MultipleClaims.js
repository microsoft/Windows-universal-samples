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

    var claimResultText;

    var page = WinJS.UI.Pages.define("/html/Scenario3_MultipleClaims.html", {
        ready: function (element, options) {
            claimResultText = document.getElementById("claimResultText");

            document.getElementById("claimSamePrinterButton").addEventListener("click", claimSamePrinter);
        }
    });

    function claimSamePrinter() {
        claimResultText.innerText = "";
        WinJS.log("", "sample", "status");

        if (!SdkSample.printer) {
            WinJS.log("Use scenario 1 to find a printer first.", "sample", "error");
            return;
        }

        Windows.Devices.PointOfService.PosPrinter.fromIdAsync(SdkSample.printer.deviceId).then(function (printer) {
            if (!printer) {
                WinJS.log("Cannot create same printer as scenario 1.", "sample", "error");
            } else {
                return printer.claimPrinterAsync().then(function (claimedPrinter) {
                    if (claimedPrinter) {
                        claimResultText.innerText = "Claimed the printer.";

                        // This scenario doesn't do anything with the printer aside from claim it.
                        // Release our claim by closing the claimed printer.
                        claimedPrinter.close();
                    } else {
                        claimResultText.innerText = "Did not claim the printer.";
                    }

                    // Close the duplicate printer.
                    printer.close();
                })
            }
        }).done();
    }

})();
