//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Simple imaging JS sample";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Image properties (FileProperties)" },
        { url: "/html/scenario2.html", title: "Image transforms/encode (BitmapDecoder)" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();