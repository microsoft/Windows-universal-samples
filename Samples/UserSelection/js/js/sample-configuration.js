//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "User Selection sample";

    var scenarios = [ // change the scenario names later when done with sample
        { url: "/html/scenario1.html", title: "All the attribute values - Parent unselectable" },
        { url: "/html/scenario4.html", title: "All attribute values - Parent selectable" },
        { url: "/html/scenario2.html", title: "Text Box default behavior  selectable" },
        { url: "/html/scenario3.html", title: "Making a text box unselectable" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();