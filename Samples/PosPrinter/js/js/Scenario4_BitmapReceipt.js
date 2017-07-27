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
    var page = WinJS.UI.Pages.define("/html/Scenario4_BitmapReceipt.html", {

        ready: function (element, options) {

            WinJS.log("Find and claim the printer to begin.", "sample", "status");

            document.getElementById("findClaimEnableButton").addEventListener("click", findClaimEnable, false);
            document.getElementById("isImportantTransactionCheckBox").addEventListener("click", toggleIsImportant, false);
            document.getElementById("scenarioEndButton").addEventListener("click", endScenario, false);
            document.getElementById("printBitmapButton").addEventListener("click", printBitmap, false);
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
                                    document.getElementById("printBitmapButton").disabled = false;
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

    function toggleIsImportant(e) {
        _isImportantTransaction = e.target.checked;
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
        document.getElementById("printBitmapButton").disabled = true;
        document.getElementById("scenarioEndButton").disabled = true;
    }

    //
    //Prints the logo bitmap.
    //
    function printBitmap() {

        if (_claimedPrinter == null) {
            WinJS.log("Claimed printer instance is null. Cannot print.", "sample", "error");
        } else {
            var uri = new Windows.Foundation.Uri("ms-appx:///images/coffee-logo.png");
            Windows.Storage.StorageFile.getFileFromApplicationUriAsync(uri).then(function (file) {
                return file.openReadAsync();
            }).then(function (readStream) {
                return Windows.Graphics.Imaging.BitmapDecoder.createAsync(readStream);
            }).then(function (decoder) {
                return decoder.getFrameAsync(0);
            }).then(function (bitmapFrame) {
                _claimedPrinter.receipt.isLetterQuality = true;
                var job = _claimedPrinter.receipt.createJob();
                job.printBitmap(bitmapFrame, Windows.Devices.PointOfService.PosPrinterAlignment.Center, 500);
                return job.executeAsync();
            }).done(function (success) {
                if (success) {
                    WinJS.log("Was not able to print bitmap.", "sample", "error");
                } else {
                    WinJS.log("Printed Bitmap.", "sample", "status");
                }
            });
        }
    }
})();
