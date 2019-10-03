//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var adCount = 0;
    var adContainer;

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            adContainer = document.getElementById("adContainer");
            document.getElementById("createAdControlButton").addEventListener("click", createAdControl);
        }
    });

    function createAdControl(e) {
        e.srcElement.disabled = true;

        // Set the ad container's size and position.
        adContainer.style.width = '160px';
        adContainer.style.height = '600px';

        // Create the ad control and attach event handlers.
        // The applicationId and adUnitId can be obtained from Dev Center.
        // See "Monetize with Ads" at https://msdn.microsoft.com/en-us/library/windows/apps/mt170658.aspx
        var ad = new MicrosoftNSJS.Advertising.AdControl(adContainer, {
                applicationId: 'd25517cb-12d4-4699-8bdc-52040c712cab',
                adUnitId: '10043134',
                onErrorOccurred: errorHandler,
                onAdRefreshed: refreshHandler
            });
    }

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
