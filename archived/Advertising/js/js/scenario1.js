//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var adCount = 0;

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            var adControl = document.getElementById("adContainer").winControl;
            adControl.onErrorOccurred = errorHandler;
            adControl.onAdRefreshed = refreshHandler;
        }
    });

    // This is an error event handler for the ad control.
    function errorHandler(adControl, e) {
        WinJS.log && WinJS.log("An error occurred. " + e.errorCode + ": " + e.errorMessage, "samples", "error");
    }

    // This is an event handler for the ad control. It is called when the ad is refreshed with a new ad.
    function refreshHandler(adControl) {
        // We increment the ad count so that the message changes at every refresh.
        adCount++;
        WinJS.log && WinJS.log("Advertisement #" + adCount, "samples", "status");
    }
})();
