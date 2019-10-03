//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var storeContext = Windows.Services.Store.StoreContext.getDefault();

    var page = WinJS.UI.Pages.define("/html/scenario6-appListingURI.html", {
        ready: function (element, options) {
            document.getElementById("displayLink").addEventListener("click", displayLink);
        }
    });

    function displayLink() {
        storeContext.getStoreProductForCurrentAppAsync().then(function (result) {
            if (result.extendedError) {
                SdkSample.reportExtendedError(result.extendedError);
                return;
            }

            Windows.System.Launcher.launchUriAsync(result.product.linkUri);
        });
    }
})();
