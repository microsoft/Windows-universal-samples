//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Sample Title here";

    var scenarios = [
        { url: "/html/Scenario1_SpellingTextSuggestions.html", title: "Spelling and Text Suggestions" },
        { url: "/html/Scenario2_ScopedViews.html", title: "Scoped Views" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();