//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "StreamSocket";

    var scenarios = [
        { url: "/html/scenario1_Start.html", title: "Start StreamSocketListener" },
        { url: "/html/scenario2_Connect.html", title: "Connect to Listener" },
        { url: "/html/scenario3_Send.html", title: "Send Data" },
        { url: "/html/scenario4_Close.html", title: "Close Socket" },
        { url: "/html/scenario5_Certificates.html", title: "Handle Server Certificates" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();