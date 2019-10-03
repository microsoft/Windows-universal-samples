//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario2-disable.html", {
        ready: function (element, options) {
            // Disable screen capture when this page is loaded.
            Windows.UI.ViewManagement.ApplicationView.getForCurrentView().isScreenCaptureEnabled = false;
        },
        unload: function () {
            // Re-enable screen capture when this page is unloaded.
            Windows.UI.ViewManagement.ApplicationView.getForCurrentView().isScreenCaptureEnabled = true;
        }
    });
})();
