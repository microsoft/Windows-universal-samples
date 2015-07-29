//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Adaptive Streaming";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Basic HLS/DASH Playback" },
        { url: "/html/scenario2.html", title: "Configuring HLS/DASH Playback" },
        { url: "/html/scenario3.html", title: "Customized Resource Acquisition" },
        { url: "/html/scenario4.html", title: "Playback with PlayReady DRM" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();