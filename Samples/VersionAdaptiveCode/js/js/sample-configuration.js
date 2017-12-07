//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Version adaptive code JS sample";

    var scenarios = [
        { url: "/html/scenario1-basic.html", title: "Basics" },
        { url: "/html/scenario2-conditional.html", title: "Conditional execution" },
        { url: "/html/scenario3-chooseControl.html", title: "Choosing controls" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();