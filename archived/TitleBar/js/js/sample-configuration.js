//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Title bar";

    var scenarios = [
        { url: "/html/scenario1-colors.html", title: "Custom colors" },
        { url: "/html/scenario2-extend.html", title: "Custom drawing" },
    ];

    // We insert the title bar as the first element in the body.
    var customTitleBar;
    var customTitleBarPromise = WinJS.Promise.wrap();

    function addCustomTitleBar() {
        // Wait for the previous operation to complete before starting a new one.
        customTitleBarPromise = customTitleBarPromise.then(function() {
            if (!customTitleBar) {
                var host = document.createElement("div");
                document.body.insertBefore(host, document.body.childNodes[0]);
                return WinJS.UI.Pages.render("/html/customTitleBar.html", host).then(function (result) {
                    customTitleBar = result;
                });
            }
        });
        return customTitleBarPromise;
    }

    function removeCustomTitleBar() {
        // Wait for the previous operation to complete before starting a new one.
        customTitleBarPromise = customTitleBarPromise.then(function () {
            if (customTitleBar) {
                customTitleBar.unload();
                document.body.removeChild(customTitleBar.element);
                customTitleBar = null;
            }
        });
        return customTitleBarPromise;
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        addCustomTitleBar: addCustomTitleBar,
        removeCustomTitleBar: removeCustomTitleBar,
    });
})();