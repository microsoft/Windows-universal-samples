//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Full screen mode";

    var scenarios = [
        { url: "/html/scenario1-basic.html", title: "Entering and exiting full screen mode" },
        { url: "/html/scenario2-launch.html", title: "Launching into full screen mode" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();