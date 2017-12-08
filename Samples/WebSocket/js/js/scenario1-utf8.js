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
    var secureWebSocketCheckBox;
    var connectButton;
    var disconnectButton;
    var inputField;
    var sendButton;
    var outputField;

    var page = WinJS.UI.Pages.define("/html/scenario1-utf8.html", {
        ready: function (element, options) {
            serverAddressField = document.getElementById("serverAddressField");
            secureWebSocketCheckBox = document.getElementById("secureWebSocketCheckBox");
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
        secureWebSocketCheckBox.disabled = busy || messageWebSocket;
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

        // When connecting to wss:// endpoint, the OS by default performs validation of
        // the server certificate based on well-known trusted CAs. We can perform additional custom
        // validation if needed.
        if (secureWebSocketCheckBox.checked)
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

            // Add event handler to listen to the ServerCustomValidationRequested event. This enables performing
            // custom validation of the server certificate. The event handler must implement the desired 
            // custom certificate validation logic.
            messageWebSocket.addEventListener("servercustomvalidationrequested", onServerCustomValidationRequested);

            // Certificate validation occurs only for secure connections.
            if (server.schemeName !== "wss")
            {
                appendOutputLine("Note: Certificate validation is performed only for the wss: scheme.");
            }
        }

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

    function onServerCustomValidationRequested(args) {
        // In order to call async APIs in this handler, you must first take a deferral and then
        // release it once you are done with the operation. The "using" statement
        // ensures that the deferral completes when control leaves the method.
        var deferral = args.getDeferral();

        // Get the server certificate and certificate chain from the args parameter.
        SdkSample.areCertificateAndCertChainValidAsync(args.serverCertificate, args.serverIntermediateCertificates).then(function (isValid) {
            if (isValid) {
                appendOutputLine("Custom validation of server certificate passed.");
            } else {
                args.reject();
                appendOutputLine("Custom validation of server certificate failed.");
            }
            deferral.complete();
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
        if (messageWriter) {
            // In order to reuse the socket with another DataWriter, the socket's output stream needs to be detached.
            // Otherwise, the DataWriter's destructor will automatically close the stream and all subsequent I/O
            // operations invoked on the socket's output stream will fail with RO_E_CLOSED.
            //
            // This is only added for completeness, as this sample closes the socket in the very next code block.
            messageWriter.detachStream();
            messageWriter.close();
            messageWriter = null;
        }

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
