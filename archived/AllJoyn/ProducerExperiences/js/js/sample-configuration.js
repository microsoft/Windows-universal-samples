//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "AllJoyn Producer Experiences";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Secure Producer" },
        { url: "/html/scenario2.html", title: "Onboarding Producer" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();