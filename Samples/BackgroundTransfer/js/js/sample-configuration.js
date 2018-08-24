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
        { url: "/html/scenario4_CompletionGroups.html", title: "Completion Groups" },
        { url: "/html/scenario5_RandomAccess.html", title: "Random Access" },
        { url: "/html/scenario6_RecoverableErrors.html", title: "Recoverable Errors" },
        { url: "/html/scenario7_DownloadReordering.html", title: "Download Reordering" }
    ];

    // Look up the name for an enumeration member.
    function lookupEnumName(e, value) {
        for (var name in e) {
            if (e[name] === value) {
                return name;
            }
        }
        // No name available; just use the number.
        return value.toString();
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        lookupEnumName: lookupEnumName
    });
})();
