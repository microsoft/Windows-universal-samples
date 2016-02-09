//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Globalization preferences JS sample";

    var scenarios = [
        { url: "/html/scenario1-prefs.html", title: "User preferences" },
        { url: "/html/scenario2-lang.html", title: "Language characteristics" },
        { url: "/html/scenario3-region.html", title: "Region characteristics" },
        { url: "/html/scenario4-input.html", title: "Current input language" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();