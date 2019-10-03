//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function() {
    "use strict";
    var sampleTitle = "Bluetooth Low Energy Advertisement";

    var scenarios = [
        { url: "/html/scenario1_Watcher.html", title: "Foreground watcher" },
        { url: "/html/scenario2_Publisher.html", title: "Foreground publisher" },
        { url: "/html/scenario3_BackgroundWatcher.html", title: "Background watcher" },
        { url: "/html/scenario4_BackgroundPublisher.html", title: "Background publisher" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();