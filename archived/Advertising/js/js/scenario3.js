//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var interstitialAd = null;

    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            // Instantiate the ad
            interstitialAd = new MicrosoftNSJS.Advertising.InterstitialAd();

            // Attach the event handlers
            interstitialAd.onAdReady = adReadyHandler;
            interstitialAd.onErrorOccurredHandler = adErrorHandler;
            interstitialAd.onCancelled = adCancelledHandler;
            interstitialAd.onCompleted = adCompletedHandler;

            document.getElementById("runInterstitialAdButton").addEventListener("click", requestAdHandler);
        }
    });

    function requestAdHandler(e) {
        // Request the ad.
        // The application id and ad unit id are passed in here.
        // The applicationId and adUnitId can be obtained from Dev Center.
        // See "Monetize with Ads" at https://msdn.microsoft.com/en-us/library/windows/apps/mt170658.aspx
        interstitialAd.requestAd(MicrosoftNSJS.Advertising.InterstitialAdType.video, "d25517cb-12d4-4699-8bdc-52040c712cab", "11389925");
        WinJS.log && WinJS.log("Ad requested", "samples", "status");
    }

    // This is an event handler for the interstitial ad. It is triggered when the interstitial ad information has been downloaded and is ready to show.
    function adReadyHandler(adControl) {
        // The ad is ready, show it
        // In some cases you might choose to request the ad in advance and 
        // show it at the time of your choosing, rather than calling show()
        // in the adReady event handler. This will reduce the user's wait time for the ad.
        interstitialAd.show();
    }

    // This is an event handler for the interstitial ad. It is triggered when the interstitial ad has been cancelled.
    function adCancelledHandler(adControl) {
        WinJS.log && WinJS.log("Ad cancelled", "samples", "status");
    }

    // This is an event handler for the interstitial ad. It is triggered when the interstitial ad playback has completed.
    function adCompletedHandler(adControl) {
        WinJS.log && WinJS.log("Ad completed", "samples", "status");
    }

    // This is an error handler for the interstitial ad.
    function adErrorHandler(adControl, e) {
        WinJS.log && WinJS.log("An error occurred. " + e.errorCode + ": " + e.errorMessage, "samples", "error");
    }
})();
