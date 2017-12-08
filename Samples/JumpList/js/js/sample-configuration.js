//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Resize app view";

    var scenarios = [
        { url: "/html/scenario1-launched.html", title: "Respond to being launched" },
        { url: "/html/scenario2-remove.html", title: "Remove the jump list" },
        { url: "/html/scenario3-customitems.html", title: "Creating custom items" },
        { url: "/html/scenario4-changesystemgroup.html", title: "Change the system group" },
        { url: "/html/scenario5-issupported.html", title: "Check if the platform is supported" },
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();