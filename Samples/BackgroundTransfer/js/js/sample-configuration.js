//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

(function () {
    "use strict";

    var sampleTitle = "BackgroundTransfer";
    var scenarios = [
        { url: "/html/scenario1_Download.html", title: "File Download" },
        { url: "/html/scenario2_Upload.html", title: "File Upload" },
        { url: "/html/scenario3_Notifications.html", title: "Completion Notifications" },
        { url: "/html/scenario4_CompletionGroups.html", title: "Completion Groups" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
