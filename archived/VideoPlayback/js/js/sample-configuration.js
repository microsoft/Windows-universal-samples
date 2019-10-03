//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Video Playback JS";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Playing Videos" },
        { url: "/html/scenario2.html", title: "Displaying in-band closed captions (MKV)" },
        { url: "/html/scenario3.html", title: "Displaying out-of-band closed captions (MP4+WebVTT)" },
        { url: "/html/scenario4.html", title: "Selecting video tracks" },
        { url: "/html/scenario5.html", title: "Selecting audio tracks" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();