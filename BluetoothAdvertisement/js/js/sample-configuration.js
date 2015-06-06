//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function() {
    "use strict";
    var sampleTitle = "Bluetooth Low Energy Advertisement";

    var scenarios = [
        { url: "/html/scenario1_Watcher.html", title: "Advertisement watcher" },
        { url: "/html/scenario2_Publisher.html", title: "Advertisement publisher" },
        { url: "/html/scenario3_BackgroundWatcher.html", title: "Background advertisement watcher" },
        { url: "/html/scenario4_BackgroundPublisher.html", title: "Background advertisement publisher" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();