//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Sample Title here";

    var scenarios = [
        { url: "/html/1_NavToUrl.html", title: "Navigate to a URL" },
        { url: "/html/2_NavToString.html", title: "Navigate to a string" },
        { url: "/html/3_NavToState.html", title: "Navigate to app state" },
        { url: "/html/4_NavToStream.html", title: "Navigate to a stream" },
        { url: "/html/5_InvokeScript.html", title: "Interact with script" },
        { url: "/html/6_ScriptNotify.html", title: "Use ScriptNotify" },
        { url: "/html/7_SupportShare.html", title: "Share with other apps" },
        { url: "/html/8_PermissionRequest.html", title: "Manage permission requests" },
        { url: "/html/9_NewWindow.html", title: "Handle new window creation" },
        { url: "/html/10_AppWebviewComm.html", title: "App and Webview communication" },
        { url: "/html/11_UseWinrt.html", title: "Winrt APIs usage" }

    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();