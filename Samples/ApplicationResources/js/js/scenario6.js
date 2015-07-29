//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var output;

    var page = WinJS.UI.Pages.define("/html/scenario6.html", {
        ready: function (element, options) {
            output = document.getElementById('output');
            WinJS.Resources.processAll(output);
            WinJS.Resources.addEventListener("contextchanged", refresh, false);
        },
        unload: function () {
            WinJS.Resources.removeEventListener("contextchanged", refresh, false);
        }
    });

    function refresh(e) {

        WinJS.Resources.processAll(output); // Refetch string resources

        // If a page contains img elements and the images have scale or other variations,
        // the HTML layout engine does not automatically reload those images if the scale 
        // is changed (e.g., if a view is moved to a different display that has a 
        // different DPI). The call to WinJS.Resources.processAll() will reload strings 
        // resources bound using data-win-res attributes, but does not reload images. The 
        // following will reload images that are referenced using a URI with the ms-appx 
        // schema.

        // The contextchanged event can occur in relation to several different qualifiers.
        // If only certain qualifiers or qualifier values are relevant for an app, e.detail 
        // can be tested to filter for relevant changes. This sample has image variants for
        // scale and language, so we will test for those particular changes.

        if (e.detail.qualifier === "Language" || e.detail.qualifier === "Scale") {
            var imageElements = document.getElementsByTagName("img");
            for (var i = 0, l = imageElements.length; i < l; i++) {
                var previousSource = imageElements[i].src;
                var uri = new Windows.Foundation.Uri(document.location, previousSource);
                if (uri.schemeName === "ms-appx") {
                    imageElements[i].src = "";
                    imageElements[i].src = previousSource;
                }
            }
        }
    }
})();
