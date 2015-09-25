//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Password Vault";

    var scenarios = [
        { url: "/html/scenario1-save.html", title: "Save credentials" },
        { url: "/html/scenario2-manage.html", title: "Manage credentials" }
    ];

    // Preload the password vault in the background.
    new Worker("js/backgroundInit.js");

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();