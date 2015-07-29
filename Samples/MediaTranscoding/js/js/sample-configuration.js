//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Media Transcoding";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Transcode using default presets" },
        { url: "/html/scenario2.html", title: "Transcode with custom settings" },
        { url: "/html/scenario3.html", title: "Trim and transcode" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();