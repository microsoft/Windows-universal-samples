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

    var MessageWebSocket = Windows.Networking.Sockets.MessageWebSocket;
    var SocketMessageType = Windows.Networking.Sockets.SocketMessageType;
    var UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding;

    // Local variables
    var messageWebSocket;
    var messageWriter;
    var busy;

    // DOM elements
    var serverAddressField;
    var connectButton;
    var disconnectButton;
    var inputField;
    var sendButton;
    var outputField;

    var page = WinJS.UI.Pages.define("/html/scenario1-utf8.html", {
        ready: function (element, options) {
            serverAddressField = document.getElementById("serverAddressField");
            connectButton = document.getElementById("connectButton");
            disconnectButton = document.getElementById("disconnectButton");
            inputField = document.getElementById("inputField");
            sendButton = document.getElementById("sendButton");
            outputField = document.getElementById("outputField");

            connectButton.addEventListener("click", onConnect, false);
            disconnectButton.addEventListener("click", onDisconnect, false);
            sendButton.addEventListener("click", onSend, false);

            updateVisualState();
        },
        unload: function (eventObject) {
            closeSocket();
        }
    });

    function updateVisualState() {
        serverAddressField.disabled = busy || messageWebSocket;
        connectButton.disabled = busy || messageWebSocket;
        disconnectButton.disabled = busy || !messageWebSocket;
        inputField.disabled = busy || !messageWebSocket
        sendButton.disabled = busy || !messageWebSocket
    }

    function setBusy(value) {
        busy = value;
        updateVisualState();
    }

    function onConnect() {
        setBusy(true);
        connectAsync().done(function () {
            setBusy(false);
        });
    }

    function connectAsync() {
        // Validating the URI is required since it was received from an untrusted source (user
        // input). The URI is validated by calling validateAndCreateUri() that will return null
        // for strings that are not valid WebSocket URIs.
        // Note that when enabling the text box users may provide URIs to machines on the local network
        // or internet. In these cases the app requires the "Home or Work Networking" or
        // "Internet (Client)" capability respectively.
        var server = SdkSample.validateAndCreateUri(serverAddressField.value);
        if (!server)
        {
            return WinJS.Promise.wrap();
        }

        // Set up the socket data format and callbacks
        messageWebSocket = new MessageWebSocket();
        messageWebSocket.control.messageType = SocketMessageType.utf8;
        messageWebSocket.addEventListener("messagereceived", onMessageReceived);
        messageWebSocket.addEventListener("closed", onClosed);

        appendOutputLine("Connecting to: " + server.absoluteUri);

        return messageWebSocket.connectAsync(server).then(function () {
            // The default DataWriter encoding is utf8.
            messageWriter = new Windows.Storage.Streams.DataWriter(messageWebSocket.outputStream);
            WinJS.log && WinJS.log("Connected", "sample", "status");
        }, function (error) {
            messageWebSocket.close();
            messageWebSocket = null;

            appendOutputLine(SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        });
    }

    function onSend() {
        setBusy(true);
        sendAsync().done(function () {
            setBusy(false);
        });
    }

    function sendAsync() {
        var message = inputField.value;
        if (message === "") {
            WinJS.log && WinJS.log("Please specify text to send", "sample", "error");
            return WinJS.Promise.wrap();
        }

        appendOutputLine("Sending Message: " + message);

        // Buffer any data we want to send.
        messageWriter.writeString(message);

        // Send the data as one complete message.
        return messageWriter.storeAsync().then(function() {
            WinJS.log && WinJS.log("Send Complete", "sample", "status");
        }, function(error) {
            appendOutputLine(SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        });
    }

    function onMessageReceived(args) {
        appendOutputLine("Message Received; Type: " + SdkSample.lookupEnumName(SocketMessageType, args.messageType));

        // The incoming message is already buffered.
        var reader = args.getDataReader();
        reader.unicodeEncoding = UnicodeEncoding.utf8;

        try {
            appendOutputLine(reader.readString(reader.unconsumedBufferLength));
        } catch (error) {
            appendOutputLine(SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        }
    }

    function onDisconnect() {
        setBusy(true);
        WinJS.log && WinJS.log("Closing", "sample", "status");
        closeSocket();
        setBusy(false);
    }

    function onClosed(e) {
        appendOutputLine("Closed; Code: " + e.code + " Reason: " + e.reason);
        if (messageWebSocket) {
            closeSocket();
            updateVisualState();
        }
    }

    function closeSocket() {
        if (messageWebSocket) {
            try {
                messageWebSocket.close(1000, "Closed due to user request.");
            } catch (error) {
                appendOutputLine(SdkSample.buildWebSocketError(error));
                appendOutputLine(error.message);
            }
            messageWebSocket = null;
        }
        if (messageWriter) {
            messageWriter.close();
            messageWriter = null;
        }
    }

    function appendOutputLine(text) {
        outputField.innerText += "\r\n" + text;
    }

})();
