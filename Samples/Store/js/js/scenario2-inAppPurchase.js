//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var StorePurchaseStatus = Windows.Services.Store.StorePurchaseStatus;
    var storeContext = Windows.Services.Store.StoreContext.getDefault();

    var productsListView;

    var page = WinJS.UI.Pages.define("/html/scenario2-inAppPurchase.html", {
        ready: function (element, options) {
            productsListView = document.getElementById('productsListView').winControl;

            document.getElementById("getAssociatedProductsButton").addEventListener("click", getAssociatedProductsButton);
            SdkSample.addSingleItemClickHandler("purchaseAddOnButton", productsListView, purchaseAddOnButton);
        }
    });

    function getAssociatedProductsButton() {
        var productKinds = ["Consumable", "Durable", "UnmanagedConsumable"];
        storeContext.getAssociatedStoreProductsAsync(productKinds).then(function (addOns) {
            var productList = SdkSample.createProductListFromQueryResult(addOns, "Add-Ons");
            productsListView.itemDataSource = productList.dataSource;
        });
    }

    function purchaseAddOnButton(item) {
        storeContext.requestPurchaseAsync(item.storeId).done(function (result) {
            if (result.extendedError) {
                SdkSample.reportExtendedError(result.extendedError);
                return;
            }

            switch (result.status) {
                case StorePurchaseStatus.alreadyPurchased:
                    WinJS.log && WinJS.log("You already bought this AddOn.", "samples", "error");
                    break;

                case StorePurchaseStatus.succeeded:
                    WinJS.log && WinJS.log("You bought " + item.title, "samples", "status");
                    break;

                case StorePurchaseStatus.notPurchased:
                    WinJS.log && WinJS.log("Product was not purchased, it may have been canceled.", "samples", "error");
                    break;

                case StorePurchaseStatus.networkError:
                    WinJS.log && WinJS.log("Product was not purchased due to a network error.", "samples", "error");
                    break;

                case StorePurchaseStatus.serverError:
                    WinJS.log && WinJS.log("Product was not purchased due to a server error.", "samples", "error");
                    break;

                default:
                    WinJS.log && WinJS.log("Product was not purchased due to an unknown error.", "samples", "error");
                    break;
            }
        });
    }
})();
