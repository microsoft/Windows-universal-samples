//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "OCR JS sample";

    var scenarios = [
        { url: "/html/scenario1-file-image.html", title: "OCR image file" },
        { url: "/html/scenario2-camera-image.html", title: "OCR captured image" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();