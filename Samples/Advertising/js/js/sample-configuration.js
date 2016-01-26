//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Advertising JS sample";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Create an ad control in markup" },
        { url: "/html/scenario2.html", title: "Create an ad control programatically" },
        { url: "/html/scenario3.html", title: "Show an interstitial video ad" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();