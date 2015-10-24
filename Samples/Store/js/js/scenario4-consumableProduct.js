//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;
    var ProductPurchaseStatus = Windows.ApplicationModel.Store.ProductPurchaseStatus;
    var FulfillmentResult = Windows.ApplicationModel.Store.FulfillmentResult;

    var numberOfConsumablesPurchased = 0;
    var consumedTransactionIds = {};

    var purchaseCount;

    var page = WinJS.UI.Pages.define("/html/scenario4-consumableProduct.html", {
        ready: function (element, options) {
            purchaseCount = document.getElementById("purchaseCount");
            var product1Name = document.getElementById("product1Name");
            var product1Price = document.getElementById("product1Price");

            document.getElementById("buyAndFulfillProduct1").addEventListener("click", buyAndFulfillProduct1);

            return SdkSample.configureSimulatorAsync("in-app-purchase-consumables.xml").then(function () {
                return CurrentAppSimulator.loadListingInformationAsync();
            }).then(function (listing) {
                var product1 = listing.productListings["product1"];
                product1Name.innerText = product1.name;
                product1Price.innerText = product1.formattedPrice;
                WinJS.log && WinJS.log("", "samples", "status");
            }, function () {
                WinJS.log && WinJS.log("LoadListingInformationAsync API call failed", "samples", "error");
            });
        }
    });

    function buyAndFulfillProduct1() {
        WinJS.log && WinJS.log("Buying product 1...", "sample", "status");
        CurrentAppSimulator.requestProductPurchaseAsync("product1").done(function (purchaseResults) {
            if (purchaseResults.status === ProductPurchaseStatus.succeeded) {
                grantFeatureLocally(purchaseResults.transactionId);
                fulfillProduct1(purchaseResults.transactionId);
            } else if (purchaseResults.status === ProductPurchaseStatus.notFulfilled) {
                if (!isLocallyFulfilled(purchaseResults.transactionId)) {
                    grantFeatureLocally(purchaseResults.transactionId);
                }
                fulfillProduct1(purchaseResults.transactionId);
            } else if (purchaseResults.status === ProductPurchaseStatus.notPurchased) {
                WinJS.log && WinJS.log("Product 1 was not purchased.", "sample", "status");
            }
        },
        function () {
            WinJS.log && WinJS.log("Unable to buy product 1.", "sample", "error");
        });
    }

    function fulfillProduct1(transactionId) {
        CurrentAppSimulator.reportConsumableFulfillmentAsync("product1", transactionId).done(function (result) {
            switch (result) {
                case FulfillmentResult.succeeded:
                    WinJS.log && WinJS.log("You bought and fulfilled product 1.", "sample", "status");
                    break;
                case FulfillmentResult.nothingToFulfill:
                    WinJS.log && WinJS.log("There is no purchased product 1 to fulfill.", "sample", "status");
                    break;
                case FulfillmentResult.purchasePending:
                    WinJS.log && WinJS.log("You bought product 1. The purchase is pending so we cannot fulfill the product.", "sample", "status");
                    break;
                case FulfillmentResult.purchaseReverted:
                    WinJS.log && WinJS.log("You bought product 1, but your purchase has been reverted.", "sample", "status");
                    // Since the user's purchase was revoked, they got their money back.
                    // You may want to revoke the user's access to the consumable content that was granted.
                    break;
                case FulfillmentResult.serverError:
                    WinJS.log && WinJS.log("You bought product 1. There was an error when fulfilling.", "sample", "status");
                    break;
            }
        }, function () {
            WinJS.log && WinJS.log("You bought Product 1. There was an error when fulfilling.", "sample", "error");
        });
    }

    function grantFeatureLocally(transactionId) {
        consumedTransactionIds[transactionId] = true;

        // Grant the user the content, such as by increasing some kind of asset count
        numberOfConsumablesPurchased++;
        purchaseCount.innerText = numberOfConsumablesPurchased;
    }

    function isLocallyFulfilled(transactionId) {
        return consumedTransactionIds[transactionId];
    }
})();
