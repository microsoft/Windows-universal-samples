//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Logging";

    var scenarios = [
        { url: "/html/scenario1.html", title: "LoggingChannel" },
        { url: "/html/scenario2.html", title: "LoggingSession" },
        { url: "/html/scenario3.html", title: "FileLoggingSession" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();