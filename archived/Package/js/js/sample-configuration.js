//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Package";

    var scenarios = [{
        url: "/html/scenario1_Identity.html",
        title: "Identity"
    }, {
        url: "/html/scenario2_InstalledLocation.html",
        title: "Installed Location"
    }, {
        url: "/html/scenario3_Dependencies.html",
        title: "Dependencies"
    }];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();