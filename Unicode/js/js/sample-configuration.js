//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Unicode JS sample";

    var scenarios = [
        { url: "/html/scenario1-findId.html", title: "Tokenize lexical identifiers within a string" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
