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
    var MessageWebSocketReceiveMode = Windows.Networking.Sockets.MessageWebSocketReceiveMode;
    var UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding;
    var ChainValidationResult = Windows.Security.Cryptography.Certificates.ChainValidationResult;

    // Local variables
    var messageWebSocket;
    var busy;

    // DOM elements
    var serverAddressField;
    var connectButton;
    var disconnectButton;
    var inputField;
    var endOfMessageCheckBox;
    var sendButton;
    var outputField;

    var page = WinJS.UI.Pages.define("/html/scenario4-partialReadWrite.html", {
        ready: function (element, options) {
            serverAddressField = document.getElementById("serverAddressField");
            connectButton = document.getElementById("connectButton");
            disconnectButton = document.getElementById("disconnectButton");
            inputField = document.getElementById("inputField");
            endOfMessageCheckBox = document.getElementById("endOfMessageCheckBox");
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
        // or internet. In these cases the app requires the "Private Networks (Client and Server)" or
        // "Internet (Client)" capability respectively.
        var server = SdkSample.validateAndCreateUri(serverAddressField.value);
        if (!server) {
            return WinJS.Promise.wrap();
        }

        // Set up the socket data format and callbacks
        messageWebSocket = new MessageWebSocket();
        messageWebSocket.control.messageType = SocketMessageType.utf8;

        // To support receiving event notifications for partial messages, you must set this receive mode.
        // If you do not set this mode, you will only receive notifications for complete messages; which is
        // the default behavior. Setting to Partial allows us to process partial data as soon as it arrives,
        // as opposed to waiting until the EndOfMessage to process the entire data.
        messageWebSocket.control.receiveMode = MessageWebSocketReceiveMode.partialMessage;

        messageWebSocket.addEventListener("messagereceived", onMessageReceived);
        messageWebSocket.addEventListener("closed", onClosed);

        if (server.schemeName == "wss")
        {
            // WARNING: Only test applications should ignore SSL errors.
            // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
            // attacks. (Although the connection is secure, the server is not authenticated.)
            // Note that not all certificate validation errors can be ignored.
            // In this case, we are ignoring these errors since the certificate assigned to the localhost
            // URI is self-signed and has subject name = fabrikam.com
            messageWebSocket.control.ignorableServerCertificateErrors.push(
                ChainValidationResult.untrusted,
                ChainValidationResult.invalidName);
        }

        appendOutputLine("Connecting to: " + server.absoluteUri);

        return messageWebSocket.connectAsync(server).then(function () {
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
        
        // Buffer any data we want to send.
        var messageWriter = new Windows.Storage.Streams.DataWriter();
        messageWriter.writeString(message);
        var buffer = messageWriter.detachBuffer();

        // Send the data as one complete message.
        var asyncTask = null;

        if (endOfMessageCheckBox.checked == true) {
            appendOutputLine("Sending end of message: " + message);
            asyncTask = messageWebSocket.sendFinalFrameAsync(buffer);
        }
        else {
            appendOutputLine("Sending partial message: " + message);
            asyncTask = messageWebSocket.sendNonfinalFrameAsync(buffer);
        }

        return asyncTask.then(function() {
            WinJS.log && WinJS.log("Send Complete", "sample", "status");
        }, function(error) {
            appendOutputLine(SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        });
    }

    function onMessageReceived(args) {

        var partialOrCompleted = "Partial";

        if (args.isMessageComplete) {
            partialOrCompleted = "Complete";
        }

        appendOutputLine(partialOrCompleted + " message received; Type: " + SdkSample.lookupEnumName(SocketMessageType, args.messageType));

        // The incoming message is already buffered.
        var reader = args.getDataReader();
        reader.unicodeEncoding = UnicodeEncoding.utf8;

        try {
            // Note that it may be possible for partial UTF8 messages to be split between a character if it
            // extends multiple bytes. We avoid this by using only ASCII characters in this example. 
            // Should your application use multi-byte characters, you will need to do checks for broken characters.
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
    }

    function appendOutputLine(text) {
        outputField.innerText += "\r\n" + text;
    }

})();
