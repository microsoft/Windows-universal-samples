//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "DataReader/Writer Sample";

    var scenarios = [
        { url: "/html/scenario1_WriteReadStream.html", title: "Read and write simple structured data." },
        { url: "/html/scenario2_ReadBytes.html", title: "Dump file contents using readBytes()." }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();