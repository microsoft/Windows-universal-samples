//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Language Font Mapping";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Get font recommendations for UI elements" },
        { url: "/html/scenario2.html", title: "Get font recommendations for document content" },
        { url: "/html/scenario3.html", title: "Using scale values for a recommended font" },
        { url: "/html/scenario4.html", title: "LanguageFontGroup interaction with font fallback mechanisms" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();