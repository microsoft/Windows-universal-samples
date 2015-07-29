//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Application Resources";

    var scenarios = [
        { url: "/html/scenario1.html", title: "File resources" },
        { url: "/html/scenario2.html", title: "String resources in HTML" },
        { url: "/html/scenario3.html", title: "String resources in JavaScript" },
        { url: "/html/scenario4.html", title: "Resources in AppX manifest" },
        { url: "/html/scenario5.html", title: "Using additional resource files" },
        { url: "/html/scenario6.html", title: "Runtime changes/events" },
        { url: "/html/scenario7.html", title: "Working with web services" },
        { url: "/html/scenario8.html", title: "Resources in HTML and binding" },
        { url: "/html/scenario11.html", title: "Override context" },
        { url: "/html/scenario12.html", title: "Resources in web compartment" },
        { url: "/html/scenario13.html", title: "Multi-dimensional fallback" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();