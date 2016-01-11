//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

(function () {
    "use strict";

    var sampleTitle = "Camera Resolution JS sample";

    var scenarios = [
        { url: "/html/Scenario1_PreviewSettings.html", title: "Change camera preview settings" },
        { url: "/html/Scenario2_PhotoSettings.html", title: "Change preview and photo settings" },
        { url: "/html/Scenario3_AspectRatio.html", title: "Match aspect ratios" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
