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

    var sampleTitle = "CameraProfile";

    var scenarios = [
        { url: "/html/Scenario1_SetRecordProfile.html", title: "Locate Record Specific Profile" },
        { url: "/html/Scenario2_ConcurrentProfile.html", title: "Query Profile for Concurrency" },
        { url: "/html/Scenario3_EnableHdrProfile.html", title: "Query Profile for HDR Support" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
