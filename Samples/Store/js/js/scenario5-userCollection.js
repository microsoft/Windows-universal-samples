//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var storeContext = Windows.Services.Store.StoreContext.getDefault();

    var productsListView;

    var page = WinJS.UI.Pages.define("/html/scenario5-userCollection.html", {
        ready: function (element, options) {
            productsListView = document.getElementById('productsListView').winControl;

            document.getElementById("getUserCollectionButton").addEventListener("click", getUserCollectionButton);
        }
    });

    function getUserCollectionButton() {
        var productKinds = ["Consumable", "Durable", "UnmanagedConsumable"];
        storeContext.getUserCollectionAsync(productKinds).then(function (addOns) {
            var productList = SdkSample.createProductListFromQueryResult(addOns, "collection items");
            productsListView.itemDataSource = productList.dataSource;
        });
    }
})();
