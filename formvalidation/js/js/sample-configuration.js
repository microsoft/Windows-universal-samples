//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Form Validation";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Creating a form" },
        { url: "/html/scenario2.html", title: "Validating user input" },
        { url: "/html/scenario3.html", title: "Creating a custom error experience" },
        { url: "/html/scenario4.html", title: "Using validation selectors" },
        { url: "/html/scenario5.html", title: "Submitting via FormData + XHR" }

    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();