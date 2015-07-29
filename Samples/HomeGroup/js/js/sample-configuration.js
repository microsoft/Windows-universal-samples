//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "HomeGroup JS sample";

    var scenarios = [
        { url: "/html/scenario1-pick.html", title: "Open the file picker at HomeGroup" },
        { url: "/html/scenario2-search.html", title: "Search HomeGroup" },
        { url: "/html/scenario3-streamVideo.html", title: "Stream video from HomeGroup" },
        { url: "/html/scenario4-searchUser.html", title: "Search a HomeGroup user" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
