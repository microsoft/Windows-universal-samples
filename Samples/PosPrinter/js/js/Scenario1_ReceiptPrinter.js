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
    var page = WinJS.UI.Pages.define("/html/Scenario1_ReceiptPrinter.html", {

        ready: function (element, options) {

            WinJS.log("Find and claim the printer to begin.", "sample", "status");

            document.getElementById("findClaimEnableButton").disabled = false;
            document.getElementById("printLineButton").disabled = true;
            document.getElementById("printReceiptButton").disabled = true;
            document.getElementById("scenarioEndButton").disabled = true;

            document.getElementById("findClaimEnableButton").addEventListener("click", findClaimEnable, false);
            document.getElementById("scenarioEndButton").addEventListener("click", endScenario, false);
            document.getElementById("printLineButton").addEventListener("click", printLine, false);
            document.getElementById("printReceiptButton").addEventListener("click", printReceipt, false);
        },

        unload: function () {
            if (_claimedPrinter !== null) {
                _claimedPrinter.close();
                _claimedPrinter = null;
            }

            if (_printer !== null) {
                _printer.close();
                _printer = null;
            }
        }
    });

    var _printer = null;
    var _claimedPrinter = null;
    var _isImportantTransaction = true;

    //
    //Creates multiple tasks, first to find a receipt printer, then to claim the printer and then to enable and add releasedevicerequested event handler.
    //
    function findClaimEnable() {

        WinJS.log("Finding printer...", "sample", "status");

        if (_printer == null) {

            SdkSample.getFirstReceiptPrinterAsync().then(function (printer) {

                if (printer != null) {
                    _printer = printer;

                    //Claim
                    _printer.claimPrinterAsync().done(function (claimedPrinter) {

                        if (claimedPrinter !== null) {

                            _claimedPrinter = claimedPrinter;

                            //Enable printer
                            _claimedPrinter.enableAsync().done(function (success) {

                                if (success) {
                                    WinJS.log("Enabled printer. Device ID: " + _claimedPrinter.deviceId, "sample", "status");

                                    document.getElementById("findClaimEnableButton").disabled = true;
                                    document.getElementById("printLineButton").disabled = false;
                                    document.getElementById("printReceiptButton").disabled = false;
                                    document.getElementById("scenarioEndButton").disabled = false;
                                }
                                else {
                                    WinJS.log("Could not enable printer.", "sample", "error");
                                }
                            });
                        } else {
                            WinJS.log("Could not claim the printer.", "sample", "error");
                        }
                    });
                } else {
                    WinJS.log("No printer found", "sample", "error");
                }
            });
        }
    }

    function endScenario() {

        if (_claimedPrinter !== null) {
            _claimedPrinter.close();
            _claimedPrinter = null;
        }

        if (_printer !== null) {
            _printer.close();
            _printer = null;
        }

        WinJS.log("Scenario ended.", "sample", "status");

        document.getElementById("findClaimEnableButton").disabled = false;
        document.getElementById("printLineButton").disabled = true;
        document.getElementById("printReceiptButton").disabled = true;
        document.getElementById("scenarioEndButton").disabled = true;
    }

    //
    //Prints the line that is in the textbox.
    //
    function printLine() {

        if (_claimedPrinter == null) {
            WinJS.log("Claimed printer instance is null. Cannot print.", "sample", "error");
        }
        else {

            var job = _claimedPrinter.receipt.createJob();
            job.printLine(document.getElementById("txtPrintLine").value);

            job.executeAsync().done(function () {
                WinJS.log("Printed line.", "sample", "status");
            }
            , function error(e) {
                WinJS.log("Was not able to print line: " + e.message, "sample", "error");
            }
            );
        }
    }

    //
    //Prints a sample receipt.
    //
    function printReceipt() {

        if (_claimedPrinter == null) {
            WinJS.log("Claimed printer instance is null. Cannot print.", "sample", "error");
        }
        else {
            var receiptString = "======================\n" +
                                "|   Sample Header    |\n" +
                                "======================\n" +
                                "Item             Price\n" +
                                "----------------------\n" +
                                "Books            10.40\n" +
                                "Games             9.60\n" +
                                "----------------------\n" +
                                "Total----------- 20.00\n";


            var merchantJob = _claimedPrinter.receipt.createJob();
            var merchantFooter = getMerchantFooter();
            printLineFeedAndCutPaper(merchantJob, receiptString + merchantFooter);
            var customerJob = _claimedPrinter.receipt.createJob();
            var customerFooter = getCustomerFooter();
            printLineFeedAndCutPaper(customerJob, receiptString + customerFooter);


            merchantJob.executeAsync().done(function () {
                WinJS.log("Printed merchant receipt.", "sample", "status");
            }, function error(e) {
                WinJS.log("Was not able to print receipt: " + e.message, "sample", "error");
            });

            customerJob.executeAsync().done(function () {
                WinJS.log("Printed customer receipt.", "sample", "status");
            }, function error(e) {
                WinJS.log("Was not able to print customer footer: " + e.message, "sample", "error");
            });
        }
    }

    function getMerchantFooter() {
        return "\n" +
               "______________________\n" +
               "Tip\n" +
               "\n" +
               "______________________\n" +
               "Signature\n" +
               "\n" +
               "Merchant Copy\n";
    }

    function getCustomerFooter() {
        return "\n" +
               "______________________\n" +
               "Tip\n" +
               "\n" +
               "Customer Copy\n";
    }

    // Cut the paper after printing enough blank lines to clear the paper cutter.
    function printLineFeedAndCutPaper(job, receipt) {

        if (_printer == null) {
            WinJS.log("No printers found. Cannot print.", "sample", "error");
        }
        else if (_claimedPrinter == null) {
            WinJS.log("Claimed printer instance is null. Cannot print.", "sample", "error");
        }
        else {
            // Passing a multi-line string to the print method results in
            // smoother paper feeding than sending multiple single-line strings
            // to printLine.
            var feedString = "";
            for (var n = 0; n < _claimedPrinter.receipt.linesToPaperCut; n++) {
                feedString += "\n";
            }
            job.print(receipt + feedString);
            if (_printer.capabilities.receipt.canCutPaper) {
                job.cutPaper();
            }
        }
    }
})();
