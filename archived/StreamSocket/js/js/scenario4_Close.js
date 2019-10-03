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
            document.getElementById("buttonCloseSockets").addEventListener("click", closeListenerAndSockets, false);
        }
    });

    function closeListenerAndSockets(eventObject) {
        socketsSample.closing = true;
        if (socketsSample.listener !== null) {
            socketsSample.listener.close();
            socketsSample.listener = null;
        }
        if (socketsSample.serverSocket !== null) {
            socketsSample.serverSocket.close();
            socketsSample.serverSocket = null;
        }
        if (socketsSample.clientSocket !== null) {
            socketsSample.clientSocket.close();
            socketsSample.clientSocket = null;
            socketsSample.connected = false;
        }
        if (socketsSample.serverReader !== null) {
            socketsSample.serverReader.close();
            socketsSample.serverReader = null;
        }

        socketsSample.adapter = null;
        socketsSample.hostNameConnect = String(null);

        WinJS.log && WinJS.log("Socket and listener closed", "", "status");
    }
})();
