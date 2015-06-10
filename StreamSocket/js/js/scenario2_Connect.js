//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario2_Connect.html", {
        ready: function (element, options) {
            document.getElementById("ConnectSocket").addEventListener("click", openClient, false);
            document.getElementById("hostNameConnect").addEventListener("change", socketsSample.getValues, false);
            document.getElementById("serviceNameConnect").addEventListener("change", socketsSample.getValues, false);

            socketsSample.serviceNameConnect = "22112";
            socketsSample.setValues();
        }
    });

    function openClient() {
        if (socketsSample.clientSocket) {
            WinJS.log("Already have a client; call close to close the listener and the client.", "", "error");
            return;
        }

        var serviceName = document.getElementById("serviceNameConnect").value;
        if (serviceName === "") {
            WinJS.log("Please provide a service name.", "", "error");
            return;
        }

        // By default 'hostNameConnect' is disabled and host name validation is not required. When enabling the text
        // box validating the host name is required since it was received from an untrusted source (user input).
        // The host name is validated by catching exceptions thrown by the HostName constructor.
        // Note that when enabling the text box users may provide names for hosts on the Internet that require the
        // "Internet (Client)" capability.
        var hostName;
        try {
            hostName = new Windows.Networking.HostName(document.getElementById("hostNameConnect").value);
        } catch (error) {
            WinJS.log("Error: Invalid host name.", "", "error");
            return;
        }

        socketsSample.closing = false;
        socketsSample.clientSocket = new Windows.Networking.Sockets.StreamSocket();

        if (socketsSample.adapter === null) {
            WinJS.log("Connecting to: " + hostNameConnect.textContent, "", "status");
            socketsSample.clientSocket.connectAsync(hostName, serviceName).done(function () {
                WinJS.log("Connected", "", "status");
                socketsSample.connected = true;
            }, onError);
        } else {
            WinJS.log(
                "Connecting to: " + hostNameConnect.textContent + " using network adapter " + socketsSample.adapter.networkAdapterId,
                "",
                "status");

            // Connect to the server (in our case the listener we created in previous step)
            // limiting traffic to the same adapter that the user specified in the previous step.
            // This option will be overridden by interfaces with weak-host or forwarding modes enabled.
            socketsSample.clientSocket.connectAsync(
                hostName,
                serviceName,
                Windows.Networking.Sockets.SocketProtectionLevel.plainSocket,
                socketsSample.adapter).done(function () {
                    WinJS.log("Connected using network adapter " + socketsSample.adapter.networkAdapterId, "", "status");
                    socketsSample.connected = true;
            }, onError);
        }
    }

    function onError(reason) {
        socketsSample.clientSocket = null;

        // When we close a socket, outstanding async operations will be canceled and the
        // error callbacks called.  There's no point in displaying those errors.
        if (!socketsSample.closing) {
            WinJS.log(reason, "", "error");
        }
    }
})();
