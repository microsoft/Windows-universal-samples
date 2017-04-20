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

    var ChainValidationResult = Windows.Security.Cryptography.Certificates.ChainValidationResult;

    // Local variables
    var streamWebSocket;
    var busy;

    // DOM elements
    var serverAddressField;
    var secureWebSocketCheckBox;
    var startButton;
    var stopButton;
    var outputField;

    var page = WinJS.UI.Pages.define("/html/scenario2-binary.html", {
        ready: function (element, options) {
            serverAddressField = document.getElementById("serverAddressField");
            secureWebSocketCheckBox = document.getElementById("secureWebSocketCheckBox");
            startButton = document.getElementById("startButton");
            stopButton = document.getElementById("stopButton");
            outputField = document.getElementById("outputField");

            startButton.addEventListener("click", onStart, false);
            stopButton.addEventListener("click", onStop, false);

            updateVisualState();
        },
        unload: function () {
            closeSocket();
        }
    });

    function updateVisualState() {
        serverAddressField.disabled = busy || streamWebSocket;
        secureWebSocketCheckBox.disabled = busy || streamWebSocket;
        startButton.disabled = busy || streamWebSocket;
        stopButton.disabled = busy || !streamWebSocket;
    }

    function setBusy(value) {
        busy = value;
        updateVisualState();
    }

    function onStart() {
        setBusy(true);
        startAsync().done(function () {
            setBusy(false);
        });
    }

    function startAsync() {
        // Validating the URI is required since it was received from an untrusted source (user
        // input). The URI is validated by calling validateAndCreateUri() that will return null
        // for strings that are not valid WebSocket URIs.
        // Note that when enabling the text box users may provide URIs to machines on the local network
        // or internet. In these cases the app requires the "Home or Work Networking" or
        // "Internet (Client)" capability respectively.
        var server = SdkSample.validateAndCreateUri(serverAddressField.value);
        if (!server) {
            return WinJS.Promise.wrap();
        }

        streamWebSocket = new Windows.Networking.Sockets.StreamWebSocket();
        streamWebSocket.addEventListener("closed", onClosed);

        // When connecting to wss:// endpoint, the OS by default performs validation of
        // the server certificate based on well-known trusted CAs. We can perform additional custom
        // validation if needed.
        if (secureWebSocketCheckBox.checked) {
            // WARNING: Only test applications should ignore SSL errors.
            // In real applications, ignoring server certificate errors can lead to Man-In-The-Middle
            // attacks. (Although the connection is secure, the server is not authenticated.)
            // Note that not all certificate validation errors can be ignored.
            // In this case, we are ignoring these errors since the certificate assigned to the localhost
            // URI is self-signed and has subject name = fabrikam.com
            streamWebSocket.control.ignorableServerCertificateErrors.push(
                ChainValidationResult.untrusted,
                ChainValidationResult.invalidName);

            // Add event handler to listen to the ServerCustomValidationRequested event. This enables performing
            // custom validation of the server certificate. The event handler must implement the desired 
            // custom certificate validation logic.
            streamWebSocket.addEventListener("servercustomvalidationrequested", onServerCustomValidationRequested);

            // Certificate validation occurs only for secure connections.
            if (server.schemeName !== "wss") {
                appendOutputLine("Note: Certificate validation is performed only for the wss: scheme.");
            }
        }

        appendOutputLine("Connecting to: " + server.absoluteUri);

        return streamWebSocket.connectAsync(server).then(function () {
            WinJS.log && WinJS.log("Connected", "sample", "status");

            // Start a loop to continuously read incoming data.
            receiveData(streamWebSocket);

            // Start a loop to continuously write outgoing data.
            sendData(streamWebSocket);
        }, function (error) {
            streamWebSocket.close();
            streamWebSocket = null;

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

    function sendData(activeSocket) {
        var dataWriter = new Windows.Storage.Streams.DataWriter(activeSocket.outputStream);
        var bytesSent = 0;
        var data = "Hello World";

        function loopAsync() {
            if (streamWebSocket != activeSocket) {
                // Our socket is no longer active. Stop sending.
                return;
            }
            var size = dataWriter.measureString(data);

            dataWriter.writeString(data);
            return dataWriter.storeAsync().then(function () {
                bytesSent += size;
                dataSent.innerText = bytesSent;

                // Add a 1 second delay so the user can see what's going on.
                return WinJS.Promise.timeout(1000);
            }).then(loopAsync);
        }

        loopAsync().then(function () {
        }, function (error) {
            appendOutputLine("During write: " + SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        }).done(function () {
            dataWriter.close();
        });
    }

    function receiveData(activeSocket) {
        var dataReader = new Windows.Storage.Streams.DataReader(activeSocket.inputStream);
        var bytesReceived = 0;

        // When buffering, return as soon as any data is available.
        dataReader.inputStreamOptions = Windows.Storage.Streams.InputStreamOptions.partial;

        function loopAsync() {
            if (streamWebSocket != activeSocket) {
                // Our socket is no longer active. Stop reading.
                appendOutputLine("Background read stopped.");
                return;
            }
            // Buffer as much data as you require for your protocol.
            return dataReader.loadAsync(100).then(function (sizeBytesRead) {
                bytesReceived += sizeBytesRead;
                dataReceived.innerText = bytesReceived;

                var incomingBytes = new Array(sizeBytesRead);
                dataReader.readBytes(incomingBytes);

                // Do something with the data.
                // Alternatively you can use DataReader to read out individual booleans,
                // ints, strings, etc.

                // Start another read.
                return loopAsync();
            });
        }

        loopAsync().then(function () {
        }, function (error) {
            appendOutputLine("During read: " + SdkSample.buildWebSocketError(error));
            appendOutputLine(error.message);
        }).done(function () {
            dataReader.close();
        });
    }

    function onStop() {
        setBusy(true);
        WinJS.log && WinJS.log("Closing", "sample", "status");
        closeSocket();
        setBusy(false);
    }

    function onClosed(e) {
        appendOutputLine("Closed; Code: " + e.code + " Reason: " + e.reason);
        if (streamWebSocket === e.target) {
            closeSocket();
            updateVisualState();
        }
    }

    function closeSocket() {
        if (streamWebSocket) {
            try {
                streamWebSocket.close(1000, "Closed due to user request.");
            } catch (error) {
                WinJS.log && WinJS.log(SdkSample.buildWebSocketError(error), "sample", "error");
                appendOutputLine(error.message);
            }
            streamWebSocket = null;
        }
    }

    function appendOutputLine(text) {
        outputField.innerText += "\r\n" + text;
    }
})();
