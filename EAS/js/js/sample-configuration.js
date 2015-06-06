//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "EAS";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Get Device Information" },
        { url: "/html/scenario2.html", title: "Check Compliance" },
        { url: "/html/scenario3.html", title: "Apply EAS Policy" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();