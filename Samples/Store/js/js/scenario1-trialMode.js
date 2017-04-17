//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var StorePurchaseStatus = Windows.Services.Store.StorePurchaseStatus;
    var storeContext = Windows.Services.Store.StoreContext.getDefault();

    var licenseMode;

    var page = WinJS.UI.Pages.define("/html/scenario1-trialMode.html", {
        ready: function (element, options) {
            licenseMode = document.getElementById("licenseMode");
            var purchasePrice = document.getElementById("purchasePrice");

            document.getElementById("showTrialPeriodInformation").addEventListener("click", showTrialPeriodInformation);
            document.getElementById("purchaseFullLicense").addEventListener("click", purchaseFullLicense);

            storeContext.addEventListener("offlinelicenseschanged", onLicenseChanged);

            storeContext.getStoreProductForCurrentAppAsync().then(function (result) {
                if (!result.extendedError) {
                    purchasePrice.innerText = result.product.price.formattedPrice;
                }
            });
        },

        unload: function () {
            storeContext.removeEventListener("offlinelicenseschanged", onLicenseChanged);
        }
    });

    function onLicenseChanged(sender, args) {
        getLicenseState();
    }

    function getLicenseState() {
        storeContext.getAppLicenseAsync().then(function(license) {
            if (license.isActive) {
                if (license.isTrial) {
                    licenseMode.innerText = "Trial license";
                } else {
                    licenseMode.innerText = "Full license";
                }
            } else {
                licenseMode.innerText = "Inactive license";
            }
        });
    }

    function showTrialPeriodInformation() {
        storeContext.getAppLicenseAsync().then(function(license) {
            if (license.isActive) {
                if (license.isTrial) {
                    var remainingTrialTime = Math.ceil((license.expirationDate - Date.now()) / 86400000);
                    WinJS.log && WinJS.log("You can use this app for " + remainingTrialTime + " more days before the trial period ends.", "samples", "status");
                }
                else {
                    WinJS.log && WinJS.log("You have a full license. The trial time is not meaningful.", "samples", "error");
                }
            }
            else
            {
                WinJS.log && WinJS.log("You don't have a license. The trial time can't be determined.", "samples", "error");
            }
        });
    }

    function purchaseFullLicense() {
        storeContext.getStoreProductForCurrentAppAsync().then(function (productResult) {
            if (productResult.extendedError) {
                SdkSample.reportExtendedError(productResult.extendedError);
                return;
            }
            WinJS.log && WinJS.log("Buying the full license...", "sample", "status");
            storeContext.getAppLicenseAsync().then(function (license) {
                if (license.isTrial) {
                    return productResult.product.requestPurchaseAsync().then(function (result) {
                        switch (result.status) {
                            case StorePurchaseStatus.alreadyPurchased:
                                WinJS.log && WinJS.log("You already bought this app and have a fully-licensed version.", "sample", "error");
                                break;

                            case StorePurchaseStatus.succeeded:
                                // License will refresh automatically using the StoreContext.OfflineLicensesChanged event
                                break;

                            case StorePurchaseStatus.notPurchased:
                                WinJS.log && WinJS.log("Product was not purchased, it may have been canceled.", "sample", "error");
                                break;

                            case StorePurchaseStatus.networkError:
                                WinJS.log && WinJS.log("Product was not purchased due to a Network Error.", "sample", "error");
                                break;

                            case StorePurchaseStatus.serverError:
                                WinJS.log && WinJS.log("Product was not purchased due to a Server Error.", "sample", "error");
                                break;

                            default:
                                WinJS.log && WinJS.log("Product was not purchased due to an Unknown Error.", "sample", "error");
                                break;
                        }
                    });
                } else {
                    WinJS.log && WinJS.log("You already bought this app and have a fully-licensed version.", "sample", "error");
                }
            });
        });
    }
})();
