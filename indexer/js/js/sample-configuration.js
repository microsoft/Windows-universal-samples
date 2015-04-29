//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Indexer JS sample";

    var scenarios = [
        { url: "/html/addWithAPI.html", title: "Add item to the index using the ContentIndexer" },
        { url: "/html/updateAndDeleteWithAPI.html", title: "Update and delete indexed items using the ContentIndexer" },
        { url: "/html/retrieveWithAPI.html", title: "Retrieve indexed items added using the ContentIndexer" },
        { url: "/html/checkIndexRevision.html", title: "Check the index revision number" },
        { url: "/html/addWithAppContent.html", title: "Add indexed items by using appcontent-ms files" },
        { url: "/html/deleteWithAppContent.html", title: "Delete indexed appcontent-ms files" },
        { url: "/html/retrieveWithAppContent.html", title: "Retrieve indexed properties from appcontent-ms files" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
