//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Store JS sample";

    var scenarios = [
        { url: "/html/scenario1-trialMode.html", title: "Trial-mode" },
        { url: "/html/scenario2-inAppPurchase.html", title: "In-app purchase" },
        { url: "/html/scenario3-unmanagedConsumable.html", title: "Unmanaged consumable product" },
        { url: "/html/scenario4-consumableProduct.html", title: "Managed consumable product" },
        { url: "/html/scenario5-userCollection.html", title: "User collection" },
        { url: "/html/scenario6-appListingURI.html", title: "App listing URI" },
        { url: "/html/scenario7-b2b.html", title: "Business to Business" },
    ];

    var loggingChannel;
    function generateGuid() {
        loggingChannel = loggingChannel || new Windows.Foundation.Diagnostics.LoggingChannel("GUID Creator");

        var loggingActivity = new Windows.Foundation.Diagnostics.LoggingActivity("Make-A-GUID", loggingChannel);
        var newGuid = loggingActivity.id;
        loggingActivity.close();
        return newGuid;
    }

    function createProductListFromQueryResult(addOns, description) {
        var productList = new WinJS.Binding.List();

        if (addOns.extendedError) {
            SdkSample.reportExtendedError(addOns.extendedError);
        } else if (addOns.products.size === 0) {
            WinJS.log && WinJS.log(`No configured ${description} found for this Store Product.`, "samples", "error");
        } else {
            Object.values(addOns.products).forEach(function (product) {
                var item = {
                    title: product.title,
                    price: product.price.formattedPrice,
                    inCollection: product.isInUserCollection,
                    productKind: product.productKind,
                    storeId: product.storeId
                };

                item.formattedTitle = `${item.title} (${item.productKind}) ${item.price}, InUserCollection: ${item.inCollection}`;
                productList.push(item);
            });
        }
        return productList;
    }

    function reportExtendedError(extendedError) {
        var message;
        if (extendedError === (0x803f6107 | 0)) {
            message = "This sample has not been properly configured. See the README for instructions.";
        } else {
            // The user may be offline or there might be some other server failure.
            message = `ExtendedError: ${extendedError}`;
        }
        WinJS.log && WinJS.log(message, "sample", "error");
    }

    function enableWhenOneItemIsSelected(element, listview) {
        function selectionChangedHandler() {
            element.disabled = (listview.selection.count() != 1);
        }

        listview.addEventListener("selectionchanged", selectionChangedHandler);
        selectionChangedHandler();
    }

    function addSingleItemClickHandler(id, listview, clickHandler) {
        var element = document.getElementById(id);
        element.addEventListener("click", function () {
            listview.selection.getItems().done(function (items) {
                clickHandler(items[0].data);
            });
        });
        enableWhenOneItemIsSelected(element, listview);
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        generateGuid: generateGuid,
        createProductListFromQueryResult: createProductListFromQueryResult,
        reportExtendedError: reportExtendedError,
        enableWhenOneItemIsSelected: enableWhenOneItemIsSelected,
        addSingleItemClickHandler: addSingleItemClickHandler,
    });
})();