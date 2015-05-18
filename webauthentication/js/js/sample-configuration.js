//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Web Authentication";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Connect to Facebook Services" },
        { url: "/html/scenario2.html", title: "Connect to Twitter Services" },
        { url: "/html/scenario3.html", title: "Connect to Flickr Services" },
        { url: "/html/scenario4.html", title: "Connect to Google Services" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();