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

    var listenerSocket = null;

    var page = WinJS.UI.Pages.define("/html/scenario5_MulticastAndBroadcast.html", {
        ready: function (element, options) {
            document.getElementById("buttonStartListener").addEventListener("click", startListener, false);
            document.getElementById("buttonSendMessage").addEventListener("click", sendMessage, false);
            document.getElementById("buttonCloseListener").addEventListener("click", closeListener, false);
            document.getElementById("radioMulticast").addEventListener("change", scenarioRadioButtonChanged, false);
            document.getElementById("radioBroadcast").addEventListener("change", scenarioRadioButtonChanged, false);

            scenarioRadioButtonChanged(null);
        }
    });

    function closeListenerSocket() {
        if (listenerSocket !== null) {
            // The call below explicitly closes the socket, freeing the UDP port that it is currently bound to.
            listenerSocket.close();
            listenerSocket = null;
        }
    }

    function setupMulticastScenarioUI() {
        document.getElementById("labelRemoteAddress").innerHTML = "Multicast Group:";
        document.getElementById("buttonStartListener").innerHTML = "Start listener and join multicast group";
        document.getElementById("inputServiceName").value = "22113";
        document.getElementById("inputRemoteAddress").value = "224.3.0.5";
        document.getElementById("inputRemoteAddress").disabled = true;
        document.getElementById("buttonSendMessage").disabled = true;
        document.getElementById("buttonCloseListener").disabled = true;
        document.getElementById("labelSendOutput").innerHTML = "";
    }

    function setupBroadcastScenarioUI() {
        document.getElementById("labelRemoteAddress").innerHTML = "Broadcast Address:";
        document.getElementById("buttonStartListener").innerHTML = "Start listener";
        document.getElementById("inputServiceName").value = "22113";
        document.getElementById("inputRemoteAddress").value = "255.255.255.255";
        document.getElementById("inputRemoteAddress").disabled = true;
        document.getElementById("buttonSendMessage").disabled = true;
        document.getElementById("buttonCloseListener").disabled = true;
        document.getElementById("labelSendOutput").innerHTML = "";
    }

    function scenarioRadioButtonChanged(eventObject) {
        closeListenerSocket();
        if (document.getElementById("radioMulticast").checked) {
            setupMulticastScenarioUI();
        } else {
            setupBroadcastScenarioUI();
        }
    }

    function startListener(eventObject) {
        var serviceName = document.getElementById("inputServiceName").value;

        if (serviceName === "") {
            WinJS.log && WinJS.log("Please provide a service name.", "sample", "error");
            return;
        }

        if (listenerSocket !== null) {
            WinJS.log && WinJS.log("A listener socket is already set up.", "sample", "error");
            return;
        }

        var isMulticastSocket = document.getElementById("radioMulticast").checked;
        listenerSocket = new Windows.Networking.Sockets.DatagramSocket();
        listenerSocket.addEventListener("messagereceived", onMessageReceived);

        if (isMulticastSocket) {
            // DatagramSockets conduct exclusive (SO_EXCLUSIVEADDRUSE) binds by default, effectively blocking
            // any other UDP socket on the system from binding to the same local port. This is done to prevent
            // other applications from eavesdropping or hijacking a DatagramSocket's unicast traffic.
            //
            // Setting the MulticastOnly control option to 'true' enables a DatagramSocket instance to share its
            // local port with any Win32 sockets that are bound using SO_REUSEADDR/SO_REUSE_MULTICASTPORT and
            // with any other DatagramSocket instances that have MulticastOnly set to true. However, note that any
            // attempt to use a multicast-only DatagramSocket instance to send or receive unicast data will result
            // in an exception being thrown.
            //
            // This control option is particularly useful when implementing a well-known multicast-based protocol,
            // such as mDNS and UPnP, since it enables a DatagramSocket instance to coexist with other applications
            // running on the system that also implement that protocol.
            listenerSocket.control.multicastOnly = true;
        }

        // Start listen operation.
        listenerSocket.bindServiceNameAsync(serviceName).done(function () {
            if (isMulticastSocket) {
                try {
                    var multicastGroup = document.getElementById("inputRemoteAddress").value;
                    listenerSocket.joinMulticastGroup(new Windows.Networking.HostName(multicastGroup));

                    WinJS.log && WinJS.log(
                        "Listening on port " + listenerSocket.information.localPort + " and joined to multicast group.",
                        "sample",
                        "status");
                }
                catch (exception) {
                    onError("Error while joining multicast group: " + exception.message);
                    return;
                }
            } else {
                WinJS.log && WinJS.log("Listening on port " + listenerSocket.information.localPort, "sample", "status");
            }

            // Enable scenario steps that require us to have an active listening socket.
            document.getElementById("buttonSendMessage").disabled = false;
            document.getElementById("buttonCloseListener").disabled = false;
        }, onError);
    }

    function sendMessage(eventObject) {
        document.getElementById("labelSendOutput").innerHTML = "";

        try {
            var remoteHostname = new Windows.Networking.HostName(document.getElementById("inputRemoteAddress").value);
            var serviceName = document.getElementById("inputServiceName").value;

            // GetOutputStreamAsync can be called multiple times on a single DatagramSocket instance to obtain
            // IOutputStreams pointing to various different remote endpoints. The remote hostname given to
            // GetOutputStreamAsync can be a unicast, multicast or broadcast address.
            listenerSocket.getOutputStreamAsync(remoteHostname, serviceName).done(function (outputStream) {
                try {
                    // Send out some multicast or broadcast data. Datagrams generated by the IOutputStream will use
                    // <source host, source port> information obtained from the parent socket (i.e., 'listenSocket' in
                    // this case).
                    var stringToSend = "Hello";
                    var writer = new Windows.Storage.Streams.DataWriter(outputStream);
                    writer.writeString(stringToSend);
                    writer.storeAsync().done(function () {
                        document.getElementById("labelSendOutput").innerHTML = "\"" + stringToSend + "\" sent successfully.";
                    }, onError);
                }
                catch (exception) {
                    onError("Error sending message: " + exception.message);
                }
            }, onError);
        }
        catch (exception) {
            onError("Error sending message: " + exception.message);
        }
    }

    function closeListener(eventObject) {
        closeListenerSocket();

        // Disable scenario steps that require us to have an active listening socket.
        document.getElementById("buttonSendMessage").disabled = true;
        document.getElementById("buttonCloseListener").disabled = true;
        document.getElementById("labelSendOutput").innerHTML = "";

        WinJS.log && WinJS.log("Listener closed", "sample", "status");
    }

    function onMessageReceived(eventArguments) {
        try {
            // Interpret the incoming datagram's entire contents as a string.
            var stringLength = eventArguments.getDataReader().unconsumedBufferLength;
            var receivedMessage = eventArguments.getDataReader().readString(stringLength);

            WinJS.log && WinJS.log(
                "Received data from remote peer (Remote Address: " +
                    eventArguments.remoteAddress.canonicalName +
                    ", Remote Port: " +
                    eventArguments.remotePort + "): \"" +
                    receivedMessage + "\"",
                "sample",
                "status");
        } catch (exception) {
            WinJS.log && WinJS.log("Error happened when receiving a datagram: " + exception.message, "sample", "error");
        }
    }

    function onError(reason) {
        closeListenerSocket();
        WinJS.log && WinJS.log(reason, "sample", "error");
    }

})();
