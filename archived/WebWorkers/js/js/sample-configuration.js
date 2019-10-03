//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "WebWorkers Sample";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Basic Multithreaded JavaScript" },
        { url: "/html/scenario3.html", title: "Timers in Workers" },
        { url: "/html/scenario4.html", title: "Channel Messaging" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
