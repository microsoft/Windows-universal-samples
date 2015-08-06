//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Account picture name JS sample";

    var scenarios = [
        { url: "/html/scenario1-findUsers.html", title: "Find users" },
        { url: "/html/scenario2-watchUsers.html", title: "Watch users" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
