//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "AllJoyn Consumer Experiences";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Secure Consumer" },
        { url: "/html/scenario2.html", title: "Onboarding Consumer" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();