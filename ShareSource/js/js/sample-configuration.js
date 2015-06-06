//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Share source JS sample";

    var scenarios = [
        { url: "/html/text.html", title: "Share text", applink: "ms-sdk-sharesourcejs:navigate?page=ShareText" },
        { url: "/html/weblink.html", title: "Share web link", applink: "ms-sdk-sharesourcejs:navigate?page=ShareWebLink" },
        { url: "/html/applicationlink.html", title: "Share application link", applink: "ms-sdk-sharesourcejs:navigate?page=ShareApplicationLink" },
        { url: "/html/image.html", title: "Share an image", applink: "ms-sdk-sharesourcejs:navigate?page=ShareImage" },
        { url: "/html/files.html", title: "Share files", applink: "ms-sdk-sharesourcejs:navigate?page=ShareFiles" },
        { url: "/html/delay-render.html", title: "Share delay rendered files", applink: "ms-sdk-sharesourcejs:navigate?page=ShareDeplayRenderedFiles" },
        { url: "/html/html.html", title: "Share HTML content", applink: "ms-sdk-sharesourcejs:navigate?page=ShareHTML" },
        { url: "/html/custom.html", title: "Share custom data", applink: "ms-sdk-sharesourcejs:navigate?page=ShareCustomData" },
        { url: "/html/fail.html", title: "Fail with display text", applink: "ms-sdk-sharesourcejs:navigate?page=ShareErrorMessage" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
