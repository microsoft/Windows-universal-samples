//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "PrintSample";

    var scenarios = [
        { url: "/html/scenario1basic.html", title: "Basic" },
        { url: "/html/scenario2standardoptions.html", title: "Standard Options" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();