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

    var page = WinJS.UI.Pages.define("/html/scenario4_Close.html", {
        ready: function (element, options) {
            document.getElementById("buttonClose").addEventListener("click", closeListenerAndSockets, false);
        }
    });

    function closeListenerAndSockets(eventObject) {
        socketsSample.close();
        socketsSample.hostNameConnect = String(null);
        WinJS.log && WinJS.log("Client and server closed.", "sample", "status");
    }
})();
