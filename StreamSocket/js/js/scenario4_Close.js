//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario4_Close.html", {
        ready: function (element, options) {
            document.getElementById("CloseSockets").addEventListener("click", closeListenerAndSockets, false);
        }
    });

    function closeListenerAndSockets() {
        socketsSample.closing = true;
        if (socketsSample.listener) {
            socketsSample.listener.close();
            socketsSample.listener = null;
        }
        if (socketsSample.serverSocket) {
            socketsSample.serverSocket.close();
            socketsSample.serverSocket = null;
        }
        if (socketsSample.clientSocket) {
            socketsSample.clientSocket.close();
            socketsSample.clientSocket = null;
            socketsSample.connected = false;
        }
        if (socketsSample.serverReader) {
            socketsSample.serverReader.close();
            socketsSample.serverReader = null;
        }

        socketsSample.adapter = null;
        socketsSample.hostNameConnect = null;

        WinJS.log("Socket and listener closed", "", "status");
    }
})();
