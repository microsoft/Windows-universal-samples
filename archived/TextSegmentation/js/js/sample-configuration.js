//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Text segmentation JS sample";

    var scenarios = [
        { url: "/html/scenario1-ExtractTextSegments.html", title: "Extract Text Segments" },
        { url: "/html/scenario2-GetCurrentTextSegmentFromIndex.html", title: "Get Current Text Segment From Index" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
