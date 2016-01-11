//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;

    var product1Message;

    var page = WinJS.UI.Pages.define("/html/scenario3-expiringProduct.html", {
        ready: function (element, options) {
            product1Message = document.getElementById("product1Message");

            CurrentAppSimulator.licenseInformation.addEventListener("licensechanged", onLicenseChanged);
            return SdkSample.configureSimulatorAsync("expiring-product.xml");
        },

        unload: function () {
            CurrentAppSimulator.licenseInformation.removeEventListener("licensechanged", onLicenseChanged);
        }
    });

    function onLicenseChanged() {
        var licenseInformation = CurrentAppSimulator.licenseInformation;
        if (licenseInformation.isActive) {
            if (licenseInformation.isTrial) {
                WinJS.log && WinJS.Log("You don't have full license to this app.", "sample", "error");
            } else {
                var productLicense1 = licenseInformation.productLicenses["product1"];
                if (productLicense1.isActive) {
                    var longdateTemplate = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("longdate");
                    var remainingTrialTime = Math.ceil((productLicense1.expirationDate - Date.now()) / 86400000);
                    product1Message.innerText = "Product 1 expires on " + longdateTemplate.format(productLicense1.expirationDate) + ". Days remaining: " + remainingTrialTime;
                } else {
                    WinJS.log && WinJS.Log("Product 1 is not installed.", "sample", "error");
                }
            }
        } else {
            WinJS.log && WinJS.Log("You don't have an active license.", "sample", "error");
        }
    }
})();
