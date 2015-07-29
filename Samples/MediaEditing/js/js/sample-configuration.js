//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Media Editing";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Trimming and Saving a clip" },
        { url: "/html/scenario2.html", title: "Appending multiple clips" },
        { url: "/html/scenario3.html", title: "Adding background audio tracks" },
        { url: "/html/scenario4.html", title: "Adding overlays to a clip" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();