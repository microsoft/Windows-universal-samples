//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;
    var ProductPurchaseStatus = Windows.ApplicationModel.Store.ProductPurchaseStatus;
    var FulfillmentResult = Windows.ApplicationModel.Store.FulfillmentResult;
    var ProductPurchaseDisplayProperties = Windows.ApplicationModel.Store.ProductPurchaseDisplayProperties;

    var product1ListingName;

    var offerIdInput;
    var displayNameInput;
    var productName;

    var consumedTransactionIds = {};

    var page = WinJS.UI.Pages.define("/html/scenario6-largeAppCatalog.html", {
        ready: function (element, options) {
            offerIdInput = document.getElementById("offerIdInput");
            displayNameInput = document.getElementById("displayNameInput");
            productName = document.getElementById("productName");
            var productPrice = document.getElementById("productPrice");

            document.getElementById("buyAndFulfillProduct").addEventListener("click", buyAndFulfillProduct);
            displayNameInput.addEventListener("input", productNameChanged);

            return SdkSample.configureSimulatorAsync("in-app-purchase-large-catalog.xml").then(function () {
                return CurrentAppSimulator.loadListingInformationAsync();
            }).then(function (listing) {
                var product1 = listing.productListings["product1"];
                product1ListingName = product1.name;
                productPrice.innerText = product1.formattedPrice;
                WinJS.log && WinJS.log("", "samples", "status");
            }, function () {
                WinJS.log && WinJS.log("LoadListingInformationAsync API call failed", "samples", "error");
            }).then(function () {
                productNameChanged();
            });
        }
    });

    function buildProductNameForDisplay() {
        return displayNameInput.value || product1ListingName;
    }

    function productNameChanged() {
        productName.innerText = buildProductNameForDisplay();
    }

    function buildOfferStringForDisplay(offerId) {
        return offerId ? " with offer id " + offerId : " with no offer id";
    }

    function buyAndFulfillProduct() {
        var offerId = offerIdInput.value;
        var displayPropertiesName = displayNameInput.value;
        var displayProperties = new ProductPurchaseDisplayProperties(displayPropertiesName);

        WinJS.log && WinJS.log("Buying product 1...", "sample", "status");
        CurrentAppSimulator.requestProductPurchaseAsync("product1", offerId, displayProperties).done(function (purchaseResults) {
            switch (purchaseResults.status) {
                case ProductPurchaseStatus.succeeded:
                    grantFeatureLocally(purchaseResults.transactionId);
                    fulfillProduct(purchaseResults);
                    break;
                case ProductPurchaseStatus.notFulfilled:
                    if (!isLocallyFulfilled(purchaseResults.transactionId)) {
                        grantFeatureLocally(purchaseResults.transactionId);
                    }
                    fulfillProduct(purchaseResults);
                    break;
                case ProductPurchaseStatus.notPurchased:
                    WinJS.log && WinJS.log("Product 1 was not purchased.", "sample", "status");
                    break;
            }
        }, function () {
            WinJS.log && WinJS.log("Unable to buy product 1.", "sample", "error");
        });
    }

    function fulfillProduct(purchaseResults) {
        var itemDescription = product1ListingName + buildOfferStringForDisplay(purchaseResults.offerId);
        var purchaseStringSimple = "You bought " + itemDescription + ".";
        if (purchaseResults.status === ProductPurchaseStatus.notFulfilled) {
            purchaseStringSimple = "You already purchased " + itemDescription + ".";
        }

        CurrentAppSimulator.reportConsumableFulfillmentAsync("product1", purchaseResults.transactionId).done(function (result) {
            switch (result) {
                case FulfillmentResult.succeeded:
                    if (purchaseResults.status === ProductPurchaseStatus.notFulfilled) {
                        WinJS.log && WinJS.log("You already purchased " + itemDescription + " and it was just fulfilled.", "sample", "status");
                    } else {
                        WinJS.log && WinJS.log("You bought and fulfilled " + itemDescription, "sample", "status");
                    }
                    break;
                case FulfillmentResult.nothingToFulfill:
                    WinJS.log && WinJS.log("There is no purchased product 1 to fulfill with that transaction id.", "sample", "status");
                    break;
                case FulfillmentResult.purchasePending:
                    log(purchaseStringSimple + " The purchase is pending so we cannot fulfill the product.", "sample", "status");
                    break;
                case FulfillmentResult.purchaseReverted:
                    WinJS.log && WinJS.log(purchaseStringSimple + " But your purchase has been reverted.", "sample", "status");
                    // Since the user's purchase was revoked, they got their money back.
                    // You may want to revoke the user's access to the consumable content that was granted.
                    break;
                case FulfillmentResult.serverError:
                    WinJS.log && WinJS.log(purchaseStringSimple + " There was an error when fulfilling.", "sample", "status");
                    break;
            }
        }, function () {
            WinJS.log && WinJS.log(purchaseStringSimple + " There was an error when fulfilling.", "sample", "error");
        });
    }

    function grantFeatureLocally(transactionId) {
        consumedTransactionIds[transactionId] = true;
    }

    function isLocallyFulfilled(transactionId) {
        return consumedTransactionIds[transactionId];
    }
})();
