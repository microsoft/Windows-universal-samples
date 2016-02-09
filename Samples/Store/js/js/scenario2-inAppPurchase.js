//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;

    var product1Name;
    var product2Name;

    var page = WinJS.UI.Pages.define("/html/scenario2-inAppPurchase.html", {
        ready: function (element, options) {
            product1Name = document.getElementById("product1Name");
            var product1Price = document.getElementById("product1Price");
            product2Name = document.getElementById("product2Name");
            var product2Price = document.getElementById("product2Price");

            document.getElementById("testProduct1").addEventListener("click", testProduct1);
            document.getElementById("buyProduct1").addEventListener("click", buyProduct1);
            document.getElementById("testProduct2").addEventListener("click", testProduct2);
            document.getElementById("buyProduct2").addEventListener("click", buyProduct2);

            return SdkSample.configureSimulatorAsync("in-app-purchase.xml").then(function () {
                return CurrentAppSimulator.loadListingInformationAsync();
            }).then(function (listing) {
                var product1 = listing.productListings["product1"];
                product1Name.innerText = product1.name;
                product1Price.innerText = product1.formattedPrice;

                var product2 = listing.productListings["product2"];
                product2Name.innerText = product2.name;
                product2Price.innerText = product2.formattedPrice;
            }, function () {
                WinJS.log && WinJS.log("LoadListingInformationAsync API call failed", "samples", "error");
            });
        },

        unload: function () {
        }
    });

    function testProduct(productId, productName) {
        var licenseInformation = CurrentAppSimulator.licenseInformation;
        if (licenseInformation.productLicenses[productId].isActive) {
            WinJS.log && WinJS.log("You can use " + productName + ".", "samples", "status");
        } else {
            WinJS.log && WinJS.log("You don't own " + productName + ".", "samples", "error");
        }
    }

    function buyProduct(productId, productName) {
        var licenseInformation = CurrentAppSimulator.licenseInformation;
        if (!licenseInformation.productLicenses[productId].isActive) {
            WinJS.log && WinJS.log("Buying " + productName + "...", "samples", "status");
            CurrentAppSimulator.requestProductPurchaseAsync(productId).done(function () {
                if (licenseInformation.productLicenses[productId].isActive) {
                    WinJS.log && WinJS.log("You bought " + productName + ".", "samples", "status");
                } else {
                    WinJS.log && WinJS.log(productName + " was not purchased.", "samples", "status");
                }
            }, function () {
                WinJS.log && WinJS.log("Unable to buy " + productName + ".", "samples", "error");
            });
        } else {
            WinJS.log && WinJS.log("You already own " + productName + ".", "samples", "error");
        }
    }

    function testProduct1() {
        testProduct("product1", product1Name.innerText);
    }

    function buyProduct1() {
        buyProduct("product1", product1Name.innerText);
    }

    function testProduct2() {
        testProduct("product2", product2Name.innerText);
    }

    function buyProduct2() {
        buyProduct("product2", product2Name.innerText);
    }
})();
