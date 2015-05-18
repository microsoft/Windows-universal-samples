//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Context menu JS sample";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Show context menu" },
        { url: "/html/scenario2.html", title: "Replace default context menu" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
