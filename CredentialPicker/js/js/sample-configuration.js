//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Credential Picker";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Message" },
        { url: "/html/scenario2.html", title: "Message + Caption" },
        { url: "/html/scenario3.html", title: "Credential Picker Options" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();