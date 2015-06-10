//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

var socketsSample = {};

(function () {
    "use strict";

    socketsSample.listener = null; // A StreamSocketListener that acts as our server.
    socketsSample.adapter = null; // Limit traffic to the same adapter that the listener is using.
    socketsSample.serverSocket = null; // The server socket that's been accepted.
    socketsSample.serverReader = null; // The reader for the server socket.
    socketsSample.clientSocket = null; // The client socket that will connect to the server socket.
    socketsSample.connected = false;
    socketsSample.closing = false;
    socketsSample.bindingToService = false;

    socketsSample.serviceNameAccept = "22112";
    socketsSample.hostNameConnect = "localhost";
    socketsSample.serviceNameConnect = "22112";

    socketsSample.displayOutput = function (message) {
        document.getElementById("outputBox").innerHTML = message;
    };

    socketsSample.setValues = function () {
        var serviceNameAcceptInput = document.getElementById("serviceNameAccept");
        var hostNameConnectInput = document.getElementById("hostNameConnect");
        var serviceNameConnectInput = document.getElementById("serviceNameConnect");

        if (serviceNameAcceptInput) {
            serviceNameAcceptInput.value = socketsSample.serviceNameAccept;
        }
        if (hostNameConnectInput) {
            hostNameConnectInput.value = socketsSample.hostNameConnect;
        }
        if (serviceNameConnectInput) {
            serviceNameConnectInput.value = socketsSample.serviceNameConnect;
        }
    };

    socketsSample.getValues = function (evt) {
        switch (evt.target.id) {
            case "serviceNameAccept":
                socketsSample.serviceNameAccept = evt.target.value;
                break;
            case "hostNameConnect":
                socketsSample.hostNameConnect = evt.target.value;
                break;
            case "serviceNameConnect":
                socketsSample.serviceNameConnect = evt.target.value;
                break;
        }
    };
})();
