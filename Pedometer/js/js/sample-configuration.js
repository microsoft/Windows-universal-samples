//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Sample Title here";

    var scenarios = [
        { url: "/html/scenario1_Events.html", title: "Events" },
        { url: "/html/scenario2_History.html", title: "History" },
        { url: "/html/scenario3_CurrentStepCount.html", title: "Current step count" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();