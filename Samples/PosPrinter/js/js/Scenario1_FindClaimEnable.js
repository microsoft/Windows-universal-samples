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

    var findButton;
    var printerNameSpan;
    var claimAndEnableButton;
    var retainDeviceCheckBox;
    var releaseClaimedPrinterButton;
    var releaseAllPrintersButton;

    var isBusy = false;

    var page = WinJS.UI.Pages.define("/html/Scenario1_FindClaimEnable.html", {

        ready: function (element, options) {
            findButton = document.getElementById("findButton");
            printerNameSpan = document.getElementById("printerNameSpan");
            claimAndEnableButton = document.getElementById("claimAndEnableButton");
            retainDeviceCheckBox = document.getElementById("retainDeviceCheckBox");
            releaseClaimedPrinterButton = document.getElementById("releaseClaimedPrinterButton");
            releaseAllPrintersButton = document.getElementById("releaseAllPrintersButton");            

            findButton.addEventListener("click", findPrinter);
            claimAndEnableButton.addEventListener("click", claimAndEnable);
            retainDeviceCheckBox.addEventListener("click", isImportantTransactionChanged);
            releaseClaimedPrinterButton.addEventListener("click", SdkSample.releaseClaimedPrinter);
            releaseAllPrintersButton.addEventListener("click", SdkSample.releaseAllPrinters);

            retainDeviceCheckBox.checked = SdkSample.isAnImportantTransaction;
            SdkSample.onstatechanged = updateButtons;
            updateButtons();

        },

        unload: function () {
            SdkSample.onstatechanged = null;
        }
    });

    function updateButtons() {
        printerNameSpan.innerText = SdkSample.printer ? SdkSample.printer.deviceId : "None";
        if (isBusy) {
            findButton.disabled = true;
            claimAndEnableButton.disabled = true;
            releaseClaimedPrinterButton.disabled = true;
            releaseAllPrintersButton.disabled = true;
        } else if (!SdkSample.printer) {
            findButton.disabled = false;
            claimAndEnableButton.disabled = true;
            releaseClaimedPrinterButton.disabled = true;
            releaseAllPrintersButton.disabled = true;
        } else {
            findButton.disabled = true;
            releaseAllPrintersButton.disabled = false;
            if (!SdkSample.claimedPrinter) {
                claimAndEnableButton.disabled = false;
                releaseClaimedPrinterButton.disabled = true;
            } else {
                claimAndEnableButton.disabled = true;
                releaseClaimedPrinterButton.disabled = false;
            }
        }
    }

    function findPrinter() {
        isBusy = true;
        updateButtons();
        WinJS.log("Finding printer", "sample", "status");
        SdkSample.getFirstReceiptPrinterAsync().done(function (printer) {
            SdkSample.printer = printer;
            if (printer) {
                WinJS.log("Found receipt printer.", "sample", "status");
            } else {
                WinJS.log("No receipt printer found.", "sample", "error");
            }
            isBusy = false;
            updateButtons();
        });
    }

    function claimAndEnable() {
        isBusy = true;
        updateButtons();
        SdkSample.printer.claimPrinterAsync().then(function (claimedPrinter) {
            SdkSample.claimedPrinter = claimedPrinter;
            if (!claimedPrinter) {
                WinJS.log("Unable to claim printer", "sample", "error");
            } else {
                WinJS.log("Claimed printer", "sample", "status");

                // Register for the ReleaseDeviceRequested event so we know when somebody
                // wants to claim the printer away from us.
                SdkSample.subscribeToReleaseDeviceRequested();

                return claimedPrinter.enableAsync().then(function (enabled) {
                    if (enabled) {
                        WinJS.log("Enabled printer.", "sample", "status");
                    }
                    else {
                        WinJS.log("Could not enable printer", "sample", "error");
                        SdkSample.releaseClaimedPrinter();
                    }
                });
            }
        }).done(function () {
            isBusy = false;
            updateButtons();
        })
    }

    function isImportantTransactionChanged() {
        SdkSample.isAnImportantTransaction = retainDeviceCheckBox.checked;
    }
})();
