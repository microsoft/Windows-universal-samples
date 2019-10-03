//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Folder enumeration JS sample";

    var scenarios = [
        { url: "/html/scenario1.html", title: "Enumerate files and folders in the Pictures library" },
        { url: "/html/scenario2.html", title: "Enumerate files in the Pictures library, by groups" },
        { url: "/html/scenario3.html", title: "Enumerate files in the Pictures library with prefetch APIs" },
        { url: "/html/scenario4.html", title: "Enumerate files in a folder and display availability" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
