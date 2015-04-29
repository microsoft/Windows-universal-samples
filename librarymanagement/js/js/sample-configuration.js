//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Library management JS sample";

    var scenarios = [
        { url: "/html/S1_AddFolder.html",    title: "Adding a folder to the Pictures library" },
        { url: "/html/S2_ListFolders.html",  title: "Listing folders in the Pictures library" },
        { url: "/html/S3_RemoveFolder.html", title: "Removing a folder from the Pictures library" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
