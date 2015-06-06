//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Clipboard JS sample";

    var scenarios = [
        { url: "/html/text.html", title: "Copy and paste text" },
        { url: "/html/image.html", title: "Copy and paste an image" },
        { url: "/html/files.html", title: "Copy and paste files" },
        { url: "/html/other.html", title: "Other Clipboard operations" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
