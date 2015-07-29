//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Resize app view";

    var scenarios = [
        { url: "/html/scenario1-basic.html", title: "Resizing the view" },
        { url: "/html/scenario2-launch.html", title: "Launching at a custom size" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();