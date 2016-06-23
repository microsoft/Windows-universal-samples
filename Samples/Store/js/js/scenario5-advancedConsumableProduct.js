//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;
    var ProductPurchaseStatus = Windows.ApplicationModel.Store.ProductPurchaseStatus;
    var FulfillmentResult = Windows.ApplicationModel.Store.FulfillmentResult;

    var purchaseInfos;

    var page = WinJS.UI.Pages.define("/html/scenario5-advancedConsumableProduct.html", {
        ready: function (element, options) {
            purchaseInfos = [
                {
                    productId: "product1",
                    tempTransactionId: "00000000-0000-0000-0000-000000000000",
                    numPurchases: 0,
                    numFulfillments: 0,
                    nameElement: document.getElementById("product1Name"),
                    priceElement: document.getElementById("product1Price"),
                    purchasesElement: document.getElementById("product1Purchases"),
                    fulfilledElement: document.getElementById("product1Fulfilled"),
                    consumedTransactionIds: {}
                },
                {
                    productId: "product2",
                    tempTransactionId: "00000000-0000-0000-0000-000000000000",
                    numPurchases: 0,
                    numFulfillments: 0,
                    nameElement: document.getElementById("product2Name"),
                    priceElement: document.getElementById("product2Price"),
                    purchasesElement: document.getElementById("product2Purchases"),
                    fulfilledElement: document.getElementById("product2Fulfilled"),
                    consumedTransactionIds: {}
                }
            ];

            document.getElementById("showUnfulfilledConsumables").addEventListener("click", showUnfulfilledConsumables);
            document.getElementById("buyProduct1").addEventListener("click", buyProduct1);
            document.getElementById("fulfillProduct1").addEventListener("click", fulfillProduct1);
            document.getElementById("buyProduct2").addEventListener("click", buyProduct2);
            document.getElementById("fulfillProduct2").addEventListener("click", fulfillProduct2);

            return SdkSample.configureSimulatorAsync("in-app-purchase-consumables-advanced.xml").then(function () {
                return CurrentAppSimulator.loadListingInformationAsync();
            }).then(function (listing) {
                // Initialize the UI for our consumables.
                purchaseInfos.forEach(function (info) {
                    var product = listing.productListings[info.productId];
                    info.nameElement.innerText = product.name;
                    info.priceElement.innerText = product.formattedPrice;
                });

                WinJS.log && WinJS.log("", "samples", "status");
            }, function () {
                WinJS.log && WinJS.log("LoadListingInformationAsync API call failed", "samples", "error");
            }).then(function () {
                // Recover any unfulfilled consumables.
                return CurrentAppSimulator.getUnfulfilledConsumablesAsync();
            }).then(function (consumables) {
                consumables.forEach(function (consumable) {
                    purchaseInfos.forEach(function (info) {
                        if (info.productId === consumable.productId) {
                            // This is a consumable which the user purchased but which has not yet been fulfilled.
                            // Update our statistics so we know that there is a purchase pending.
                            info.numPurchases++;

                            // This is where you would normally grant the user their consumable content and call CurrentApp(Simulator).reportConsumableFulfillment.
                            // For demonstration purposes, we leave the purchase unfulfilled.
                            info.tempTransactionId = consumable.TransactionId;
                        }
                    });
                });
            }, function () {
                WinJS.log && WinJS.log("GetUnfulfilledConsumablesAsync API call failed", "samples", "error");
            }).then(function () {
                updateStatistics();
            });
        }
    });

    function showUnfulfilledConsumables() {
        CurrentAppSimulator.getUnfulfilledConsumablesAsync().done(function (consumables) {
            var logMessage = "Number of unfulfilled consumables: " + consumables.length;
            consumables.forEach(function (consumable) {
                logMessage += "\nProduct Id: " + consumable.productId + " Transaction Id: " + consumable.transactionId;
            });
            WinJS.log && WinJS.log(logMessage, "samples", "status");
        }, function() {
            WinJS.log && WinJS.log("GetUnfulfilledConsumablesAsync API call failed", "samples", "error");
        });
    }

    function buyProduct(info) {
        WinJS.log && WinJS.log("Buying " + info.nameElement.innerText + "...", "sample", "status");
        CurrentAppSimulator.requestProductPurchaseAsync(info.productId).then(function (purchaseResults) {
            switch (purchaseResults.status) {
                case ProductPurchaseStatus.succeeded:
                    info.numPurchases++;
                    info.tempTransactionId = purchaseResults.transactionId;
                    WinJS.log && WinJS.log("You bought " + info.nameElement.innerText + ". Transaction Id: " + purchaseResults.transactionId, "sample", "status");

                    // Normally you would grant the user their content here, and then call currentApp.reportConsumableFulfillment
                    // For demonstration purposes, we leave the purchase unfulfilled.
                    break;
                case ProductPurchaseStatus.notFulfilled:
                    info.tempTransactionId = purchaseResults.transactionId;
                    WinJS.log && WinJS.log("You have an unfulfilled copy of " + info.nameElement.innerText + ". Fulfill it before purchasing another one.", "sample", "status");

                    // Normally you would grant the user their content here, and then call currentApp.reportConsumableFulfillment
                    // For demonstration purposes, we leave the purchase unfulfilled.
                    break;
                case ProductPurchaseStatus.notPurchased:
                    WinJS.log && WinJS.log(info.nameElement.innerText + " was not purchased.", "sample", "status");
                    break;
            }
        }, function () {
            WinJS.log && WinJS.log("Unable to buy " + info.nameElement.innerText + ".", "sample", "error");
        }).done(function () {
            updateStatistics();
        });
    }

    function fulfillProduct(info) {
        if (!isLocallyFulfilled(info, info.tempTransactionId)) {
            grantFeatureLocally(info, info.tempTransactionId);
        }

        CurrentAppSimulator.reportConsumableFulfillmentAsync(info.productId, info.tempTransactionId).then(function (result) {
            switch (result) {
                case FulfillmentResult.succeeded:
                    info.numFulfillments++;
                    WinJS.log && WinJS.log(info.nameElement.innerText + " was fulfilled. You are now able to buy another one.", "sample", "status");
                    break;
                case FulfillmentResult.nothingToFulfill:
                    WinJS.log && WinJS.log("There is nothing to fulfill. You must purchase " + info.nameElement.innerText + " before it can be fulfilled.", "sample", "status");
                    break;
                case FulfillmentResult.purchasePending:
                    WinJS.log && WinJS.log("Purchase hasn't completed yet. Wait and try again.", "sample", "status");
                    break;
                case FulfillmentResult.purchaseReverted:
                    WinJS.log && WinJS.log("Purchase was reverted before fulfillment.", "sample", "status");
                    // Since the user's purchase was revoked, they got their money back.
                    // You may want to revoke the user's access to the consumable content that was granted.
                    break;
                case FulfillmentResult.serverError:
                    WinJS.log && WinJS.log("There was an error when fulfilling.", "sample", "status");
                    break;
            }
        }, function () {
            WinJS.log && WinJS.log("There was an error when fulfilling.", "sample", "error");
        }).done(function () {
            updateStatistics();
        });
    }

    function buyProduct1() {
        buyProduct(purchaseInfos[0]);
    }

    function fulfillProduct1() {
        fulfillProduct(purchaseInfos[0]);
    }

    function buyProduct2() {
        buyProduct(purchaseInfos[1]);
    }

    function fulfillProduct2() {
        fulfillProduct(purchaseInfos[1]);
    }

    function updateStatistics() {
        purchaseInfos.forEach(function (info) {
            info.purchasesElement.innerText = info.numPurchases;
            info.fulfilledElement.innerText = info.numFulfillments;
        });
    }

    function grantFeatureLocally(info, transactionId) {        
        info.consumedTransactionIds[transactionId] = true;

        // Grant the user their content. You will likely increase some kind of gold/coins/some other asset count.
    }

    function isLocallyFulfilled(info, transactionId) {
        return info.consumedTransactionIds[transactionId];
    }
})();
