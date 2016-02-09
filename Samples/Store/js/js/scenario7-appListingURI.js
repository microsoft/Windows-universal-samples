//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;

    var page = WinJS.UI.Pages.define("/html/scenario7-appListingURI.html", {
        ready: function (element, options) {
            document.getElementById("displayLink").addEventListener("click", displayLink);
            return SdkSample.configureSimulatorAsync("app-listing-uri.xml");        
        }
    });

    function displayLink() {
        Windows.System.Launcher.launchUriAsync(new Windows.Foundation.Uri(CurrentAppSimulator.linkUri.absoluteUri));
    }
})();
