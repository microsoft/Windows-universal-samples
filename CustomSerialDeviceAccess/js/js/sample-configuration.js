//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Custom Serial Device Access";

    var scenarios = [
        { url: "/html/scenario1_connectDisconnect.html", title: "Connect/Disconnect" },
        { url: "/html/Scenario2_configureDevice.html", title: "Configure Device" },
        { url: "/html/Scenario3_readWrite.html", title: "Read/Write" },
        { url: "/html/Scenario4_events.html", title: "Events" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();