//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Packaged Content";

    var scenarios = [
        { url: "/html/navigationErrors.html", title: "Navigation Errors" },
        { url: "/html/referencingPackagedContent.html", title: "Referencing Packaged Content" },
        { url: "/html/xmlHttpRequest.html", title: "Using XMLHttpRequest (XHR)" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();