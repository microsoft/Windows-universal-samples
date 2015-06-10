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
            document.getElementById("buttonOpen").addEventListener("click", openClient, false);
            document.getElementById("hostNameConnect").addEventListener("change", socketsSample.getValues, false);
            document.getElementById("serviceNameConnect").addEventListener("change", socketsSample.getValues, false);
            socketsSample.serviceNameConnect = "22112";
            socketsSample.setValues();
        }
    });

    function openClient(eventObject) {
        if (socketsSample.clientSocket !== null) {
            WinJS.log && WinJS.log(
                "Already have a client; call close to close the listener and the client.",
                "sample",
                "error");
            return;
        }

        var serviceName = document.getElementById("serviceNameConnect").value;
        var dontFragment = document.getElementById("dontFragment").winControl;

        if (serviceName === "") {
            WinJS.log && WinJS.log("Please provide a service name.", "sample", "error");
            return;
        }

        // By default 'hostNameConnect' is disabled and host name validation is not required. When enabling the text
        // box validating the host name is required since it was received from an untrusted source (user input).
        // The host name is validated by catching exceptions thrown by the HostName constructor.
        // Note that when enabling the text box users may provide names for hosts on the internet that require the
        // "Internet (Client)" capability.
        var hostName;
        try {
            hostName = new Windows.Networking.HostName(document.getElementById("hostNameConnect").value);
        } catch (error) {
            WinJS.log && WinJS.log("Error: Invalid host name.", "sample", "error");
            return;
        }

        socketsSample.closing = false;
        socketsSample.clientSocket = new Windows.Networking.Sockets.DatagramSocket();
        socketsSample.clientSocket.addEventListener("messagereceived", onMessageReceived);

        if (dontFragment.checked) {
            // Set the IP DF (Don't Fragment) flag.
            // This won't have any effect when running both client and server on localhost.
            socketsSample.clientSocket.control.dontFragment = true;
        }

        WinJS.log && WinJS.log("Client: connection started.", "sample", "status");
        socketsSample.clientSocket.connectAsync(hostName, serviceName).done(function () {
            WinJS.log && WinJS.log("Client: connection completed.", "sample", "status");
            socketsSample.connected = true;
        }, onError);
    }

    function onMessageReceived(eventArgument) {
        try {
            var messageLength = eventArgument.getDataReader().unconsumedBufferLength;
            var message = eventArgument.getDataReader().readString(messageLength);
            WinJS.log && WinJS.log("Client: receive message from server \"" + message + "\"", "sample", "status");
        } catch (exception) {
            status = Windows.Networking.Sockets.SocketError.getStatus(exception.number);
            if (status === Windows.Networking.Sockets.SocketErrorStatus.connectionResetByPeer) {
                WinJS.log && WinJS.log(
                    "Peer does not listen on the specific port. Please make sure that you run step 1 first " +
                    "or you have a server properly working on a remote server.",
                    "sample",
                    "error");
            } else {
                WinJS.log && WinJS.log("Error happened when receiving a datagram: " + exception.message, "sample", "error");
            }
        }
    }

    function onError(reason) {
        socketsSample.clientSocket = null;

        // When we close a socket, outstanding async operations will be canceled and the
        // error callbacks called.  There's no point in displaying those errors.
        if (!socketsSample.closing) {
            WinJS.log && WinJS.log(reason, "sample", "error");
        }
    }
})();
