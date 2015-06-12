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

    socketsSample.setValues = function () {
        var serviceNameAcceptInput = document.getElementById("serviceNameAccept");
        var hostNameConnectInput = document.getElementById("hostNameConnect");
        var serviceNameConnectInput = document.getElementById("serviceNameConnect");

        if (serviceNameAcceptInput !== null) {
            serviceNameAcceptInput.value = socketsSample.serviceNameAccept;
        }
        if (hostNameConnectInput !== null) {
            hostNameConnectInput.value = socketsSample.hostNameConnect;
        }
        if (serviceNameConnectInput !== null) {
            serviceNameConnectInput.value = socketsSample.serviceNameConnect;
        }
    };

    socketsSample.getValues = function (eventObject) {
        switch (eventObject.target.id) {
            case "serviceNameAccept":
                socketsSample.serviceNameAccept = eventObject.target.value;
                break;
            case "hostNameConnect":
                socketsSample.hostNameConnect = eventObject.target.value;
                break;
            case "serviceNameConnect":
                socketsSample.serviceNameConnect = eventObject.target.value;
                break;
        }
    };
})();
