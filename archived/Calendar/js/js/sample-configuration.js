//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Calendar JS sample";

    var scenarios = [
        { url: "/html/scenario1-data.html", title: "Display a calendar" },
        { url: "/html/scenario2-stats.html", title: "Retrieve calendar statistics" },
        { url: "/html/scenario3-enum.html", title: "Calendar enumeration and math" },
        { url: "/html/scenario4-unicodeExtensions.html", title: "Calendar with Unicode extensions in languages" },
        { url: "/html/scenario5-timeZone.html", title: "Calendar time zone support" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();