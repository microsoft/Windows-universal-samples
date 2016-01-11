//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Store JS sample";

    var scenarios = [
        { url: "/html/scenario1-trialMode.html", title: "Trial-mode" },
        { url: "/html/scenario2-inAppPurchase.html", title: "In-app purchase" },
        { url: "/html/scenario3-expiringProduct.html", title: "Expiring product" },
        { url: "/html/scenario4-consumableProduct.html", title: "Consumable product" },
        { url: "/html/scenario5-advancedConsumableProduct.html", title: "Advanced consumable product" },
        { url: "/html/scenario6-largeAppCatalog.html", title: "Large catalog product" },
        { url: "/html/scenario7-appListingURI.html", title: "App listing URI" },
        { url: "/html/scenario8-receipt.html", title: "Receipt" },
        { url: "/html/scenario9-b2b.html", title: "Business to Business" },
    ];

    function configureSimulatorAsync(filename) {
        return Windows.ApplicationModel.Package.current.installedLocation.getFileAsync("data\\" + filename).then(function (proxyFile) {
            return Windows.ApplicationModel.Store.CurrentAppSimulator.reloadSimulatorAsync(proxyFile);
        });
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        configureSimulatorAsync: configureSimulatorAsync
    });
})();