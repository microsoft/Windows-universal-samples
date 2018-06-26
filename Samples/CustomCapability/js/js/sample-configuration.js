//// Copyright (c) Microsoft Corporation. All rights reserved

(function() {
    "use strict";
    var sampleTitle = "CustomCapability JS Sample";

    var scenarios = [
        { url: "/html/Scenario1_MeteringData.html", title: "Connect to an NT Service" },
        { url: "/html/scenario2_deviceConnect.html", title: "Connecting to the Fx2 Device" },
        { url: "/html/scenario3_deviceIO.html", title: "Getting and Setting device properties" },
        { url: "/html/scenario4_deviceEvents.html", title: "Registering for device events" },
        { url: "/html/scenario5_deviceReadWrite.html", title: "Read and Write operations" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();