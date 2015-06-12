//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Wi-Fi Direct";

    var scenarios = [
        { url: "/html/scenario1_advertiser.html", title: "Advertiser" },
        { url: "/html/scenario2_connector.html", title: "Connector" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();