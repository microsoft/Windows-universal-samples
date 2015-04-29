//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Animation metrics JS sample";

    var scenarios = [
        { url: "/html/scenario1-metrics.html", title: "Retrieve Animation Metrics" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
