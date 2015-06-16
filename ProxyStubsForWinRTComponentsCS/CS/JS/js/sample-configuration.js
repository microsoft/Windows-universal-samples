//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Proxy and stub generation for Windows Runtime components";

    var scenarios = [
        { url: "/html/scenario1-oven-client.html", title: "Using Custom Components" },
        { url: "/html/scenario2-custom-exception.html", title: "Handling Windows Runtime Exceptions" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();