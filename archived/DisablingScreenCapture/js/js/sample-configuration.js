//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Disabling screen capture JS sample";

    var scenarios = [
        { url: "/html/scenario1-enable.html", title: "Enable screen capture" },
        { url: "/html/scenario2-disable.html", title: "Disable screen capture" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();