//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Text Suggestion";

    var scenarios = [
        { url: "/html/Scenario1_Conversion.html", title: "Scenario 1 Conversion" },
        { url: "/html/Scenario2_Prediction.html", title: "Scenario 2 Prediction" },
        { url: "/html/Scenario3_ReverseConversion.html", title: "Scenario 3 ReverseConversion" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();