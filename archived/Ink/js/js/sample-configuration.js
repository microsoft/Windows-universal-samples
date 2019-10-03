//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Ink sample";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Scenario 1" },
        { url: "/html/scenario2.html", title: "Scenario 2" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();