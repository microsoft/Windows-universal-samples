//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Lock screen personalization JS sample";

    var scenarios = [
        { url: "/html/SetLockScreenImage.html", title: "Set lock screen image" },
        { url: "/html/AddImageFeedScenario.html", title: "Register lock screen slide show image feed" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
