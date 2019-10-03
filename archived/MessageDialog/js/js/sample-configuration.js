//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Message dialog JS sample";

    var scenarios = [
        { url: "/html/customcommand.html", title: "Use custom commands" },
        { url: "/html/defaultclosecommand.html", title: "Use default close command" },
        { url: "/html/completedcallback.html", title: "Use completed callback" },
        { url: "/html/cancelcommand.html", title: "Use cancel command" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
