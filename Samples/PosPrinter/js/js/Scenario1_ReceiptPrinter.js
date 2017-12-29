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

    var BarcodeSymbologies = Windows.Devices.PointOfService.BarcodeSymbologies;
    var PosPrinterAlignment = Windows.Devices.PointOfService.PosPrinterAlignment;
    var PosPrinterBarcodeTextPosition = Windows.Devices.PointOfService.PosPrinterBarcodeTextPosition;

    var page = WinJS.UI.Pages.define("/html/Scenario2_PrintReceipt.html", {

        ready: function (element, options) {
        }
    });

    function isPrinterClaimed() {
        if (SdkSample.claimedPrinter) {
            WinJS.log("", "sample", "status");
            return true;
        } else {
            WinJS.log("Use scenario 1 to find, claim, and enable a receipt printer.", "sample", "error");
            return false;
        }
    }

    function executeJobAndReportResultAsync(job) {

        return job.executeAsync().then(function (success) {
            if (success) {
                WinJS.log("Printing complete.", "sample", "status");
            }
            else {
                var reason;
                var receipt = SdkSample.claimedPrinter.receipt;
                if (receipt.isCartridgeEmpty) {
                    reason = "Printer is out of ink. Please replace cartridge.";
                }
                else if (receipt.isCartridgeRemoved) {
                    reason = "Printer cartridge is missing. Please replace cartridge.";
                }
                else if (receipt.isCoverOpen) {
                    reason = "Printer cover is open. Please close it.";
                }
                else if (receipt.isHeadCleaning) {
                    reason = "Printer is currently cleaning the cartridge. Please wait until cleaning has completed.";
                }
                else if (receipt.isPaperEmpty) {
                    reason = "Printer is out of paper. Please insert a new roll.";
                }
                else {
                    reason = "Unable to print.";
                }
                WinJS.log(reason, "sample", "error");
            }
            return success;
        });
    }

    function printLine() {
        if (!isPrinterClaimed()) {
            return;
        }

        var job = SdkSample.claimedPrinter.receipt.createJob();
        job.printLine(printLineTextBox.value);

        executeJobAndReportResultAsync(job).done();
    }

    function printReceipt() {
        if (!isPrinterClaimed()) {
            return;
        }

        var receiptString =
            "======================\n" +
            "|   Sample Header    |\n" +
            "======================\n" +
            "Item             Price\n" +
            "----------------------\n" +
            "Books            10.40\n" +
            "Games             9.60\n" +
            "----------------------\n" +
            "Total----------- 20.00\n";

        // The job consists of two receipts, one for the merchant and one
        // for the customer.
        var job = SdkSample.claimedPrinter.receipt.createJob();
        printLineFeedAndCutPaper(job, receiptString + getMerchantFooter());
        printLineFeedAndCutPaper(job, receiptString + getCustomerFooter());

        executeJobAndReportResultAsync(job).done();
    }

    function getMerchantFooter() {
        return "\n" +
            "______________________\n" +
            "Signature\n" +
            "\n" +
            "Merchant Copy\n";
    }

    function getCustomerFooter() {
        return "\n" +
            "Customer Copy\n";
    }

    // Cut the paper after printing enough blank lines to clear the paper cutter.
    function printLineFeedAndCutPaper(job, receipt) {
        // Passing a multi-line string to the print method results in
        // smoother paper feeding than sending multiple single-line strings
        // to printLine.
        var feedString = "";
        for (var n = 0; n < SdkSample.claimedPrinter.receipt.linesToPaperCut; n++) {
            feedString += "\n";
        }
        job.print(receipt + feedString);
        if (SdkSample.printer.capabilities.receipt.canCutPaper) {
            job.cutPaper();
        }
    }

    function printBitmap() {
        if (!isPrinterClaimed()) {
            return;
        }

        loadLogoBitmapAsync().done(function (bitmapFrame) {
            SdkSample.claimedPrinter.receipt.isLetterQuality = true;
            var job = SdkSample.claimedPrinter.receipt.createJob();
            job.printBitmap(bitmapFrame, PosPrinterAlignment.center, 500);
            return executeJobAndReportResultAsync(job);
        });
    }

    function loadLogoBitmapAsync() {
        var uri = new Windows.Foundation.Uri("ms-appx:///images/coffee-logo.png");
        return Windows.Storage.StorageFile.getFileFromApplicationUriAsync(uri).then(function (file) {
            return file.openReadAsync();
        }).then(function (readStream) {
            return Windows.Graphics.Imaging.BitmapDecoder.createAsync(readStream);
        }).then(function (decoder) {
            return decoder.getFrameAsync(0);
        });
    }

    function printBarcode() {
        if (!isPrinterClaimed()) {
            return;
        }

        var job = SdkSample.claimedPrinter.receipt.createJob();
        job.printBarcode(barcodeText.value, BarcodeSymbologies.upca, 60, 3, PosPrinterBarcodeTextPosition.below, PosPrinterAlignment.center);
        executeJobAndReportResultAsync(job).done();
    }
})();
