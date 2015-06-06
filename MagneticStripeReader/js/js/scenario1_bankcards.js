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
    var page = WinJS.UI.Pages.define("/html/Scenario1_BankCards.html", {

        ready: function (element, options) {

            WinJS.log("Click the Start Receiving Data Button.", "sample", "status");

            document.getElementById("startReadButton").addEventListener("click", startRead, false);
            document.getElementById("endReadButton").addEventListener("click", endRead, false);
            document.getElementById("startReadButton").disabled = false;
            document.getElementById("endReadButton").disabled = true;
        },

        unload: function () {
            if (document.getElementById("startReadButton").disabled) {
                // Close the magnetic stripe reader.
                if (_claimedReader !== null) {
                    _claimedReader.removeEventListener("bankcarddatareceived", onBankCardDataReceived);
                    _claimedReader.close();
                    _claimedReader = null;
                }

                _reader = null;
            }
        }
    });

    var _reader = null;
    var _claimedReader = null;

    function startRead() {
        // Get the default magnetic stripe reader.
        Windows.Devices.PointOfService.MagneticStripeReader.getDefaultAsync().then(function (reader) {
            if (reader !== null) {
                _reader = reader;

                // Claim the magnetic stripe reader for exclusive use.
                reader.claimReaderAsync().done(function (claimedReader) {
                    if (claimedReader !== null) {
                        _claimedReader = claimedReader;
                        claimedReader.isDecodeDataEnabled = true;

                        // Register event listeners
                        claimedReader.addEventListener("bankcarddatareceived", onBankCardDataReceived);

                        // Enable receiving data
                        claimedReader.enableAsync().done(function () {
                            WinJS.log("Ready to swipe. Device ID: " + claimedReader.deviceId, "sample", "status");
                            document.getElementById("startReadButton").disabled = true;
                            document.getElementById("endReadButton").disabled = false;
                        }, function error(e) {
                            WinJS.log("Error enabling reader..." + e.message, "sample", "error");
                        });
                    }
                    else {
                        WinJS.log("Could not claim reader...", "sample", "error");
                    }

                }, function error(e) {
                    WinJS.log("Could not claim reader..." + e.message, "sample", "error");
                });
            }
            else {
                WinJS.log("Magnetic Stripe Reader not found.  Please connect a Magnetic Stripe Reader.", "sample", "error");
            }

        }, function error(e) {
            WinJS.log("Magnetic Stripe Reader not found.  Please connect a Magnetic Stripe Reader.", "sample", "error");
        });
    }

    function onBankCardDataReceived(args) {

        WinJS.log("Got data.", "sample", "status");

        // Display the received data
        document.getElementById("accountNumber").textContent = args.accountNumber;
        document.getElementById("expirationDate").textContent = args.expirationDate;
        document.getElementById("firstName").textContent = args.firstName;
        document.getElementById("middleInitial").textContent = args.middleInitial;
        document.getElementById("serviceCode").textContent = args.serviceCode;
        document.getElementById("suffix").textContent = args.suffix;
        document.getElementById("surname").textContent = args.surname;
        document.getElementById("title").textContent = args.title;
    }

    function endRead() {
        // Remove event listeners and unclaim the reader.
        if (_claimedReader !== null) {
            _claimedReader.removeEventListener("bankcarddatareceived", onBankCardDataReceived);
            _claimedReader.close();
            _claimedReader = null;
        }
        _reader = null;

        WinJS.log("Click the Start Receiving Data Button.", "sample", "status");
        document.getElementById("startReadButton").disabled = false;
        document.getElementById("endReadButton").disabled = true;

        // Clear any displayed data.
        document.getElementById("accountNumber").textContent = "No data";
        document.getElementById("expirationDate").textContent = "No data";
        document.getElementById("firstName").textContent = "No data";
        document.getElementById("middleInitial").textContent = "No data";
        document.getElementById("serviceCode").textContent = "No data";
        document.getElementById("suffix").textContent = "No data";
        document.getElementById("surname").textContent = "No data";
        document.getElementById("title").textContent = "No data";
    }
})();
