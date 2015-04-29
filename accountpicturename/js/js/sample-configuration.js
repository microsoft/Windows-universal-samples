//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Account picture name JS sample";

    var scenarios = [
        { url: "/html/userDisplayName.html", title: "Get display name" },
        { url: "/html/userFirstAndLastName.html", title: "Get first and last name" },
        { url: "/html/getAccountPicture.html", title: "Get account picture" },
        { url: "/html/setAccountPicture.html", title: "Set account picture and listen for changes" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();

