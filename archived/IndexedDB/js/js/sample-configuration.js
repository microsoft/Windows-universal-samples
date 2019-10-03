//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "IndexedDB";

    var scenarios = [
        { url: "/html/createschema.html", title: "Create IndexedDB Schema" },
        { url: "/html/populatedata.html", title: "Populate Initial Data" },
        { url: "/html/readdata.html", title: "Reading Data" },
        { url: "/html/writedata.html", title: "Writing Data" }
    ];

    var db = null;

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();