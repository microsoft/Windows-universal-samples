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

    var page = WinJS.UI.Pages.define("/html/scenario2_Connect.html", {
        ready: function (element, options) {
            document.getElementById("buttonConnectSocket").addEventListener("click", openClient, false);
            document.getElementById("hostNameConnect").addEventListener("change", socketsSample.getValues, false);
            document.getElementById("serviceNameConnect").addEventListener("change", socketsSample.getValues, false);

            socketsSample.serviceNameConnect = "22112";
            socketsSample.setValues();
        }
    });

    function openClient(eventObject) {
        if (socketsSample.clientSocket !== null) {
            WinJS.log && WinJS.log("Already have a client; call close to close the listener and the client.", "", "error");
            return;
        }

        var serviceName = document.getElementById("serviceNameConnect").value;
        if (serviceName === "") {
            WinJS.log && WinJS.log("Please provide a service name.", "", "error");
            return;
        }

        // By default 'hostNameConnect' is disabled and host name validation is not required. When enabling the text
        // box validating the host name is required since it was received from an untrusted source (user input).
        // The host name is validated by catching exceptions thrown by the HostName constructor.
        var hostName;
        try {
            hostName = new Windows.Networking.HostName(document.getElementById("hostNameConnect").value);
        } catch (error) {
            WinJS.log && WinJS.log("Error: Invalid host name.", "", "error");
            return;
        }

        socketsSample.closing = false;
        socketsSample.clientSocket = new Windows.Networking.Sockets.StreamSocket();

        // If necessary, tweak the socket's control options before carrying out the connect operation.
        // Refer to the StreamSocketControl class' MSDN documentation for the full list of control options.
        socketsSample.clientSocket.control.keepAlive = false;

        if (socketsSample.adapter === null) {
            WinJS.log && WinJS.log("Connecting to: " + hostName, "", "status");
            socketsSample.clientSocket.connectAsync(hostName, serviceName).done(function () {
                WinJS.log && WinJS.log("Connected", "", "status");
                socketsSample.connected = true;
            }, onError);
        } else {
            WinJS.log && WinJS.log(
                "Connecting to: " + hostName + " using network adapter " + socketsSample.adapter.networkAdapterId,
                "",
                "status");

            // Connect to the server (by default, the listener we created in the previous step)
            // limiting traffic to the same adapter that the user specified in the previous step.
            // This option will be overridden by interfaces with weak-host or forwarding modes enabled.
            socketsSample.clientSocket.connectAsync(
                hostName,
                serviceName,
                Windows.Networking.Sockets.SocketProtectionLevel.plainSocket,
                socketsSample.adapter).done(function () {
                    WinJS.log && WinJS.log("Connected using network adapter " + socketsSample.adapter.networkAdapterId, "", "status");
                    socketsSample.connected = true;
            }, onError);
        }
    }

    function onError(reason) {
        socketsSample.clientSocket = null;

        // When we close a socket, outstanding async operations will be canceled and the
        // error callbacks called.  There's no point in displaying those errors.
        if (!socketsSample.closing) {
            WinJS.log && WinJS.log(reason, "", "error");
        }
    }
})();
