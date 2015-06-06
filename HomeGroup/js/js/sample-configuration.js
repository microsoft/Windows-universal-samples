//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "HomeGroup JS sample";

    var scenarios = [
        { url: "/html/OpenItemPickerAtHomegroup.html", title: "Open the file picker at HomeGroup" },
        { url: "/html/SearchHomegroup.html", title: "Search HomeGroup" },
        { url: "/html/StreamHomegroupVideo.html", title: "Stream video from Homegroup" },
        { url: "/html/AdvancedSearch.html", title: "Advanced search" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
