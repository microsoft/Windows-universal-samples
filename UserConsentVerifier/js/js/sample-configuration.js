//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "User Consent Verification";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Check Consent Availability" },
        { url: "/html/scenario2.html", title: "Request Consent" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();