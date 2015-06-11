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

    var messageWebSocket;
    var messageWriter;

    var page = WinJS.UI.Pages.define("/html/scenario1_Download.html", {
        ready: function (element, options) {
            document.getElementById("startButton").addEventListener("click", startSend, false);
            document.getElementById("closeButton").addEventListener("click", closeSocket, false);
        },
        unload: function (eventObject) {
            closeSocket(eventObject);
        }
    });

    function startSend(eventObject) {
        if (document.getElementById("inputField").value === "") {
            WinJS.log && WinJS.log("Please specify text to send", "sample", "error");
            return;
        }

        if (!messageWebSocket) {
            // Set up the socket data format and callbacks
            var webSocket = new Windows.Networking.Sockets.MessageWebSocket();
            // Both utf8 and binary message types are supported. If utf8 is specified then the developer
            // promises to only send utf8 encoded data.
            webSocket.control.messageType = Windows.Networking.Sockets.SocketMessageType.utf8;
            webSocket.onmessagereceived = onMessageReceived;
            webSocket.onclosed = onClosed;

            // Validating the URI is required since it was received from an untrusted source (user
            // input). The URI is validated by calling validateAndCreateUri() that will throw an exception for
            // strings that are not valid WebSocket URIs.
            // Note that when enabling the text box users may provide URIs to machines in the LAN
            // or internet. In these cases the app requires the "Home or Work Networking" or
            // "Internet (Client)" capability respectively.
            var uri;
            try {
                uri = validateAndCreateUri(document.getElementById("serverAddress").value);
            } catch (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
                return;
            }

            WinJS.log && WinJS.log("Connecting to: " + uri.absoluteUri, "sample", "status");

            webSocket.connectAsync(uri).done(function () {

                WinJS.log && WinJS.log("Connected", "sample", "status");

                messageWebSocket = webSocket;
                // The default DataWriter encoding is utf8.
                messageWriter = new Windows.Storage.Streams.DataWriter(webSocket.outputStream);
                sendMessage();

            }, function (error) {
                var errorStatus = Windows.Networking.Sockets.WebSocketError.getStatus(error.number);
                if (errorStatus === Windows.Web.WebErrorStatus.cannotConnect ||
                    errorStatus === Windows.Web.WebErrorStatus.notFound ||
                    errorStatus === Windows.Web.WebErrorStatus.requestTimeout) {
                    WinJS.log && WinJS.log("Cannot connect to the server. Please make sure " +
                        "to run the server setup script before running the sample.", "sample", "error");
                } else {
                    WinJS.log && WinJS.log("Failed to connect: " + getError(error), "sample", "error");
                }
            });
        } else {
            WinJS.log && WinJS.log("Already Connected", "sample", "status");
            sendMessage();
        }
    }

    function onMessageReceived(eventArguments) {
        // The incoming message is already buffered.
        var dataReader = eventArguments.getDataReader();
        log("Message Received; Type: " + getMessageTypeName(eventArguments.messageType)
            + ", Bytes: " + dataReader.unconsumedBufferLength + ", Text: ");
        log(dataReader.readString(dataReader.unconsumedBufferLength));
    }

    function getMessageTypeName(messageType) {
        switch (messageType) {
            case Windows.Networking.Sockets.SocketMessageType.utf8:
                return "UTF-8";
            case Windows.Networking.Sockets.SocketMessageType.binary:
                return "Binary";
            default:
                return "Unknown";
        }
    }

    function sendMessage() {
        log("Sending message");
        messageWriter.writeString(document.getElementById("inputField").value);
        messageWriter.storeAsync().done("", sendError);
    }

    function sendError(error) {
        log("Send error: " + getError(error));
    }

    function onClosed(closedEventInfo) {
        log("Closed; Code: " + closedEventInfo.code + " Reason: " + closedEventInfo.reason);
        if (!messageWebSocket) {
            return;
        }

        closeSocketCore(Number(null), String(null));
    }

    function closeSocket(eventObject) {
        if (!messageWebSocket) {
            WinJS.log && WinJS.log("Not connected", "sample", "status");
            return;
        }

        WinJS.log && WinJS.log("Closing", "sample", "status");
        closeSocketCore(1000, "Closed due to user request.");
    }

    function closeSocketCore(closeCode, closeStatus) {
        if (closeCode && closeStatus) {
            messageWebSocket.close(closeCode, closeStatus);
        } else {
            messageWebSocket.close();
        }

        messageWebSocket = null;

        if (messageWriter) {
            messageWriter.close();
            messageWriter = null;
        }
    }

    function log(text) {
        var outputFiled = document.getElementById("outputField");
        if (outputFiled !== null) {
            outputFiled.innerHTML += text + "<br>";
        }
    }

})();
