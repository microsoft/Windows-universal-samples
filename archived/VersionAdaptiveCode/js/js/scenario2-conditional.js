//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ApiInformation = Windows.Foundation.Metadata.ApiInformation;
    var statusBar;

    var page = WinJS.UI.Pages.define("/html/scenario2-conditional.html", {
        ready: function (element, options) {
            // If the StatusBar class exists, then show it.
            if (ApiInformation.isTypePresent("Windows.UI.ViewManagement.StatusBar")) {
                statusBar = Windows.UI.ViewManagement.StatusBar.getForCurrentView();
                statusBar.progressIndicator.text = "Hello, world";
                statusBar.showAsync();
            }
        },
        unload: function () {
            if (statusBar) {
                statusBar.hideAsync();
            }
        }
    });
})();
