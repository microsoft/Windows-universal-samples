//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;

    var licenseMode;

    var page = WinJS.UI.Pages.define("/html/scenario1-trialMode.html", {
        ready: function (element, options) {
            licenseMode = document.getElementById("licenseMode");
            var purchasePrice = document.getElementById("purchasePrice");

            document.getElementById("showTrialPeriodInformation").addEventListener("click", showTrialPeriodInformation);
            document.getElementById("purchaseFullLicense").addEventListener("click", purchaseFullLicense);

            CurrentAppSimulator.licenseInformation.addEventListener("licensechanged", onLicenseChanged);
            return SdkSample.configureSimulatorAsync("trial-mode.xml").then(function () {
                return CurrentAppSimulator.loadListingInformationAsync();
            }).then(function (listing) {
                purchasePrice.innerText = listing.formattedPrice;
            }, function () {
                WinJS.log && WinJS.log("LoadListingInformationAsync API call failed", "samples", "error");
            });
        },

        unload: function () {
            CurrentAppSimulator.licenseInformation.removeEventListener("licensechanged", onLicenseChanged);
        }
    });

    function onLicenseChanged() {
        var licenseInformation = CurrentAppSimulator.licenseInformation;
        if (licenseInformation.isActive) {
            if (licenseInformation.isTrial) {
                licenseMode.innerText = "Trial license";
            } else {
                licenseMode.innerText = "Full license";
            }
        } else {
            licenseMode.innerText = "Inactive license";
        }
    }

    function showTrialPeriodInformation() {
        var licenseInformation = CurrentAppSimulator.licenseInformation;
        if (licenseInformation.isActive) {
            if (licenseInformation.isTrial) {
                var remainingTrialTime = Math.ceil((licenseInformation.expirationDate - Date.now()) / 86400000);
                WinJS.log && WinJS.log("You can use this app for " + remainingTrialTime + " more days before the trial period ends.", "samples", "status");
            } else {
                WinJS.log && WinJS.log("You have a full license. The trial time is not meaningful.", "samples", "error");
            }
        } else {
            WinJS.log && WinJS.log("You don't have a license. The trial time can't be determined.", "samples", "error");
        }
    }

    function purchaseFullLicense() {
        WinJS.log && WinJS.log("Buying the full license...", "sample", "status");
        var licenseInformation = CurrentAppSimulator.licenseInformation;
        if (!licenseInformation.isActive || licenseInformation.isTrial) {
            CurrentAppSimulator.requestAppPurchaseAsync(false).done(function () {
                if (licenseInformation.isActive && !licenseInformation.isTrial) {
                    WinJS.log && WinJS.log("You successfully upgraded your app to the fully-licensed version.", "sample", "status");
                } else {
                    WinJS.log && WinJS.log("You decided not to upgrade.", "sample", "error");
                }
            }, function () {
                WinJS.log && WinJS.log("The upgrade transaction faioed.", "sample", "error");
            });
        } else {
            WinJS.log && WinJS.log("You already bought this app and have a fully-licensed version.", "sample", "error");
        }
    }
})();
